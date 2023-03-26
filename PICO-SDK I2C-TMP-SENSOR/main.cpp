#include "Controller.h"
#include <stdio.h>
#include "pico/stdlib.h"


int main() {
    Controller controller;

    while (1) {
        controller.display_main_menu();
        
        float temperature_c = controller.sensor.read_temperature_c();
        float temperature_f = controller.sensor.read_temperature_f();
        printf("Temperature: %.2f C (%.2f F)\n", temperature_c, temperature_f);

        sleep_ms(1000);   
    }

    return 0;
}
