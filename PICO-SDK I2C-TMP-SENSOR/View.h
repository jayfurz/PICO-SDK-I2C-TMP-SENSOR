#ifndef VIEW_H
#define VIEW_H

#include "TemperatureSensor.h"
#include "I2CBusScanner.h"

class View {
public:
    void show_main_menu();
    void show_i2c_bus_scan();
    void show_config_menu();
    void show_shutdown_submenu();
    void success_or_fail(bool success);
    void show_device_id_menu();
    void success_or_fail_id(bool success, uint8_t device_id);
    void print_temperature(float temp_c, float temp_f);
    bool first_temp_call = true;
    void print_alert();

private:
    I2CBusScanner scanner;
    // Add any private member functions or variables needed for the view
};

#endif // VIEW_H
