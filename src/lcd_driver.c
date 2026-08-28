#include "lcd_driver.h"
#include "spi.h"
#include "gpio.h"
#include "font.h"

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
        - Inversion needs to be turned off so RGB doesn't show as CMY
        - 0x00 is Portrait
        - 0x70 is Landscape (Rotate 90 deg)
        - 0xA0 is Landscape Inverted (Rotate 270 deg)*/

    write_register(0x36, (uint8_t[]){0x70}, 1);
    write_register(0x3A, (uint8_t[]){0x55}, 1);
    write_register(0x20, 0, 0); 
    
    // Normal display is turned on and tells it to user the full screen
    write_register(0x13, 0, 0);

    // Connects the memory buffer to the screen so pixels become visible
    write_register(0x29, 0, 0);
    delay_ms(150);
}

void lcd_fill_screen(uint32_t colour){
    set_window(0, 0, 319, 239);

    // Splits 24-bit RGB color number into three separate 8-bit components
    uint8_t R = (colour >> 16) & 0xFF;
    uint8_t G = (colour >> 8) & 0xFF;
    uint8_t B =  colour & 0xFF;

    // 1 Line is 320 pixels * 2 bytes = 640 bytes
    uint8_t line_buffer[320*2];

    // 240-bit RBG to RGB565
    uint16_t colour_565 = ((R & 0xF8) << 8) |
                          ((G & 0xFC) << 3) |
                          (B >> 3);

    for (int i = 0; i < 320; i++) {
        line_buffer[i * 2]     = colour_565 >> 8;
        line_buffer[i * 2 + 1] = colour_565 & 0xFF;
    }


    // Send bulk pixel data
    mmio_write(GPCLR0, PIN_CS);
    mmio_write(GPSET0, PIN_DC);

    for (int row = 0; row < 240; row++) {
        spi_send_buffer(line_buffer, sizeof(line_buffer));
    }

    mmio_write(GPSET0, PIN_CS);
}

static uint16_t colour_to_565(uint32_t colour){
    uint8_t R = (colour >> 16) & 0xFF;
    uint8_t G = (colour >> 8) & 0xFF;
    uint8_t B =  colour & 0xFF;
    return ((R & 0xF8) << 8) | ((G & 0xFC) << 3) | (B >> 3);
}

void lcd_draw_char(char c, uint16_t x, uint16_t y, uint32_t fg_colour, uint32_t bg_colour) {
    /* Only draws characters I want it to
        I need to define a small grid for where each character belongs*/
    if (c < 32 || c > 127) {
        return;
    } 
    
    set_window(x, y, x + 7, y + 7);
    uint16_t fg = colour_to_565(fg_colour);
    uint16_t bg = colour_to_565(bg_colour);
    
    // Every 8x8 grid is 64 pixels which is 128 bytes so buffer needs to be this size
    uint8_t buffer[128]; 
    int idx = 0;
    
    /* First character I can represent is a space which has value 32, so need offset.
        I also need to check if something is a background or foreground by checking if the bit is 1 or 0 */

    int font_idx = c - 32;

    for (int row = 0; row < 8; row++) {
        uint8_t pixel_row = font8x8[font_idx][row];
        
        for (int col = 0; col < 8; col++) {
            
            if (pixel_row & (0x80 >> col)) { 
                buffer[idx++] = fg >> 8;
                buffer[idx++] = fg & 0xFF;
            } else {
                buffer[idx++] = bg >> 8;
                buffer[idx++] = bg & 0xFF;
            }
        }
    }

    // Updates buffer
    mmio_write(GPCLR0, PIN_CS);
    mmio_write(GPSET0, PIN_DC);
    spi_send_buffer(buffer, 128);
    mmio_write(GPSET0, PIN_CS);
}

void lcd_draw_string(const char *str, uint16_t x, uint16_t y, uint32_t fg_colour, uint32_t bg_colour) {
    uint16_t start_x = x;
    
    /*  Move down line, and then reset x position*/
    while (*str) {
        if (*str == '\n') {
            y += 8;         
            x = start_x;
        }

        else{
            lcd_draw_char(*str, x, y, fg_colour, bg_colour);
            x += 8;
        }
        str++;
    }
}

void lcd_draw_string_animated(const char *str, uint16_t x, uint16_t y, uint32_t fg_colour, uint32_t bg_colour, uint32_t speed_ms) {
    uint16_t start_x = x;
    
    while (*str) {
        if (*str == '\n') {
            y += 8;         
            x = start_x;
        } else {
            lcd_draw_char(*str, x, y, fg_colour, bg_colour);
            x += 8;
            delay_ms(speed_ms);
        }
        str++;
    }
}

void lcd_draw_char_scaled(char c, uint16_t x, uint16_t y, uint32_t fg_colour, uint32_t bg_colour, uint8_t scale) {
    if (c < 32 || c > 127 || scale < 1) return; 
    
    uint16_t scaled_size = 8 * scale;
    set_window(x, y, x + scaled_size - 1, y + scaled_size - 1);
    
    uint16_t fg = colour_to_565(fg_colour);
    uint16_t bg = colour_to_565(bg_colour);
    
    // A buffer large enough to hold one horizontally stretched row (up to scale x10)
    uint8_t row_buffer[160]; 
    
    int font_idx = c - 32;

    mmio_write(GPCLR0, PIN_CS);
    mmio_write(GPSET0, PIN_DC);

    for (int row = 0; row < 8; row++) {
        uint8_t pixel_row = font8x8[font_idx][row];
        
        // Builds a horizontal line buy repeating pixels since they need to be stretched
        int idx = 0;
        for (int col = 0; col < 8; col++) {
            uint16_t pixel_color = (pixel_row & (0x80 >> col)) ? fg : bg;
            for (int s = 0; s < scale; s++) {
                row_buffer[idx++] = pixel_color >> 8;
                row_buffer[idx++] = pixel_color & 0xFF;
            }
        }
        
        // Stretches the lines to scale them
        for (int s = 0; s < scale; s++) {
            spi_send_buffer(row_buffer, scaled_size * 2);
        }
    }

    mmio_write(GPSET0, PIN_CS);
}