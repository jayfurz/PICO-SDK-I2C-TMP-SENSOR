#ifndef I2C_BUS_SCANNER_H
#define I2C_BUS_SCANNER_H
#include "pico/stdlib.h"

class I2CBusScanner {
public:
    I2CBusScanner();
    void scan();

private:
    bool reserved_addr(uint8_t addr);
};

#endif // I2C_BUS_SCANNER_H
