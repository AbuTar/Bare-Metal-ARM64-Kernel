#include "led_err.h"
#include "gpio.h"

extern void delay_ms(uint32_t ms);

void led_err(int err_code){
    while (1){
        for (int i = 0; i<err_code; i++){
            led_on();
            delay_ms(200);
            led_off();
            delay_ms(200);
        }

        delay_ms(500);
    }
}