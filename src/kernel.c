#include <stddef.h>
#include <stdint.h>
#include "gpio.h"
#include "led_err.h"
#include "lcd_driver.h"
#include "timer.h"
#include "boot_sequence.h"
#include "utils.h"
#include "uart.h"

/* ARM Generic Timer Helpers (Cortex-A53)
   cntfrq_el0 = fixed frequency of the system counter (Hz)
   cntpct_el0 = current counter value (ticks since boot) */

static inline uint64_t get_timer_freq(void) {
    uint64_t freq;
    asm volatile("mrs %0, cntfrq_el0" : "=r"(freq));
    return freq;
}

static inline uint64_t get_timer_ticks(void) {
    uint64_t ticks;
    asm volatile("mrs %0, cntpct_el0" : "=r"(ticks));
    return ticks;
}

/* Busy-wait for approximately ms milliseconds, timed against the
   hardware counter rather than a fixed instruction count, so the
   delay stays accurate regardless of CPU clock speed or optimization level. */
void delay_ms(uint32_t ms) {
    uint64_t freq = get_timer_freq();
    uint64_t start = get_timer_ticks();
    uint64_t target = (freq / 1000) * ms;
    while ((get_timer_ticks() - start) < target) {
        // busy wait
    }
}

/* Initialize Onboard LED (GPIO 29) */
void led_init(void) {
    uint32_t selector = mmio_read(GPFSEL2);
    selector &= ~GPIO29_FSEL_MASK;
    selector |= GPIO29_FSEL_OUTPUT;
    mmio_write(GPFSEL2, selector);
}

void led_on(void) {
    mmio_write(GPCLR0, LED_PIN);
}

void led_off(void) {
    mmio_write(GPSET0, LED_PIN);
}

/* Kernel Entry Point called by boot.S */
void kernel_main(uint64_t dtb_ptr32, uint64_t x1, uint64_t x2, uint64_t x3) {
    (void)dtb_ptr32; (void)x1; (void)x2; (void)x3; // Suppress unused warnings

    lcd_init();
    lcd_fill_screen(BLACK);
    boot_sequence();
    uart_send_string("Serial Interface is Online!\n");
    lcd_draw_string("Waiting for input...", 10, 40, WHITE, BLACK);

    int cursor_x = 10;
    int cursor_y = 70;

    while(1) {
        char character = uart_receive();
        uart_send(character);
        lcd_draw_char(character, cursor_x, cursor_y, WHITE, BLACK);
        cursor_x += 8;

        if (cursor_x >= 310){
            cursor_x = 10;
            cursor_y += 10;
        }


    }
}