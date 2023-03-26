#include "TemperatureSensor.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "pico/binary_info.h"

#define TEMP_REG 0x00
#define CONFIG_REG 0x01
#define TCN75A_ADDR 0x48
#define I2C_PORT i2c0

TemperatureSensor::TemperatureSensor() {
    stdio_init_all();
    setup_i2c();
    bi_decl(bi_2pins_with_func(PICO_DEFAULT_I2C_SDA_PIN, PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C));
}

int16_t TemperatureSensor::read_temperature_raw() {
    uint8_t buffer[2];
    int16_t temperature;
    uint8_t temp_reg = TEMP_REG;

    i2c_write_blocking(I2C_PORT, TCN75A_ADDR, &temp_reg, 1, true);
    i2c_read_blocking(I2C_PORT, TCN75A_ADDR, buffer, 2, false);

    int8_t integer_part = buffer[0];
    uint8_t fractional_part = buffer[1] >> 4;
    float fraction = 0.5 * (fractional_part & 0x08 ? 1 : 0) +
        0.25 * (fractional_part & 0x04 ? 1 : 0) +
        0.125 * (fractional_part & 0x02 ? 1 : 0) +
        0.0625 * (fractional_part & 0x01 ? 1 : 0);
    temperature = (int16_t)(integer_part * 16 + (fraction * 16));

    return temperature;
}

float TemperatureSensor::read_temperature_c() {
    int16_t raw_temperature = read_temperature_raw();
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
    i2c_write_blocking(I2C_PORT, TCN75A_ADDR, &config_reg, 1, true);
    i2c_read_blocking(I2C_PORT, TCN75A_ADDR, config_data, 1, false);

    // Update the shutdown bit based on the 'shutdown' argument
    if (shutdown) {
        config_data[0] |= (1 << 0); // Set the shutdown bit
    }
    else {
        config_data[0] &= ~(1 << 0); // Clear the shutdown bit
    }

    // Write the updated configuration register value back
    uint8_t data_to_write[2] = { config_reg, config_data[0]};
    int result = i2c_write_blocking(I2C_PORT, TCN75A_ADDR, data_to_write, 2, false);
    
    // Read back the configuration register value
    uint8_t read_back_config_data[1];
    i2c_write_blocking(I2C_PORT, TCN75A_ADDR, &config_reg, 1, true);
    i2c_read_blocking(I2C_PORT, TCN75A_ADDR, read_back_config_data, 1, false);

    // Check if the updated value matches the expected value
    if (read_back_config_data[0] == config_data[0]) {
        return true;
    }
    else {
        return false;
    }
}