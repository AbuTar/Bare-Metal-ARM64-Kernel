#include <stddef.h>
#include <stdint.h>
#include "gpio.h"
#include "led_err.h"
#include "lcd_driver.h"
#include "timer.h"

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

    // led_init();
    // led_off();
    // lcd_init();

    // while (1) {
    //     // Core loop
    //     led_on();
    //     lcd_fill_screen(RED);
    //     delay_ms(2000);
        
    //     led_off();
    //     lcd_fill_screen(GREEN);
    //     delay_ms(2000);

    //     led_on();
    //     lcd_fill_screen(BLUE);
    //     delay_ms(2000);

    //     led_off();
    //     delay_ms(2000);
    // }

    lcd_init();
    lcd_fill_screen(BLACK);

    // Initialisation Sequence

    lcd_draw_string("Starting Bare Metal OS...", 10, 10, WHITE, BLACK);
    delay_ms(600);
    lcd_draw_string("Initialising SPI Bus...", 10, 20, WHITE, BLACK);
    delay_ms(400);
    lcd_draw_string("[ OK ]", 300, 20, GREEN, BLACK);
    lcd_draw_string("Probing ST7789 Display...", 10, 30, WHITE, BLACK);
    delay_ms(500);
    lcd_draw_string("[ OK ]", 300, 30, GREEN, BLACK);
    lcd_draw_string("System Initialisation Complete.", 10, 50, GREEN, BLACK);
    delay_ms(1200);

    lcd_fill_screen(BLACK);
    
    // Print text
    lcd_draw_string("BARE METAL PI ZERO 2 W", 10, 10, GREEN, BLACK);
    lcd_draw_string("ST7789 Display Driver", 10, 20, WHITE, BLACK);
    lcd_draw_string("Status: Online", 10, 30, BLUE, BLACK);

    while(1) {
        // Blink LED to prove the kernel is alive
        led_on();
        delay_ms(1000);
        led_off();
        delay_ms(1000);
    }
}