#include "boot_sequence.h"
#include "lcd_driver.h"
#include "timer.h"

void boot_sequence(void) {
    lcd_fill_screen(BLACK);
    
    const char spinner[] = {'|', '/', '-', '\\'};

    lcd_draw_string_animated("Initialising Bare Metal OS: ", 10, 10, WHITE, BLACK, 60);
    for(int i = 0; i < 30; i++) { 
        lcd_draw_char(spinner[i % 4], 250, 10, YELLOW, BLACK);
        delay_ms(100);
    }

    lcd_draw_string("[ OK ]", 250, 10, GREEN, BLACK);
    delay_ms(800);

    // Remaining Sequence
    lcd_draw_string("Splash Sequence Commencing", 10, 20, GREEN, BLACK);
    delay_ms(1000); 
    lcd_fill_screen(BLACK);

   const char* splash_logo = 
        "    _    ____  __  __    ___  ____  \n"
        "   / \\  |  _ \\|  \\/  |   / _ \\/ ___| \n"
        "  / _ \\ | |_) | |\\/| | - | | | \\___ \\ \n"
        " / ___ \\|  _ <| |  | |   | |_| |___) |\n"
        "/_/   \\_\\_| \\_\\_|  |_|   |_____/|____/ \n";


    lcd_draw_string_animated(splash_logo, 0, 50, GREEN, BLACK, 5);

    lcd_draw_string_animated("PI Zero 2 W Edition", 84, 100, GREEN, BLACK, 50);
    for (int i = 0; i < 60; i++) {
        lcd_draw_char_scaled(spinner[i % 4], 144, 140, WHITE, BLACK, 4);
        delay_ms(100);
    }

    lcd_fill_screen(BLACK);

    lcd_draw_string_animated("ARM OS - Pi Zero 2 W Edition", 10, 10, GREEN, BLACK, 30);
    lcd_draw_string_animated("Display: ST7789 LCD ", 10, 20, YELLOW, BLACK, 30);
    lcd_draw_string_animated("Status: Online", 10, 30, BLUE, BLACK, 30);
}