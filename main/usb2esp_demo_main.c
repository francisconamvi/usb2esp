#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "string.h"
#include "driver/gpio.h"
#include "usb2esp.h"

#define led (GPIO_NUM_0)
#define GPIO_OUTPUT_PIN_SEL  (1ULL<<led)
#define SW_ISR          7

#define RX1_PIN (GPIO_NUM_16)
#define TX1_PIN (GPIO_NUM_17)
#define SS (GPIO_NUM_4)

static const int RX_BUF_SIZE = 1024;

int led_flag = 0;

static void IRAM_ATTR soft_isr_handler(void* arg)
{
    xt_set_intclear(1 << SW_ISR);
}

void init(void) {
    /*uart configuration*/
    const uart_config_t uart_config_1 = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };
    uart_driver_install(UART_NUM_1, RX_BUF_SIZE * 2, 0, 0, NULL, 0);
    uart_param_config(UART_NUM_1, &uart_config_1);
    uart_set_pin(UART_NUM_1, TX1_PIN, RX1_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);

    // SS pin configuration (GPIO)
    gpio_set_direction( SS, GPIO_MODE_INPUT );
	gpio_set_pull_mode( SS, GPIO_PULLUP_ONLY );
    gpio_set_direction( led, GPIO_MODE_OUTPUT );

    esp_intr_alloc(ETS_INTERNAL_SW0_INTR_SOURCE, 0, soft_isr_handler, NULL, NULL); 
}

static void tx_task(void *arg)
{
    uart_port_t UART_USB = UART_NUM_1;
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    closeFile(UART_USB);
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    //Wait to put pendrive
    // while(!gpio_get_level(SS)){
    //     vTaskDelay(500 / portTICK_PERIOD_MS);
    // }

    setBaud(UART_USB, "57600");
    const uart_config_t uart_config_1 = {
        .baud_rate = 57600,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_APB,
    };
    uart_param_config(UART_NUM_1, &uart_config_1);

    //Set Time and Date
    setTime(UART_USB, "12:00:00");
    setDate(UART_USB, "2020-11-05");

    /* Example modes
    ex = 1 -> Create file and write
    ex = 2 -> Read files
    ex = 3 -> Copy, delete and rename files
    ex = 4 -> Directories operations
    */
    int ex = 2;
    if(ex==1){
        delFile(UART_USB, "filename.txt");
        createFile(UART_USB, "filename.txt");
        for(int i=0;i<3;i++){
            char *mystring = "Writing some stuff";
            writeOnFile(UART_USB, mystring, strlen(mystring));
        }
        closeFile(UART_USB);

        appendFile(UART_USB, "filename.txt");

        long int n = 128000;
        char *mystring = (char*)malloc((n+1)*sizeof(char));
        for(long int i = 0; i<n; i++){
            if(i%10 == 0){
                mystring[i] = '\n';
            }
            else if(i%10 == 1){
                mystring[i] = '\r';
            }
            else{
                char aux[2];
                itoa(i%10, aux, 10);
                mystring[i] = aux[0];
            }
        }
        mystring[n] = '\0';
        printf("%d\n", strlen(mystring));
        writeOnFile(UART_USB, mystring, n+1);
        closeFile(UART_USB);
    }
    else if(ex==2){
        fileSize(UART_USB, "filename.txt", LINE);
        // readFile(UART_USB, "filename.txt"); //exemple file is too big
        readLine(UART_USB, "filename.txt", 3);
        readNextLine(UART_USB, "filename.txt");
        readSection(UART_USB, "filename.txt", 0, 7);
        readNextSection(UART_USB, "filename.txt", 11);
    }
    else if(ex==3){
        copyFile(UART_USB, "filename.txt", "filecopy.txt");
        delFile(UART_USB, "filename.txt");
        readFile(UART_USB, "filecopy.txt");
        renameFile(UART_USB, "filecopy.txt", "renamed.txt");
    }
    else if(ex==4){
        dir(UART_USB, "");
        removeDir(UART_USB, "newdir");
        dir(UART_USB, "");
        makeDir(UART_USB, "newdir");
        dir(UART_USB, "");
        changeDir(UART_USB, "newdir");
        dir(UART_USB, "");
        createFile(UART_USB, "file.txt");
        char *ms = "Writing some stuff";
        writeOnFile(UART_USB, ms, strlen(ms));
        closeFile(UART_USB);
        changeDir(UART_USB, "..");
        dir(UART_USB, "");
    }

    while (1) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        // If pendrive is out, restart
        // if(!gpio_get_level(SS)){
        //     esp_restart();
        // }
    }
}

static void rx_task(void *arg)
{
    static const char *RX_TASK_TAG = "RX_TASK";
    esp_log_level_set(RX_TASK_TAG, ESP_LOG_INFO);
    uint8_t* data = (uint8_t*) malloc(RX_BUF_SIZE+1);
    while (1) {
        const int rxBytes = uart_read_bytes(UART_NUM_1, data, RX_BUF_SIZE, 50 / portTICK_RATE_MS);
        if (rxBytes > 0) {
            data[rxBytes] = 0;
            ESP_LOGI(RX_TASK_TAG, "Read %d bytes:\n'%s'", rxBytes, data);
        }
    }
    free(data);
}

static void bomber(void *arg)
{
    while(1){
        xt_set_intset(1 << SW_ISR);
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

static void ledt(void *arg)
{
    while(1){
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        if(led_flag){
            gpio_set_level(led, 0);
            led_flag = 0;
        }
        else{
            gpio_set_level(led, 1);
            led_flag = 1;
        }
    }
}

void app_main(void)
{
    init();
    xTaskCreate(rx_task, "uart_rx_task", 1024*2, NULL, configMAX_PRIORITIES, NULL);
    xTaskCreate(tx_task, "uart_tx_task", 1024*2, NULL, configMAX_PRIORITIES-1, NULL);
    xTaskCreate(bomber, "bomber_task", 1024*2, NULL, configMAX_PRIORITIES-1, NULL);
    xTaskCreate(ledt, "led_task", 1024*2, NULL, configMAX_PRIORITIES-2, NULL);
}
