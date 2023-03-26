#include "View.h"
#include <stdio.h>


void View::show_config_menu() {
    printf("Configuration menu:\n");
    printf("[0] Shutdown Setting\n");
    printf("[1] Comp/Int Select\n");
    printf("[2] ALERT POLARITY\n");
    printf("[3] FAULT QUEUE\n");
    printf("[4] ADC RES\n");
    printf("[5] ONE-SHOT\n");
    printf("[x] QUIT\n");
}

void View::show_main_menu() {
    printf("Main menu:\n");
    printf("[0] I2C Bus Scan\n");
    printf("[1] Configuration Menu\n");
}

void View::show_shutdown_submenu() {
    printf("Shutdown submenu:\n");
    printf("[0] Disable Shutdown\n");
    printf("[1] Enable Shutdown\n");
    printf("[x] Back to main menu\n");
}

void View::show_device_id_menu() {
    int available_ids[] = { 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F };
    int num_options = sizeof(available_ids) / sizeof(available_ids[0]);

    printf("Change Device ID\n");
    for (int i = 0; i < num_options; i++) {
        printf("[%d] 0x%02X\n", i, available_ids[i]);
    }
    printf("[x] Return to main\n");
}

void View::show_i2c_bus_scan() {
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


// Add more functions for other submenus here