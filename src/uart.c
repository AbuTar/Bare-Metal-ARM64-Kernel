#include "uart.h"

// A loop to add some delays so the hardware has time to setup
static void delay_cycles(uint32_t count) {
    while(count--) {
        asm volatile("nop");
    }
}

void uart_init(void){
    
    /* This function does a series of things:
        1) Enable UART whilst disabling TX and RX
        2) Configures Pins 14 and 15 by clearing them and then switching function
            to ALT5 (need to write bits 010)

        3) Disable the pull resistors on both pins

        4) Configure UART by disabling interrupts, setting the 8-bit mode, set the RTS
            line to high and then receive and transmit any buffers

        5) Set the Baud Rate to 115200 which is approx 250MHz
        6) Re-enable TX and RX */
    
    mmio_write(AUX_ENABLES, mmio_read(AUX_ENABLES) | 1);
    mmio_write(AUX_MU_CNTL_REG, 0);

    // Pins 14 and 15
    uint32_t selector;
    selector = mmio_read(GPFSEL1);
    selector &= ~(7 << 12);
    selector |= (2 << 12);
    selector &= ~(7 << 15);
    selector |= (2 << 15);
    mmio_write(GPFSEL1, selector);

    mmio_write(GPPUD, 0);
    delay_cycles(150);
    mmio_write(GPPUDCLK0, (1 << 14) | (1 << 15));
    delay_cycles(150);
    mmio_write(GPPUDCLK0, 0);

    // UART config
    mmio_write(AUX_MU_IER_REG, 0);
    mmio_write(AUX_MU_LCR_REG, 3);
    mmio_write(AUX_MU_MCR_REG, 0);
    mmio_write(AUX_MU_IIR_REG, 0xC6);

    // Baud and TX, RX
    mmio_write(AUX_MU_BAUD_REG, 270);
    mmio_write(AUX_MU_CNTL_REG, 3);
}

void uart_send(char c) {
    // Wait until ready to send (Or, in otherwords, Bit 5 of LSR is 1)
    while(!(mmio_read(AUX_MU_LSR_REG) & 0x20)); 
    mmio_write(AUX_MU_IO_REG, c);
}

char uart_receive(void) {
    // Wait until data ready read (Bit 0 of LSR is 1)
    while(!(mmio_read(AUX_MU_LSR_REG) & 0x01)); 
    return (char)mmio_read(AUX_MU_IO_REG);
}

void uart_send_string(const char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == '\n') {
            uart_send('\r');
        }
        uart_send(str[i]);
    }
}