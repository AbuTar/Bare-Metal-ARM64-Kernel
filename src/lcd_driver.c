#include "lcd_driver.h"
#include "spi.h"
#include "gpio.h"

extern void delay_ms(uint32_t ms);

static void write_register(uint8_t cmd, const uint8_t *data, uint32_t length){
    /*  Had to rewrite this code to not let the Pi pulse the line automatically.
        The following signals are needed to hardware to control the display:
        - CS LOW  : Wakes up the display controller to listen to the SPI bus
        - DC LOW  : Signals that the incoming byte is a hardware command
        - DC HIGH : Signals that the incoming bytes are configuration or pixel data
        - CS HIGH : Closes the transaction and tells the display to execute it
    */
    mmio_write(GPCLR0, PIN_CS);
    mmio_write(GPCLR0, PIN_DC);
    spi_send_byte(cmd);
    
    if (length > 0) {
        mmio_write(GPSET0, PIN_DC);
        spi_send_buffer(data, length);
    }

    mmio_write(GPSET0, PIN_CS);
}

static void set_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    write_register(0x2A, (uint8_t[]){ x0 >> 8, x0 & 0xFF, x1 >> 8, x1 & 0xFF }, 4);
    write_register(0x2B, (uint8_t[]){ y0 >> 8, y0 & 0xFF, y1 >> 8, y1 & 0xFF }, 4);

    // Sends (0x2C) to alert display data is incoming
    mmio_write(GPCLR0, PIN_CS);
    mmio_write(GPCLR0, PIN_DC);
    spi_send_byte(0x2C);
    mmio_write(GPSET0, PIN_CS);
}

void lcd_init(void) {
    spi_init();

    // Hardware Reset
    mmio_write(GPSET0, PIN_RESET);
    delay_ms(10);
    mmio_write(GPCLR0, PIN_RESET);
    delay_ms(10);
    mmio_write(GPSET0, PIN_RESET);
    delay_ms(120);

    // Software Reset + Delay
    write_register(0x01, 0, 0);
    delay_ms(150);
    write_register(0x11, 0, 0);
    delay_ms(150);

    /*  For the LCD I need to make use of MAC (Memory Access Control)
        - 0x00 is RGB but if the colours show as one another I can shift to BGR (also common)
        - Colour mode needs ot be set to RGB565 (0x55)
        - Inversion needs to be turned off so RGB doesn't show as CMY*/

    write_register(0x36, (uint8_t[]){0x00}, 1);
    write_register(0x3A, (uint8_t[]){0x55}, 1);
    write_register(0x20, 0, 0); 
    
    // Normal display is turned on and tells it to user the full screen
    write_register(0x13, 0, 0);

    // Connects the memory buffer to the screen so pixels become visible
    write_register(0x29, 0, 0);
    delay_ms(150);
}

void lcd_fill_screen(uint32_t colour){
    set_window(0, 0, 239, 319);

    // Splits 24-bit RGB color number into three separate 8-bit components
    uint8_t R = (colour >> 16) & 0xFF;
    uint8_t G = (colour >> 8) & 0xFF;
    uint8_t B =  colour & 0xFF;

    // 1 Line is 240 pixels * 2 bytes = 480 bytes
    uint8_t line_buffer[240*2];

    // 240-bit RBG to RGB565
    uint16_t colour_565 = ((R & 0xF8) << 8) |
                          ((G & 0xFC) << 3) |
                          (B >> 3);

    for (int i = 0; i < 240; i++) {
        line_buffer[i * 2]     = colour_565 >> 8;
        line_buffer[i * 2 + 1] = colour_565 & 0xFF;
    }


    // Send bulk pixel data
    mmio_write(GPCLR0, PIN_CS);
    mmio_write(GPSET0, PIN_DC);

    for (int row = 0; row < 320; row++) {
        spi_send_buffer(line_buffer, sizeof(line_buffer));
    }

    mmio_write(GPSET0, PIN_CS);
}