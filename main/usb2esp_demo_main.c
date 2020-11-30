#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "string.h"
#include "driver/gpio.h"
#include "usb2esp.h"

#define RX1_PIN (GPIO_NUM_16)
#define TX1_PIN (GPIO_NUM_17)
#define SS (GPIO_NUM_4)

static const int RX_BUF_SIZE = 1024;

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
}

static void tx_task(void *arg)
{
    uart_port_t UART_USB = UART_NUM_1;
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    //Wait to put pendrive
    // while(!gpio_get_level(SS)){
    //     vTaskDelay(500 / portTICK_PERIOD_MS);
    // }
    //Set Baud Rate (default = 9600)
    //setBaud(UART_USB, "9600"); 

    //Set Time and Date
    setTime(UART_USB, "12:00:00");
    setDate(UART_USB, "2020-11-05");

    /* Example modes
    ex = 1 -> Create file and write
    ex = 2 -> Read files
    ex = 3 -> Copy, delete and rename files
    ex = 4 -> Directories operations
    */
    int ex = 1; 
    if(ex==1){
        delFile(UART_USB, "filename.txt");
        createFile(UART_USB, "filename.txt");
        for(int i=0;i<3;i++){
            char *mystring = "Writing some stuff";
            writeOnFile(UART_USB, mystring, strlen(mystring));
        }
        closeFile(UART_USB);

        appendFile(UART_USB, "filename.txt");
        for(int i=0;i<3;i++){
            char *mystring = "More stuff";
            writeOnFile(UART_USB, mystring, strlen(mystring));
        }
        closeFile(UART_USB);
    }
    else if(ex==2){
        fileSize(UART_USB, "filename.txt", LINE);
        readFile(UART_USB, "filename.txt");
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

void app_main(void)
{
    init();
    xTaskCreate(rx_task, "uart_rx_task", 1024*2, NULL, configMAX_PRIORITIES, NULL);
    xTaskCreate(tx_task, "uart_tx_task", 1024*2, NULL, configMAX_PRIORITIES-1, NULL);
}
