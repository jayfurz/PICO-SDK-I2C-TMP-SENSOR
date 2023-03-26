#ifndef TEMPERATURE_SENSOR_H
#define TEMPERATURE_SENSOR_H
#include "pico/stdlib.h"

class TemperatureSensor {
public:
    TemperatureSensor();

    int16_t read_temperature_raw();
    float read_temperature_c();
    float read_temperature_f();
    bool change_and_verify_register();
    bool set_shutdown_bit(bool shutdown);

private:
    void setup_i2c();
};

#endif // TEMPERATURE_SENSOR_H
