#include "View.h"
#include <stdio.h>
#include <iostream>
#include <iomanip>

void clear_console() {
    std::cout << "\033[2J\033[H";
}

void View::show_config_menu() {
    printf("Configuration menu:\n");
    printf("[0] Shutdown Setting\n");
    printf("[1] Comp/Int Select (not implemented)\n");
    printf("[2] ALERT POLARITY (not implemented)\n");
    printf("[3] FAULT QUEUE (not implemented)\n");
    printf("[4] ADC RES (not implemented)\n");
    printf("[5] ONE-SHOT (not implemented)\n");
    printf("[x] QUIT\n");
}

void View::show_main_menu() {
    printf("Main menu:\n");
    printf("[0] I2C Bus Scan\n");
    printf("[1] Configuration Menu\n");
    printf("[2] Device ID Menu\n");
    printf("[3] Temp Alarm Setting\n");
    printf("[4] Read Temperature\n");
}

void View::show_shutdown_submenu() {
    printf("Shutdown submenu:\n");
    printf("[0] Disable Shutdown\n");
    printf("[1] Enable Shutdown\n");
    printf("[x] Back to main menu\n");
}

void View::show_device_id_menu() {
    clear_console();
    int available_ids[] = { 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F };
    int num_options = sizeof(available_ids) / sizeof(available_ids[0]);

    printf("Change Device ID\n");
    for (int i = 0; i < num_options; i++) {
        printf("[%d] 0x%02X\n", i, available_ids[i]);
    }
    printf("[x] Return to main\n");
}

void View::show_i2c_bus_scan() {
    clear_console();
    scanner.scan();
}



void View::success_or_fail(bool success) {
    if (success) {
        printf("Success\n");
    }
    else {
        printf("Failure\n");
    }
}

void View::print_alert() {
    printf("Alert activated!");
}

void View::success_or_fail_id(bool success, uint8_t device_id) {
    if (success) {
        printf("Device ID successfully changed to 0x%02X\n", device_id);

    }
    else {
        printf("Failed to change device ID to 0x%02X\n", device_id);
    }
}

void View::print_temperature(float temp_c, float temp_f) {
    if (first_temp_call) {
        std::cout << std::left << std::fixed << std::setprecision(1) << std::setw(15) << "Temperature" << std::setw(7) << "C" << "F" << std::endl;
        std::cout << std::string(25, '-') << std::endl;
        first_temp_call = false;
    }
    std::cout << std::left << std::setw(15) << " " << std::setw(7) << temp_c
        << temp_f << std::endl;
}
// Add more functions for other submenus here