#ifndef UART_H
#define UART_H

#include <stdint.h>
#include "gpio.h"

// UART Register Addresses
#define AUX_BASE            (MMIO_BASE + 0x215000)
#define AUX_ENABLES         (AUX_BASE + 0x04)
#define AUX_MU_IO_REG       (AUX_BASE + 0x40)
#define AUX_MU_IER_REG      (AUX_BASE + 0x44)
#define AUX_MU_IIR_REG      (AUX_BASE + 0x48)
#define AUX_MU_LCR_REG      (AUX_BASE + 0x4C)
#define AUX_MU_MCR_REG      (AUX_BASE + 0x50)
#define AUX_MU_LSR_REG      (AUX_BASE + 0x54)
#define AUX_MU_CNTL_REG     (AUX_BASE + 0x60)
#define AUX_MU_BAUD_REG     (AUX_BASE + 0x68)

void uart_init(void);
void uart_send(char c);
char uart_receive(void);
void uart_send_string(const char* str);

#endif