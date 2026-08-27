/* Intended for the ILI9431 LCD*/
#ifndef LCD_DRIVER_H
#define LCD_DRIVER_H


#include <stdint.h>

#define RED   0xFF0000
#define GREEN 0x00FF00
#define BLUE  0x0000FF
#define WHITE  0xFFFFFF
#define BLACK  0x000000

void lcd_init(void);
void lcd_fill_screen(uint32_t colour);
void lcd_draw_char(char c, uint16_t x, uint16_t y, uint32_t fg_colour, uint32_t bg_colour);
void lcd_draw_string(const char *str, uint16_t x, uint16_t y, uint32_t fg_colour, uint32_t bg_colour);

#endif
