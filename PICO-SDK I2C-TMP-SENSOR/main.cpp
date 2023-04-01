#include "Controller.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include <vector>


int main() {
    // Define the button pins to be used
    std::vector<uint8_t> button_pins = { BTN0, BTN1, BTN2, BTN3, BTN4, BTN5, BTN6, BTN7, ALERT_GP};
    
    // Create a Controller object with the specified button pins
    Controller controller(button_pins);

    // Main loop
    while (1) {
        // Call the display_view() method of the controller to handle the current state
        controller.display_view();
        // Sleep for 1000 ms (1 second) to avoid excessive processing
        sleep_ms(1000);   
    }

    // Return 0 when the program ends (this line is not reachable in this example)
    return 0;
}
