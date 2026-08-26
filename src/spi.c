#include "spi.h"

void spi_init(void) {
    // GPIO 8 (CEO) to ALT MODE - pulling CS Line high and low
    // Toggling this line alerts the display when data moves
    uint32_t sel0 = mmio_read(GPFSEL0);
    sel0 &= ~GPIO8_FSEL_MASK;
    sel0 |= GPIO8_FSEL_OUTPUT;
    mmio_write(GPFSEL0, sel0);
    mmio_write(GPSET0, PIN_CS);

    // GPIO 10 (MOSI) and 11 (SCLK) to ALT MODE - SPI data/clock
    uint32_t sel1 = mmio_read(GPFSEL1);
    sel1 &= ~(GPIO10_FSEL_MASK | GPIO11_FSEL_MASK);
    sel1 |= (GPIO10_FSEL_ALT0 | GPIO11_FSEL_ALT0);
    mmio_write(GPFSEL1, sel1);

    // GPIO 24 (PWR) and 25 (RESET) to standard dig output
    uint32_t sel2 = mmio_read(GPFSEL2);
    sel2 &= ~(GPIO24_FSEL_MASK | GPIO25_FSEL_MASK);
    sel2 |= (GPIO24_FSEL_OUTPUT | GPIO25_FSEL_OUTPUT);
    mmio_write(GPFSEL2, sel2);

    // Sets clock and clears TX/RX 
    mmio_write(SPI0_CLK, 32);
    mmio_write(SPI0_CS, 0x00000030); 
}

void spi_send_byte(uint8_t data) {
    /*Sets Transfer Bit
      Waits for Space
      Push Byte to be transfered
      Wait for transfer (bit 16)
      Clear Transfer Bit*/

    mmio_write(SPI0_CS, mmio_read(SPI0_CS) | (1 << 7));
    while (!(mmio_read(SPI0_CS) & (1 << 18)));
    mmio_write(SPI0_FIFO, data);
    while (!(mmio_read(SPI0_CS) & (1 << 17))); // Clear RXD after each write
    (void)mmio_read(SPI0_FIFO); 
    while (!(mmio_read(SPI0_CS) & (1 << 16)));
    mmio_write(SPI0_CS, mmio_read(SPI0_CS) & ~(1 << 7));
}

// void spi_send_buffer(const uint8_t *buffer, uint32_t length) {

//     mmio_write(SPI0_CS, mmio_read(SPI0_CS) | (1 << 7));
//     for (uint32_t i = 0; i < length; i++) {
//         while (!(mmio_read(SPI0_CS) & (1 << 18)));
//         mmio_write(SPI0_FIFO, buffer[i]);
//         while (!(mmio_read(SPI0_CS) & (1 << 17)));
//     }

//     while (!(mmio_read(SPI0_CS) & (1 << 16)));
//     mmio_write(SPI0_CS, mmio_read(SPI0_CS) & ~(1 << 7));
// }

// void spi_send_buffer(const uint8_t *buffer, uint32_t length) {
// /*  Same as for send_byte but it's a buffer to keep TA for larger transmissions */
//     mmio_write(SPI0_CS, mmio_read(SPI0_CS) | (1 << 7));
//     for (uint32_t i = 0; i < length; i++) {
//         while (!(mmio_read(SPI0_CS) & (1 << 18))) {}
//         mmio_write(SPI0_FIFO, buffer[i]);

//         while (mmio_read(SPI0_CS) & (1 << 17)) {
//             (void)mmio_read(SPI0_FIFO);
//         }
//     }

//     while (!(mmio_read(SPI0_CS) & (1 << 16))) {}

//     while (mmio_read(SPI0_CS) & (1 << 17)) {
//         (void)mmio_read(SPI0_FIFO);
//     }

//     mmio_write(SPI0_CS, mmio_read(SPI0_CS) & ~(1 << 7));
// }

void spi_send_buffer(const uint8_t *buffer, uint32_t length) {
    // Set TA (Transfer Active)
    mmio_write(SPI0_CS, mmio_read(SPI0_CS) | (1 << 7)); 
    
    uint32_t tx_idx = 0;
    uint32_t rx_idx = 0;

    // Loop until we have both SENT everything and RECEIVED everything
    while (tx_idx < length || rx_idx < length) {
        
        // 1. Fill TX FIFO as long as there is room and we have data
        while (tx_idx < length && (mmio_read(SPI0_CS) & (1 << 18))) {
            mmio_write(SPI0_FIFO, buffer[tx_idx++]);
        }

        // 2. Drain RX FIFO as long as it has data 
        // This prevents the FIFO from filling up and stalling the hardware!
        while (rx_idx < length && (mmio_read(SPI0_CS) & (1 << 17))) {
            (void)mmio_read(SPI0_FIFO);
            rx_idx++;
        }
    }

    // Wait for DONE bit (transfer entirely finished)
    while (!(mmio_read(SPI0_CS) & (1 << 16))) {}
    
    // Clear TA
    mmio_write(SPI0_CS, mmio_read(SPI0_CS) & ~(1 << 7));
}