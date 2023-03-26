#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "TemperatureSensor.h"
#include "View.h"

#define LED0 6
#define LED1 7
#define BTN0 15
#define BTN1 14

class Controller {
public:
    Controller();
    void display_config_menu();
    void display_main_menu();
    void change_sensor_id();
    TemperatureSensor sensor;

    static void button_isr(uint gpio, uint32_t events);

private:
    enum State {
        MAIN_MENU,
        I2C_BUS_SCAN,
        CONFIG_MENU,
        SHUTDOWN_MENU,
        SHUTDOWN_ENABLE, 
        SHUTDOWN_DISABLE,
        DEVICE_ID_MENU,
        // Add other submenus states here
    };


    View view;
    uint32_t last_button_press;
    uint8_t green_led_pin;
    uint8_t red_led_pin;
    uint8_t button0_pin;
    uint8_t button1_pin;
    State state;

    void handle_menu_choice(char choice);
    void blink_led(uint8_t led_pin, int count, int duration_ms);
    void shutdown_enable(bool enable);
    void blink_led_on_success(bool success);
};

#endif // CONTROLLER_H
