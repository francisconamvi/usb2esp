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

static const int RX_BUF_SIZE = 1024;

void init(void) {
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

}

static void tx_task(void *arg)
{
    uart_port_t UART_USB = UART_NUM_1;
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    //write mode functions
    setTime(UART_USB, "12:00:00");
    setDate(UART_USB, "2020-11-05");

    // Create the file ESP.TXT
    createFile(UART_USB, "filename.txt");

    // Write three lines to the file
    for(int i=0;i<3;i++){
        char *mystring = "Writing some stuff";
        writeOnFile(UART_USB, mystring, strlen(mystring));
    }
    
    //Now we need to close the file
    closeFile(UART_USB);

    //look if file was created
    dir(UART_USB, "*.txt");

    appendFile(UART_USB, "filename.txt");

    // Write three lines to the file
    for(int i=0;i<3;i++){
        char *mystring = "More stuff";
        writeOnFile(UART_USB, mystring, strlen(mystring));
    }

    closeFile(UART_USB);
    
    //read mode functions
    fileSize(UART_USB, "filename.txt", LINE);
    readFile(UART_USB, "filename.txt");
    readLine(UART_USB, "filename.txt", 3);
    readNextLine(UART_USB, "filename.txt");
    readSection(UART_USB, "filename.txt", 0, 7);
    readNextSection(UART_USB, "filename.txt", 11);

    delFile(UART_USB, "filecopy.txt");
    delFile(UART_USB, "renamed.txt");

    copyFile(UART_USB, "filename.txt", "filecopy.txt");
    delFile(UART_USB, "filename.txt");
    dir(UART_USB, "*.txt");
    readFile(UART_USB, "filecopy.txt");
    renameFile(UART_USB, "filecopy.txt", "renamed.txt");
    dir(UART_USB, "*.txt");




    while (1) {
        vTaskDelay(1000 / portTICK_PERIOD_MS);
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
