# USB2ESP

## Using USB Flash Drive Reader/Writer with ESP32

This is a library develop to help using the USB Host Board - USB Flash Drive Reader/Writer, by hobbytronics, with ESP32.

The board already has a chip that process commands sent by UART, but to makes things less complicated I wrote some functions, so I wouldn't have to worry about some details evertime.

The [hobbytronics webpage](https://hobbytronics.co.uk/usb-host-flash-drive) provide some informations about the command that might be usefull. It also has some examples, but for Arduino.

## Examples 
In main/usb2esp_demo_main.c I wrote some examples of how to use each functions.
There are 4 examples:
 - ex = 1 -> Create file and write
 - ex = 2 -> Read files
 - ex = 3 -> Copy, delete and rename files
 - ex = 4 -> Directories operations
 
Looking at each one I think you might be able to understand how to use the functions.