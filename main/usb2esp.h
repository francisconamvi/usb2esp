#ifndef usb2esp_h
#define usb2esp_h
#define BYTE 1
#define LINE 0

int sendData(uart_port_t UART_NUM, const char* data);

int setTime(uart_port_t UART_NUM, char* time);

int setDate(uart_port_t UART_NUM, char* date);

int createFile(uart_port_t UART_NUM, char* filename);

int appendFile(uart_port_t UART_NUM, char* filename);

int writeOnFile(uart_port_t UART_NUM, char* s, int ssize);

int closeFile(uart_port_t UART_NUM);

int dir(uart_port_t UART_NUM, char* filename);

int fileSize(uart_port_t UART_NUM, char* filename, int mode);

int readFile(uart_port_t UART_NUM, char* filename);

int readLine(uart_port_t UART_NUM, char* filename, int line);

int readNextLine(uart_port_t UART_NUM, char* filename);

int readSection(uart_port_t UART_NUM, char* filename, int start, int len);

int readNextSection(uart_port_t UART_NUM, char* filename, int len);

int copyFile(uart_port_t UART_NUM, char* origin, char* destiny);

int delFile(uart_port_t UART_NUM, char* filename);

int renameFile(uart_port_t UART_NUM, char* oldname, char* newname);

#endif