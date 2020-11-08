#include "driver/uart.h"
#include "string.h"
#include "esp_log.h"

int sendData(uart_port_t UART_NUM, const char* data)
{
    const int len = strlen(data);
    const int txBytes = uart_write_bytes(UART_NUM, data, len);
    ESP_LOGI("TO_USB", "Wrote %d bytes - %s", txBytes, data);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    return txBytes;
}

int setTime(uart_port_t UART_NUM, char* time){
    char command[16] = "$TIME ";
    strcat(command, time);
    strcat(command, "\r");
    return sendData(UART_NUM, command);
} 

int setDate(uart_port_t UART_NUM, char* date){
    char command[20] = "$DATE ";
    strcat(command, date);
    strcat(command, "\r");
    return sendData(UART_NUM, command);
}

int createFile(uart_port_t UART_NUM, char* filename){
    char command[100] = "$WRITE ";
    strcat(command, filename);
    strcat(command, "\r");
    int r = sendData(UART_NUM, command);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    return r;
}

int appendFile(uart_port_t UART_NUM, char* filename){
    char command[40] = "$APPEND ";
    strcat(command, filename);
    strcat(command, "\r");
    int r = sendData(UART_NUM, command);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    return r;
}

int writeOnFile(uart_port_t UART_NUM, char* s, int ssize){
    char *ns = (char*)malloc((ssize+2)*sizeof(char));
    strcpy(ns,s);
    strcat(ns, "\r");
    int r = sendData(UART_NUM, ns);
    vTaskDelay(300 / portTICK_PERIOD_MS);
    return r;
}

int closeFile(uart_port_t UART_NUM){
    char cz[] = { 0x1A };
    int txBytes = uart_write_bytes(UART_NUM, cz, sizeof(cz));
    vTaskDelay(500 / portTICK_PERIOD_MS); // Give time for the file to be saved and closed 
    return txBytes;
}

int dir(uart_port_t UART_NUM, char* filename){
    char command[40] = "$DIR ";
    strcat(command, filename);
    strcat(command, "\r");
    int r = sendData(UART_NUM_1, command);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    return r;
}

int fileSize(uart_port_t UART_NUM, char* filename, int mode){
    char command[50] = "$SIZE ";
    strcat(command, filename);
    if(mode){
        strcat(command, " BYTE\r");
    }
    else{
        strcat(command, " LINE\r");
    }
    return sendData(UART_NUM, command); 
}


int readFile(uart_port_t UART_NUM, char* filename){
    char command[50] = "$TYPE ";
    strcat(command, filename);
    strcat(command, "\r");
    int r = sendData(UART_NUM, command);
    vTaskDelay(300 / portTICK_PERIOD_MS);
    return r;
}

int readLine(uart_port_t UART_NUM, char* filename, int line){
    char command[50] = "$READ ";
    strcat(command, filename);
    char sline[5];
    itoa(line, sline,10);
    strcat(command, " ");
    strcat(command, sline);
    strcat(command, "\r");
    int r = sendData(UART_NUM, command);
    vTaskDelay(300 / portTICK_PERIOD_MS);
    return r;
}

int readNextLine(uart_port_t UART_NUM, char* filename){
    char command[50] = "$READ ";
    strcat(command, filename);
    strcat(command, " n");
    strcat(command, "\r");
    int r = sendData(UART_NUM, command);
    vTaskDelay(300 / portTICK_PERIOD_MS);
    return r; 
}

int readSection(uart_port_t UART_NUM, char* filename, int start, int len){
    char command[50] = "$READ ";
    strcat(command, filename);
    strcat(command, " ");
    char sstart[5];
    itoa(start, sstart,10);
    strcat(command, sstart);
    strcat(command, " ");
    char slen[5];
    itoa(len, slen,10);
    strcat(command, slen);
    strcat(command, "\r");
    int r = sendData(UART_NUM, command);
    vTaskDelay(300 / portTICK_PERIOD_MS);
    return r;
}

int readNextSection(uart_port_t UART_NUM, char* filename, int len){
    char command[50] = "$READ ";
    strcat(command, filename);
    strcat(command, " n ");
    char slen[5];
    itoa(len, slen,10);
    strcat(command, slen);
    strcat(command, "\r");
    int r = sendData(UART_NUM, command);
    vTaskDelay(300 / portTICK_PERIOD_MS);
    return r;
}

int copyFile(uart_port_t UART_NUM, char* origin, char* destiny){
    char command[50] = "$COPY ";
    strcat(command, origin);
    strcat(command, " ");
    strcat(command, destiny);
    strcat(command, "\r");
    int r = sendData(UART_NUM, command);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    return r;
}

int delFile(uart_port_t UART_NUM, char* filename){
    char command[50] = "$DEL ";
    strcat(command, filename);
    strcat(command, "\r");
    int r = sendData(UART_NUM, command);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    return r;
}

int renameFile(uart_port_t UART_NUM, char* oldname, char* newname){
    char command[60] = "$REN ";
    strcat(command, oldname);
    strcat(command, " ");
    strcat(command, newname);
    strcat(command, "\r");
    int r = sendData(UART_NUM, command);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    return r;
}

int makeDir(uart_port_t UART_NUM, char* dirname){
    char command[50] = "$MD ";
    strcat(command, dirname);
    strcat(command, "\r");
    int r = sendData(UART_NUM, command);
    vTaskDelay(1500 / portTICK_PERIOD_MS);
    return r;
}

int changeDir(uart_port_t UART_NUM, char* dirname){
    char command[50] = "$CD ";
    strcat(command, dirname);
    strcat(command, "\r");
    int r = sendData(UART_NUM, command);
    vTaskDelay(1500 / portTICK_PERIOD_MS);
    return r;
}

int removeDir(uart_port_t UART_NUM, char* dirname){
    char command[50] = "$RD ";
    strcat(command, dirname);
    strcat(command, "\r");
    int r = sendData(UART_NUM, command);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    return r;
}