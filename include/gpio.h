#ifndef GPIO_H
#define GPIO_H

/* To make the other files more legible, I've included bitmask patterns as well*/


#include<stdint.h>

// Peripheral Base Addresses
#define MMIO_BASE           0x3F000000
#define GPIO_BASE           (MMIO_BASE + 0x200000)

// GPIO Function Select Registers
#define GPFSEL0             (GPIO_BASE + 0x00)
#define GPFSEL1             (GPIO_BASE + 0x04)
#define GPFSEL2             (GPIO_BASE + 0x08)

// GPIO Output Set / Clear Registers
#define GPSET0              (GPIO_BASE + 0x1C)
#define GPCLR0              (GPIO_BASE + 0x28)

// GPIO Pull-up/down Registers
#define GPPUD               (GPIO_BASE + 0x94)
#define GPPUDCLK0           (GPIO_BASE + 0x98)

// Onboard ACT LED (GPIO 29) Configuration
#define LED_PIN             (1 << 29)
#define GPIO29_FSEL_MASK    (7 << 27)
#define GPIO29_FSEL_OUTPUT  (1 << 27)

// Display Control Pin Mask
#define PIN_DC              (1 << 24)
#define PIN_RESET           (1 << 25)

/*  If the PI is allowed to automatically control the CS Pin
    (using ALT0), it oulses the CS Line meaning when it goes high and low,
    it's actually aborting the commandes since the LCD requires CS Line to stay low*/
#define PIN_CS              (1 << 8)


// Memory-Mapped I/O Helper Functions */

static inline void mmio_write(uint32_t reg, uint32_t data) {
    *(volatile uint32_t*)(uint64_t)reg = data;
}

static inline uint32_t mmio_read(uint32_t reg) {
    return *(volatile uint32_t*)(uint64_t)reg;
}

// LED Control
void led_init(void);
void led_on(void);
void led_off(void);

#endif