#include <stddef.h>
#include <stdint.h>

// RPi Zero 2 W MMIO Base Address and Peripheral Offsets
#define MMIO_BASE       0x3F000000

// GPIO Registers
#define GPIO_BASE       (MMIO_BASE + 0x200000)
#define GPFSEL2         (GPIO_BASE + 0x08)
#define GPSET0          (GPIO_BASE + 0x1C)
#define GPCLR0          (GPIO_BASE + 0x28)

// GPIO 29 Bitmask (Onboard LED for Pi Zero 2 W)
#define LED_PIN         (1 << 29)
#define GPIO29_FSEL_MASK    (7 << 27)  // Bits 27-29: Clears bits
#define GPIO29_FSEL_OUTPUT  (1 << 27)  // Applies pattern to bits 27-29

/* Memory-Mapped I/O Helper Functions */
static inline void mmio_write(uint32_t reg, uint32_t data) {
    *(volatile uint32_t*)(uint64_t)reg = data;
}

static inline uint32_t mmio_read(uint32_t reg) {
    return *(volatile uint32_t*)(uint64_t)reg;
}

/* Hardware Assembly Delay Loop */
static inline void delay(int32_t count) {
    asm volatile("__delay_%=: subs %[count], %[count], #1; bne __delay_%=\n"
                 : "=r"(count) : [count]"0"(count) : "cc");
}

/* Initialize Onboard LED (GPIO 29) */
void led_init(void) {
    uint32_t selector = mmio_read(GPFSEL2);
    selector &= ~GPIO29_FSEL_MASK;
    selector |= GPIO29_FSEL_OUTPUT;
    mmio_write(GPFSEL2, selector);
}

void led_on(void) {
    mmio_write(GPSET0, LED_PIN);
}

void led_off(void) {
    mmio_write(GPCLR0, LED_PIN);
}

/* Kernel Entry Point called by boot.S */
void kernel_main(uint64_t dtb_ptr32, uint64_t x1, uint64_t x2, uint64_t x3) {
    (void)dtb_ptr32; (void)x1; (void)x2; (void)x3; // Suppress unused warnings

    led_init();

    // Blink loop: LED turns ON and OFF repeatedly
    while (1) {
        led_on();
        delay(30000000);
        led_off();
        delay(30000000);
    }
}