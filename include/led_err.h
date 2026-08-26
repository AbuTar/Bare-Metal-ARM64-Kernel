/*  As I don't yet have access to a USB-TTL Serial Adapter 
I will use LEDs instead for debugging errors*/

    #ifndef LED_ERR_H
    #define LED_ERR_H

    #define INIT_ERR 1
    #define MEM_ERR 2
    #define SPI_ERR 3

    void led_err(int err_code);

    #endif