/* Intended for the ILI9431 LCD*/
#ifndef LCD_DRIVER_H
#define LCD_DRIVER_H


#include <stdint.h>

#define RED   0xFF0000
#define GREEN 0x00FF00
#define BLUE  0x0000FF

void lcd_init(void);
void lcd_fill_screen(uint32_t colour);

#endif
