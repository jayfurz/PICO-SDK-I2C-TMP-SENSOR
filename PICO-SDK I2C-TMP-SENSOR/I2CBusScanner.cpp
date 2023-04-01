#include "I2CBusScanner.h"
#include <stdio.h>
#include "hardware/i2c.h"

I2CBusScanner::I2CBusScanner() {
    // No initialization needed in this case.
}

// The 'scan()' function scans the I2C bus for devices by attempting to read
// one byte from each possible address.
void I2CBusScanner::scan() {
    printf("\nI2C Bus Scan\n");
    printf("   0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F\n");
    
    // Iterate through all possible 7-bit I2C addresses (0x00 to 0x7F).
    for (int addr = 0; addr < (1 << 7); ++addr) {
        if (addr % 16 == 0) {
            printf("%02x ", addr);
        }

        int ret;
        uint8_t rxdata;

        // Check if the address is a reserved address; if so, skip it.
        if (reserved_addr(addr))
            ret = PICO_ERROR_GENERIC;
        else
            // Attempt to read one byte from the I2C device at the current address.
            ret = i2c_read_blocking(i2c_default, addr, &rxdata, 1, false);

        // Print a "." if no device was found or "@" if a device was found.
        printf(ret < 0 ? "." : "@");
        printf(addr % 16 == 15 ? "\n" : "  ");
    }
    printf("Done.\n");
}

// The 'reserved_addr()' function checks if an I2C address is reserved.
// Returns 'true' if the address is reserved, 'false' otherwise.
bool I2CBusScanner::reserved_addr(uint8_t addr) {
    // The address is considered reserved if the 4 MSBs are 0x00 or 0x78.
    return (addr & 0x78) == 0 || (addr & 0x78) == 0x78;
}
