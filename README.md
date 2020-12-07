
# USB2ESP

  

## Using USB Flash Drive Reader/Writer with ESP32

  

This is a library develop to help using the USB Host Board - USB Flash Drive Reader/Writer, by hobbytronics, with ESP32.

  

The board already has a chip that process commands sent by UART, but to makes things less complicated I wrote some functions, so I wouldn't have to worry about some details evertime.

  

The [hobbytronics webpage](https://hobbytronics.co.uk/usb-host-flash-drive) provide some informations about the command that might be usefull. It also has some examples, but for Arduino.

  

## Tasks and Examples

In main/usb2esp_demo_main.c we have some tasks
 - ledt

 Task that blink a led to show esp is working
 - bomber
 
 Task that call software interruption every 10ms. The ideia is to interrupt uart as much as possible
 - rx_task
 
 Task that wait for messages from USB.
 - tx_task
 
 Task that send commands for USB.

In tx_task I wrote some examples of how to use each functions.

There are 5 examples:

- ex = 0 -> Just show help, in terminal mode. It's usefull to test Baud Rate

- ex = 1 -> Create file and write

- ex = 2 -> Read files

- ex = 3 -> Copy, delete and rename files

- ex = 4 -> Directories operations

- ex = 5 -> Write 128KB string

**Note:** In ex 5, we write a big string. We did this to show that the function `uart_write_bytes` feed WDT, and we don't have to worry about this.
  

## Functions
All functions returns the number of bytes sent. It can change in the future for something more usefull.

    int sendData(uart_port_t UART_NUM, const char* data)
    
function that send *data* by *UART_NUM*.
Internal usage.

    int  setTime(uart_port_t  UART_NUM, char*  time);

USB Breakout has internal time clock, but it has no battery so we need to set it everytime we turn it on.
*Time* need to be [hh:mm:ss] format.

    int  setDate(uart_port_t  UART_NUM, char*  date);
   
USB Breakout has internal time clock, but it has no battery so we need to set it everytime we turn it on.
Date need to be [yyyy-mm-dd] format.
    
    int  createFile(uart_port_t  UART_NUM, char*  filename);
    
Create a file in the actual directory with the name *filename*.
    
    int  appendFile(uart_port_t  UART_NUM, char*  filename);
    
Open the file in the actual directory named *filename*  .
    
    int  writeOnFile(uart_port_t  UART_NUM, char*  s, int  ssize);
    
Since the file is opened or created, write the string *s* with size *ssize* in the file.
    
    int  closeFile(uart_port_t  UART_NUM);
    
Send a control+z command, with closes files.
    
    int  dir(uart_port_t  UART_NUM, char*  filename);
    
It is similar to bash `ls`. If *filename* is "", all files are listed. If *filename* is "*.txt", only txt files are listed. 
    
    int  fileSize(uart_port_t  UART_NUM, char*  filename, int  mode);
    
Command that count file size. It can count number of bytes (mode=BYTE) or number of lines (mode=LINE).
    
    int  readFile(uart_port_t  UART_NUM, char*  filename);
    
Read the entire file with *filename*.
    
    int  readLine(uart_port_t  UART_NUM, char*  filename, int  line);
    
 Read only the line *line* in the file *filename*.
    
    int  readNextLine(uart_port_t  UART_NUM, char*  filename);
    
After using the command `readLine()`, you can use `readNextLine()` to read the next line of file *filename*.
    
    int  readSection(uart_port_t  UART_NUM, char*  filename, int  start, int  len);
    
 Read a part of the file *filename*. This part starts at byte *start* and end at byte *start*+*len*.
    
    int  readNextSection(uart_port_t  UART_NUM, char*  filename, int  len);
    
After using the command `readSection()`, you can use `readNextSection()` to read the next section of file *filename*.
For exemple:
filename.txt is "abcdef"
readSection(UART_NUM, "filename.txt", 0, 4); //UART receives "abcd"
readNextSection(UART_NUM, "filename.txt", 2); //UART receives "ef"
    
    int  copyFile(uart_port_t  UART_NUM, char*  origin, char*  destiny);
    
Copy file *origin* to file *destiny*.      
    
    int  delFile(uart_port_t  UART_NUM, char*  filename);
   
Delete file *filename*.
    
    int  renameFile(uart_port_t  UART_NUM, char*  oldname, char*  newname);
    
Rename file *oldname* to *newname*.
    
    int  makeDir(uart_port_t  UART_NUM, char*  dirname);
    
Equivalent to bash `mk`, create new directory with name *dirname*.
    
    int  changeDir(uart_port_t  UART_NUM, char*  dirname);
    
Equivalent to bash `cd`, change to directory *dirname*.
    
    int  removeDir(uart_port_t  UART_NUM, char*  dirname);
    
Remove directory *dirname*.
    
    int  setBaud(uart_port_t  UART_NUM, char*  baudrate);

Change Baud Rate to *baudrate*.
Allowed values:
[2400|4800|**9600**|14400|19200|38400|57600|115200]

## Future improvements

 - Binary mode
 - Terminal mode
