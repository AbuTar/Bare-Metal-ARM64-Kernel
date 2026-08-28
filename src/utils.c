#include "utils.h"

/*  This file is intended to have a lot of helper functions for
    modules normally provided by libraries*/



static void reverse(char* string, int length){
    int start = 0;
    int end = length - 1;

    while (start < end){
        char temp = string[start];
        string[start] = string[end];
        string[end] = temp;
        start++;
        end--;
    }
}

char* itoa(int value, char* string, int base){

    /*  In order to convert a number to the string, we can use the number base
        to find a remainder which we add it to the ascii value of '0' (48)
        Moving from the least power of 10, to the greatest power of 10.
        Basically converting digits to char from the least significant bit*/

    int i = 0;
    int is_negative = 0; 

    if (value == 0){
        string[i++] = '0';
        string[i] = '\0';
        return string;
    }

    if (value < 0 && base == 10) {
        is_negative = 1;
        value = -value;
    }

    while (value != 0){
        int remainder = value % base;

        if (remainder > 9){
            // if remainder > 9, for hex values, 'a' - 'f' is needed
            string[i++] = (remainder - 10) + 'a';
        }

        else{
            string[i++] = (remainder + '0');
        }

        value = value / base;
    }

    if (is_negative) {
        string[i++] = '-';
    }

    // null-terminator
    string[i] = '\0';

    // The digits were calculated backwards, so flip the string
    reverse(string, i);

    return string;

    
}