#include "Controller.h"
#include "pico/stdlib.h"

Controller* controller_instance = nullptr;

Controller::Controller() : last_button_press(0), green_led_pin(LED0), red_led_pin(LED1), button0_pin(BTN0), button1_pin(BTN1) {
    controller_instance = this;

    gpio_init(green_led_pin);
    gpio_set_dir(green_led_pin, GPIO_OUT);
    gpio_init(red_led_pin);
    gpio_set_dir(red_led_pin, GPIO_OUT);
    gpio_init(button0_pin);
    gpio_set_dir(button0_pin, GPIO_IN);
    gpio_pull_up(button0_pin);
    gpio_set_irq_enabled_with_callback(button0_pin, GPIO_IRQ_EDGE_FALL, true, &Controller::button_isr);

    gpio_init(button1_pin);
    gpio_set_dir(button1_pin, GPIO_IN);
    gpio_pull_up(button1_pin);
    gpio_set_irq_enabled_with_callback(button1_pin, GPIO_IRQ_EDGE_FALL, true, &Controller::button_isr);


}
void Controller::button_isr(uint gpio, uint32_t events) {
    if (controller_instance != nullptr && (time_us_64() - controller_instance->last_button_press) > 200000) { // 200 ms debounce
        if (gpio == 15) { // button 0
            controller_instance->last_button_press = time_us_64();
            switch (controller_instance->state) {
            case MAIN_MENU:
                controller_instance->state = I2C_BUS_SCAN;
                break;
            case I2C_BUS_SCAN:
                controller_instance->state = MAIN_MENU;
                break;
            case CONFIG_MENU:
                controller_instance->state = SHUTDOWN_MENU;
                break;
            case SHUTDOWN_MENU:
                controller_instance->state = SHUTDOWN_DISABLE;
                break;
                // Add more cases for other submenu controller_instance->states here
            default:
                controller_instance->state = MAIN_MENU;
            }
        } else if (gpio == 14) { // button 1
            switch (controller_instance->state) {
            case MAIN_MENU:
                controller_instance->state = CONFIG_MENU;
                break;
            case SHUTDOWN_MENU:
                controller_instance->state = SHUTDOWN_ENABLE;
                break;
                // Add more cases for other submenu controller_instance->states here
            default:
                controller_instance->state = MAIN_MENU;
            }
        } else if (gpio == 13) { // button 2
            switch (controller_instance->state) {
            case MAIN_MENU:
                controller_instance->state = DEVICE_ID_MENU;
                break;

            default:
                controller_instance->state = MAIN_MENU;
            } }

    }
}

void Controller::display_main_menu() {
    switch (state) {
    case MAIN_MENU:
        view.show_main_menu();
        break;
    case I2C_BUS_SCAN:
        view.show_i2c_bus_scan();
        break;
    case CONFIG_MENU:
        view.show_config_menu();
        break;
    case SHUTDOWN_MENU:
        view.show_shutdown_submenu();
        break;
    case SHUTDOWN_DISABLE:
        shutdown_enable(false);
        controller_instance->state = CONFIG_MENU;
        break;
    case SHUTDOWN_ENABLE:
        shutdown_enable(true);
        controller_instance->state = CONFIG_MENU;
        break;
        // Add more cases for other submenu states here
    }
}


void Controller::blink_led(uint8_t led_pin, int count, int duration_ms) {
    for (int i = 0; i < count; ++i) {
        gpio_put(led_pin, 1);
        sleep_ms(duration_ms / 2);
        gpio_put(led_pin, 0);
        sleep_ms(duration_ms / 2);
    }
}

void Controller::blink_led_on_success(bool success) {
    if (success) {
        blink_led(green_led_pin, 4, 250);
    }
    else {
        blink_led(red_led_pin, 4, 250);
    }
}

void Controller::shutdown_enable(bool enable) {
    bool success = sensor.set_shutdown_bit(enable);
    view.success_or_fail(success);
    blink_led_on_success(success);
}
