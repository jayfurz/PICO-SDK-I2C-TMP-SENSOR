#include "Controller.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include <vector>


int main() {
    std::vector<uint8_t> button_pins = { BTN0, BTN1, BTN2, BTN3, BTN4, BTN5, BTN6, BTN7, ALERT_GP};
    Controller controller(button_pins);

    while (1) {
        controller.display_view();

        sleep_ms(1000);   
    }

    return 0;
}
