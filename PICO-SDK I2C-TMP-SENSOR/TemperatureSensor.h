#ifndef TEMPERATURE_SENSOR_H
#define TEMPERATURE_SENSOR_H
#include "pico/stdlib.h"

class TemperatureSensor {
public:
    TemperatureSensor();

    bool read_temperature_raw(int16_t& temperature);
    uint8_t device_addr;
    float read_temperature_c();
    float read_temperature_f();
    bool change_and_verify_register();
    bool set_shutdown_bit(bool shutdown);
    bool set_device_id(uint8_t device_id);
    bool set_temperature_alarm_limit_min(uint16_t min_temp_fixed);
    bool set_temperature_alarm_limit_max(uint16_t max_temp_fixed);
    float read_temperature_alarm_limit_min();
    float read_temperature_alarm_limit_max();
    bool handle_temp_limit_settings();


private:
    void setup_i2c();
    static const uint8_t THYST_REG = 0x02;
    static const uint8_t TSET_REG = 0x03;
};

#endif // TEMPERATURE_SENSOR_H
