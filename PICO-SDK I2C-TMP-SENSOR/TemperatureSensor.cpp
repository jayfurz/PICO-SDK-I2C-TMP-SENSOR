#include "TemperatureSensor.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "pico/binary_info.h"
#include <math.h>
#include <iostream>
#include <iomanip>
#include <string>
#include "util.h"

#define TEMP_REG 0x00
#define CONFIG_REG 0x01
#define TCN75A_ADDR 0x48
#define I2C_PORT i2c0

float string_to_float(const std::string& input) {
    float value = std::stof(input);
    return value;
}

TemperatureSensor::TemperatureSensor() {
    stdio_init_all();
    setup_i2c();
    bi_decl(bi_2pins_with_func(PICO_DEFAULT_I2C_SDA_PIN, PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C));
    device_addr = TCN75A_ADDR;
}

bool TemperatureSensor::read_temperature_raw(int16_t& temperature) {
    uint8_t buffer[2];
    uint8_t temp_reg = TEMP_REG;

    int result = i2c_write_blocking(I2C_PORT, device_addr, &temp_reg, 1, true);
    if (result < 0) {
        return false;
    }

    result = i2c_read_blocking(I2C_PORT, device_addr, buffer, 2, false);
    if (result < 0) {
        return false;
    }

    int8_t integer_part = buffer[0];
    uint8_t fractional_part = buffer[1] >> 4;
    float fraction = 0.5 * (fractional_part & 0x08 ? 1 : 0) +
        0.25 * (fractional_part & 0x04 ? 1 : 0) +
        0.125 * (fractional_part & 0x02 ? 1 : 0) +
        0.0625 * (fractional_part & 0x01 ? 1 : 0);
    temperature = (int16_t)(integer_part * 16 + (fraction * 16));

    return true;
}

float TemperatureSensor::read_temperature_c() {
    int16_t raw_temperature;
    read_temperature_raw(raw_temperature);
    return raw_temperature * 0.0625;
}

float TemperatureSensor::read_temperature_f() {
    float temperature_c = read_temperature_c();
    return temperature_c * 9.0 / 5.0 + 32;
}

void TemperatureSensor::setup_i2c() {
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
    gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);
}

bool TemperatureSensor::change_and_verify_register() {
    return false;
    // Implement code to change the configuration register
}

bool TemperatureSensor::set_shutdown_bit(bool shutdown) {
    uint8_t config_reg = CONFIG_REG;
    uint8_t config_data[1];
    
    // Read the current configuration register value
    i2c_write_blocking(I2C_PORT, device_addr, &config_reg, 1, true);
    i2c_read_blocking(I2C_PORT, device_addr, config_data, 1, false);

    // Update the shutdown bit based on the 'shutdown' argument
    if (shutdown) {
        config_data[0] |= (1 << 0); // Set the shutdown bit
    }
    else {
        config_data[0] &= ~(1 << 0); // Clear the shutdown bit
    }

    // Write the updated configuration register value back
    uint8_t data_to_write[2] = { config_reg, config_data[0]};
    int result = i2c_write_blocking(I2C_PORT, device_addr, data_to_write, 2, false);
    
    // Read back the configuration register value
    uint8_t read_back_config_data[1];
    i2c_write_blocking(I2C_PORT, device_addr, &config_reg, 1, true);
    i2c_read_blocking(I2C_PORT, device_addr, read_back_config_data, 1, false);

    // Check if the updated value matches the expected value
    if (read_back_config_data[0] == config_data[0]) {
        return true;
    }
    else {
        return false;
    }
}

bool TemperatureSensor::set_device_id(uint8_t device_id) {
    // Check if the device_id is valid (0x48 to 0x4F)
    if (device_id < 0x48 || device_id > 0x4F) {
        return false;
    }

    // Store the old device_id for later use
    uint8_t old_device_id = device_addr;

    // Update the device_id
    device_addr = device_id;

    // Perform a read operation to check if the new device_id works
    int16_t temperature;
    if (read_temperature_raw(temperature)) {
        // Communication successful, new device_id is valid
        return true;
    }
    else {
        // Communication failed, restore the old device_id
        device_addr = old_device_id;
        return false;
    }
}

bool TemperatureSensor::handle_temp_limit_settings(){
    printf("Temperature Alarm Settings (C):\n");
    printf("Your current min: %f\nYour current max: %f\n",
        read_temperature_alarm_limit_min(),
        read_temperature_alarm_limit_max());
    char input[8];
    printf("Press enter to change: ");
    get_input(input, 8);
    printf("\nEnter minimum temperature (in Celsius): ");
    char input1[8];
    get_input(input1, 8);
    int32_t output1[2];
    bool is_success = str_to_fixed_point(input1, output1);
    uint8_t min_temperature[2];
    min_temperature[0] = output1[0];
    min_temperature[1] = output1[1];

    printf("\nEnter maximum temperature (in Celsius): ");
    char input2[8];
    get_input(input2, 8);
    int32_t output2[2];
    bool is_success2 = str_to_fixed_point(input2, output2);
    uint8_t max_temperature[2];
    max_temperature[0] = output2[0];
    max_temperature[1] = output2[1];

    uint16_t min_temperature_fixed = (static_cast<uint16_t>(output1[0]) << 8) | output1[1];
    uint16_t max_temperature_fixed = (static_cast<uint16_t>(output2[0]) << 8) | output2[1];

    // Set the temperature alarm limits in the temperature sensor
    if (set_temperature_alarm_limit_min(min_temperature_fixed) &&
        set_temperature_alarm_limit_max(max_temperature_fixed) &&
        set_temperature_alarm_limit_min(min_temperature_fixed)) {
        printf("\nTemperature alarm limits updated.\n");
        return true;
    }
    else {
        printf("\nFailed to update temperature alarm limits.\n");
        return false;
    }
}

float TemperatureSensor::read_temperature_alarm_limit_max() {
    uint8_t buffer[2];
    uint8_t tset_reg = TSET_REG;

    i2c_write_blocking(I2C_PORT, device_addr, &tset_reg, 1, true);
    i2c_read_blocking(I2C_PORT, device_addr, buffer, 2, false);

    uint16_t fixedPoint = (static_cast<uint16_t>(buffer[0]) << 8) | buffer[1];
    float max_temperature = fixedPoint / 256.0f;

    return max_temperature;
}

float TemperatureSensor::read_temperature_alarm_limit_min() {
    uint8_t buffer[2];
    uint8_t thyst_reg = THYST_REG;

    i2c_write_blocking(I2C_PORT, device_addr, &thyst_reg, 1, true);
    i2c_read_blocking(I2C_PORT, device_addr, buffer, 2, false);

    uint16_t fixedPoint = (static_cast<uint16_t>(buffer[0]) << 8) | buffer[1];
    float min_temperature = fixedPoint / 256.0f;

    return min_temperature;
}

bool TemperatureSensor::set_temperature_alarm_limit_max(uint16_t max_temp_fixed) {
    uint8_t data[3];
    data[0] = TSET_REG;
    data[1] = max_temp_fixed >> 8;
    data[2] = max_temp_fixed & 0xFF;

    return i2c_write_blocking(I2C_PORT, device_addr, data, 3, false) != PICO_ERROR_GENERIC;

}

bool TemperatureSensor::set_temperature_alarm_limit_min(uint16_t min_temp_fixed) {
    uint8_t data[3];
    data[0] = THYST_REG;
    data[1] = min_temp_fixed >> 8;
    data[2] = min_temp_fixed & 0xFF;

    return i2c_write_blocking(I2C_PORT, device_addr, data, 3, false) != PICO_ERROR_GENERIC;
}
