#ifndef SPI_H
#define SPI_H

/*
    Simple Breakdown for the SPIO Driver (Raspberry Pi Zero 2 W)

    Purpose:

    Maps the SPI0 peripheral registers and defines the bitmask values for the GPIO Function Select (FSEL) registers. 

    Each GPIO pin uses a 3-bit pattern in the FSEL REG to choose it's mode
    A 3-mask (7 = 0b111) shifted to the pin's slot clears existing value
    followed by writing the mode value:
    ALT0 (4 = 0b100): Routes hardware SPI0 signals to the pin.
    OUTPUT (1 = 0b001): Pin is set as general digital OUTPUT
    
    Hardware Routing:
    - GPIO 8:  SPI0_CE0 (Chip Select)  -> ALT0 (Bits 24-26, FSEL0)
    - GPIO 10: SPI0_MOSI (Data Out)    -> ALT0 (Bits 0-2,   FSEL1)
    - GPIO 11: SPI0_SCLK (Clock)       -> ALT0 (Bits 3-5,   FSEL1)
    - GPIO 24: Software CS / Reset     -> OUTPUT (Bits 12-14, FSEL2)
    - GPIO 25: Software CS / Reset     -> OUTPUT (Bits 15-17, FSEL2)
 */

#include <stdint.h>
#include "gpio.h"

// Hardware SPI0 Registers
#define SPI0_BASE           (MMIO_BASE + 0x204000)
#define SPI0_CS             (SPI0_BASE + 0x00)
#define SPI0_FIFO           (SPI0_BASE + 0x04)
#define SPI0_CLK            (SPI0_BASE + 0x08)

// FSEL Masks
#define GPIO8_FSEL_MASK     (7 << 24)
#define GPIO8_FSEL_ALT0     (4 << 24)
#define GPIO10_FSEL_MASK    (7 << 0)
#define GPIO10_FSEL_ALT0    (4 << 0)
#define GPIO11_FSEL_MASK    (7 << 3)
#define GPIO11_FSEL_ALT0    (4 << 3)
#define GPIO24_FSEL_MASK    (7 << 12)
#define GPIO24_FSEL_OUTPUT  (1 << 12)
#define GPIO25_FSEL_MASK    (7 << 15)
#define GPIO25_FSEL_OUTPUT  (1 << 15)
#define GPIO8_FSEL_OUTPUT   (1<<24)

void spi_init(void);
void spi_send_buffer(const uint8_t *buffer, uint32_t length);
void spi_send_byte(uint8_t data);

#endif