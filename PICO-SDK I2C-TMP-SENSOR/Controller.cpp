#include "Controller.h"
#include "pico/stdlib.h"
#include <map>

// Initialize the controller instance to nullptr
Controller* controller_instance = nullptr;

// Constructor for the Controller class
Controller::Controller(const std::vector<uint8_t>& button_pins) : 
    last_button_press(0), 
    green_led_pin(LED0), 
    red_led_pin(LED1), 
    button_pins(button_pins) {

    // Set the controller instance to this object
    controller_instance = this;

    // Initialize GPIO pins for the LED lights and set their direction to output
    gpio_init(green_led_pin);
    gpio_set_dir(green_led_pin, GPIO_OUT);
    gpio_init(red_led_pin);
    gpio_set_dir(red_led_pin, GPIO_OUT);

    // Initialize state-related variables
    state_changed = true;
    has_alert = false;

    // Initialize all the buttons and set up their interrupt handlers
    for (const auto& button_pin : button_pins) {
        gpio_init(button_pin);
        gpio_set_dir(button_pin, GPIO_IN);
        gpio_pull_up(button_pin);
        gpio_set_irq_enabled_with_callback(button_pin, GPIO_IRQ_EDGE_FALL, true, &Controller::button_isr);
    }

    // Initialize pending device ID and shutdown state
    uint8_t pending_device_id = 0x48;
    bool pending_shutdown_state = false;
    int8_t last_button = -1;

    // Define state transitions for the controller
    state_transitions = {
        // MAIN_MENU transitions
        {{MAIN_MENU, 0}, I2C_BUS_SCAN},
        {{MAIN_MENU, 1}, CONFIG_MENU},
        {{MAIN_MENU, 2}, DEVICE_ID_MENU},
        {{MAIN_MENU, 3}, TEMP_ALARM_SETTING},
        {{MAIN_MENU, 4}, READ_TEMP},

        // I2C_BUS_SCAN transitions
        {{I2C_BUS_SCAN, 0}, MAIN_MENU},

        // CONFIG_MENU transitions
        {{CONFIG_MENU, 0}, SHUTDOWN_MENU},

        // SHUTDOWN_MENU transitions
        {{SHUTDOWN_MENU, 0}, SHUTDOWN_PENDING},
        {{SHUTDOWN_MENU, 1}, SHUTDOWN_PENDING},

        // DEVICE_ID_MENU transitions
        {{DEVICE_ID_MENU, 0}, DEVICE_ID_PENDING},
        {{DEVICE_ID_MENU, 1}, DEVICE_ID_PENDING},
        {{DEVICE_ID_MENU, 2}, DEVICE_ID_PENDING},
        {{DEVICE_ID_MENU, 3}, DEVICE_ID_PENDING},
        {{DEVICE_ID_MENU, 4}, DEVICE_ID_PENDING},
        {{DEVICE_ID_MENU, 5}, DEVICE_ID_PENDING},
        {{DEVICE_ID_MENU, 6}, DEVICE_ID_PENDING},
        {{DEVICE_ID_MENU, 7}, DEVICE_ID_PENDING},

        // READ_TEMP transitions for all buttons
        {{READ_TEMP, 0}, STOP_READ_TEMP},
		{{READ_TEMP, 1}, STOP_READ_TEMP},
		{{READ_TEMP, 2}, STOP_READ_TEMP},
		{{READ_TEMP, 3}, STOP_READ_TEMP},
		{{READ_TEMP, 4}, STOP_READ_TEMP},
		{{READ_TEMP, 5}, STOP_READ_TEMP},
		{{READ_TEMP, 6}, STOP_READ_TEMP},


    };

}

// Interrupt service routine for handling button presses
void Controller::button_isr(uint gpio, uint32_t events) {
    if (controller_instance != nullptr && (time_us_64() - controller_instance->last_button_press) > 200000) { // 200 ms debounce
        controller_instance->last_button = 15 - gpio;

        if (controller_instance->last_button >= 0) {
            controller_instance->state_changed = true;
            controller_instance->last_button_press = time_us_64();
            std::pair<State, int> key(controller_instance->state, controller_instance->last_button);
            auto it = controller_instance->state_transitions.find(key);
            if (it != controller_instance->state_transitions.end()) {
                controller_instance->state = it->second;
            }

            else {
                controller_instance->state = MAIN_MENU;
                controller_instance->has_alert = false;
            }
        }
        else if (gpio == ALERT_GP) {
            controller_instance->view.print_alert();
            controller_instance->has_alert = true;
        }

    }
}

// Display the appropriate view based on the current state
void Controller::display_view() {

    if (has_alert) {
        blink_led(red_led_pin, 8, 125);
    }
    else {
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
        case SHUTDOWN_PENDING:
            shutdown_enable();
            break;
        case DEVICE_ID_MENU:
            view.show_device_id_menu();
            break;
        case DEVICE_ID_PENDING:
            set_device_id();
            break;
        case TEMP_ALARM_SETTING:
            set_temperature_alarm_limits();
            break;
        case READ_TEMP:
            read_temperature();
            break;
        case STOP_READ_TEMP:
            stop_reading_temp();
            break;
            


            // Add more cases for other submenu states here
        }
    }
}

// Blink the specified LED for the given count and duration
void Controller::blink_led(uint8_t led_pin, int count, int duration_ms) {
    for (int i = 0; i < count; ++i) {
        gpio_put(led_pin, 1);
        sleep_ms(duration_ms / 2);
        gpio_put(led_pin, 0);
        sleep_ms(duration_ms / 2);
    }
}

// Blink the green LED on success, or the red LED on failure
void Controller::blink_led_on_success(bool success) {
    if (success) {
        blink_led(green_led_pin, 4, 250);
    }
    else {
        blink_led(red_led_pin, 4, 250);
    }
}

// Enable or disable shutdown based on the last_button value
void Controller::shutdown_enable() {
    
    // Enable shutdown if the last button is 1, disable if 0.
    bool success = sensor.set_shutdown_bit(last_button);
    view.success_or_fail(success);
    blink_led_on_success(success);
    state = CONFIG_MENU;
}

void Controller::set_device_id() {
    // Define a map that associates button indices to device IDs
    std::map<int, uint8_t> button_to_device_id = {
        {0, 0x48},
        {1, 0x49},
        {2, 0x4A},
        {3, 0x4B},
        {4, 0x4C},
        {5, 0x4D},
        {6, 0x4E},
        {7, 0x4F}
    };

    // Get the device ID based on the last_button index
    uint8_t device_id = button_to_device_id[last_button];

    // Set the device ID
    bool success = sensor.set_device_id(device_id);
    view.success_or_fail_id(success, device_id);
    blink_led_on_success(success);
    state = MAIN_MENU;
}

void Controller::set_temperature_alarm_limits() {
    // Display the temperature alarm settings menu and prompt the user for input
    blink_led_on_success(sensor.handle_temp_limit_settings());

    // Return to the main menu
    state = MAIN_MENU;
}

void Controller::read_temperature() {
    // Read the temperature and display it after two seconds passed
    static uint64_t last_temp_read = 0;
    if (time_us_64() - last_temp_read > 2000000) { // 2 seconds
		// Read the temperature and display it
		float temperature_celsius = sensor.read_temperature_c();
		float temperature_fahrenheit = sensor.read_temperature_f();
		view.print_temperature(temperature_celsius, temperature_fahrenheit);
		last_temp_read = time_us_64();
	}
}

void Controller::stop_reading_temp() {
    view.first_temp_call = true;
    state = MAIN_MENU;
}