#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "TemperatureSensor.h"
#include "View.h"
#include <utility>
#include <unordered_map>
#include <vector>

#define ONBOARD_LED 25
#define LED0 6
#define LED1 7
#define BTN0 15
#define BTN1 14
#define BTN2 13
#define BTN3 12
#define BTN4 11
#define BTN5 10
#define BTN6 9
#define BTN7 8
#define ALERT_GP 18

struct PairHash {
    template <typename T1, typename T2>
    std::size_t operator()(const std::pair<T1, T2>& pair) const {
        auto h1 = std::hash<T1>{}(pair.first);
        auto h2 = std::hash<T2>{}(pair.second);
        return h1 ^ (h2 << 1);
    }
};

class Controller {
public:
    Controller(const std::vector<uint8_t>& button_pins);

    void display_view();
    TemperatureSensor sensor;
    bool state_changed;

    static void button_isr(uint gpio, uint32_t events);

private:
    enum State {
        MAIN_MENU,
        I2C_BUS_SCAN,
        CONFIG_MENU,
        SHUTDOWN_MENU,
        DEVICE_ID_MENU,
        DEVICE_ID_PENDING,
        SHUTDOWN_PENDING,
        TEMP_ALARM_SETTING,
        READ_TEMP,
        STOP_READ_TEMP,
        // Add other submenus states here
    };


    View view;
    uint32_t last_button_press;
    int8_t last_button;
    uint8_t green_led_pin;
    uint8_t red_led_pin;
    std::vector<uint8_t> button_pins;
    bool has_alert;
    State state;
    std::unordered_map<std::pair<State, int>, State, PairHash> state_transitions;

    void display_config_menu();
    void handle_menu_choice(char choice);
    void blink_led(uint8_t led_pin, int count, int duration_ms);
    void shutdown_enable();
    void blink_led_on_success(bool success);
    void set_device_id();
    void read_temperature();
    void change_sensor_id();
    void set_temperature_alarm_limits();
    void stop_reading_temp();
};

#endif // CONTROLLER_H
