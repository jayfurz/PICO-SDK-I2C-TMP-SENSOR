#ifndef __UTIL_H__
#define __UTIL_H__
#include "pico/stdlib.h"

float fixedToFloat(uint8_t integerPart, uint8_t decimalPart);
void clear_screen();
float c2f(float celsius);
void get_input(char* input, int max_length);
bool str_to_fixed_point(char* input, int32_t* output);
bool two_seconds_passed();
#endif