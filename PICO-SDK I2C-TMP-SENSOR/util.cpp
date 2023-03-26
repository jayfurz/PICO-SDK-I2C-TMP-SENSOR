#include "util.h"
#include <chrono>
#include <stdio.h>
#include <string.h>

/**
 * @brief Converts a fixed-point temperature value to a floating-point value.
 *
 * This function takes an 8-bit integer part and an 8-bit decimal part that
 * represent a fixed-point temperature value and converts it to a floating-point
 * value. The integer and decimal parts are combined into a 16-bit fixed-point
 * number, where the decimal part is the least significant byte. The fixed-point
 * number is then divided by 256 to convert it to a floating-point number.
 *
 * @param integerPart The integer part of the fixed-point temperature value.
 * @param decimalPart The decimal part of the fixed-point temperature value.
 *
 * @return The temperature value as a floating-point number.
 */

float fixedToFloat(uint8_t integerPart, uint8_t decimalPart) {
    // Combine the integer and decimal parts into a 16-bit fixed point number
    uint16_t fixedPoint = ((uint16_t)integerPart << 8) | decimalPart;

    // Convert the fixed point number to a float
    float floatValue = (float)fixedPoint / 256.0f;

    return floatValue;
}

/**
 * @brief Clears the console screen.
 *
 * This function sends an escape sequence to the console to clear the screen.
 *
 * @return None.
 */
void clear_screen() { printf("\e[1;1H\e[2J"); }

/**
 * @brief Converts a temperature value from Celsius to Fahrenheit.
 *
 * This function takes a temperature value in Celsius and converts it to
 * Fahrenheit using the formula F = (C * 9/5) + 32.
 *
 * @param celsius The temperature value in Celsius.
 *
 * @return The temperature value in Fahrenheit.
 */
float c2f(float celsius) {
    float fahrenheit = (celsius * 9 / 5) + 32;
    return fahrenheit;
}

/**
 * @brief Reads user input from the console with line editing capabilities.
 *
 * This function reads user input from the console and provides line editing
 * capabilities such as backspace, delete, and arrow keys for moving the cursor.
 * The input is stored in the specified character buffer.
 *
 * @param input A character buffer for storing the user input.
 * @param max_length The maximum length of the user input (including the null
 * terminator).
 *
 * @return None.
 */
void get_input(char* input, int max_length) {
    int i = 0;
    char c;
    while (1) {
        c = getchar();
        if (c == 13) {
            break;
        }
        else if (c == 8 || c == 127) {
            if (i > 0) {
                i--;
                putchar('\b');
                putchar(' ');
                putchar('\b');
            }
        }
        else if (c == 27) {
            // Handle escape sequences
            c = getchar();
            if (c == '[') {
                c = getchar();
                if (c == 'D') {
                    // Left arrow key
                    if (i > 0) {
                        i--;
                        putchar('\b');
                    }
                }
                else if (c == 'C') {
                    // Right arrow key
                    if (i < strlen(input)) {
                        i++;
                        putchar('\x1b');
                        putchar('\x5b');
                        putchar('\x43');
                    }
                }
            }
        }
        else {
            if (i < max_length - 1) {
                input[i] = c;
                i++;
                putchar(c);
            }
        }
    }
    input[i] = '\0';
}

/**
 * @brief Converts a string representing a fixed-point number to a fixed-point
 * integer.
 *
 * This function takes a string that represents a fixed-point number in the
 * format "integer.decimal" and converts it to a fixed-point integer with a
 * 16-bit integer part and an 8-bit decimal part. The integer and decimal parts
 * are returned in an array of two integers. If the input string is not a valid
 * fixed-point number, an error message is printed and the function returns
 * false.
 *
 * @param input The input string to convert to a fixed-point number.
 * @param output An array of two integers where the integer and decimal parts of
 * the fixed-point number will be stored.
 *
 * @return Returns true if the input string is a valid fixed-point number, and
 * false otherwise.
 */
bool str_to_fixed_point(char* input, int32_t* output) {
    int32_t integer_part = 0;
    int32_t decimal_part = 0;
    int32_t decimal_place = 1;
    int32_t is_numeric = 1;
    int32_t i = 0;

    // Parse integer part
    while (input[i] != '\0' && input[i] != '.') {
        if (input[i] >= '0' && input[i] <= '9') {
            integer_part = integer_part * 10 + (input[i] - '0');
        }
        else {
            is_numeric = 0;
            break;
        }
        i++;
    }

    // Parse decimal part
    if (input[i] == '.') {
        i++;
        while (input[i] != '\0') {
            if (input[i] >= '0' && input[i] <= '9') {
                decimal_part = decimal_part * 10 + (input[i] - '0');
                decimal_place *= 10;
            }
            else {
                is_numeric = 0;
                break;
            }
            i++;
        }
    }
    else if (input[i] == '\0') {
        decimal_part =
            0;  // Set default decimal part to 0 if no decimal point found
    }
    else {
        is_numeric = 0;  // Input contains invalid character
    }

    // Check if input is numeric and print result
    if (is_numeric) {
        int result = integer_part * decimal_place + decimal_part;
        if (decimal_place == 1) {
            output[0] = (int)result;
            output[1] = 0;
            return true;
        }
        else {
            output[0] = (int)(result / 10);
            output[1] = result % 10;
            return true;
        }
    }
    else {
        printf("Error: Input is not numeric\n");
        return false;
    }
}


bool two_seconds_passed() {
    // Use a static variable to store the last time the function was called
    static std::chrono::steady_clock::time_point last_call_time = std::chrono::steady_clock::now();

    // Get the current time
    std::chrono::steady_clock::time_point current_time = std::chrono::steady_clock::now();

    // Calculate the time elapsed since the last call
    std::chrono::duration<double> elapsed_seconds = current_time - last_call_time;

    // Check if at least two seconds have passed since the last call
    if (elapsed_seconds.count() >= 2.0) {
        // Update the last call time and return true
        last_call_time = current_time;
        return true;
    }
    else {
        // Less than two seconds have passed, return false
        return false;
    }
}
