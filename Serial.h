#ifndef Serial_h
#define Serial_h
#include <inttypes.h>
#include "pico/stdlib.h"
#include "FirmataDefines.h"
#include <string.h>


class Serial
{
public:
    Serial();
    void write(uint8_t);
    int available();
    int read();
    unsigned char readPort(uint8_t port, uint8_t bitmask);
    unsigned char writePort(uint8_t port, uint8_t value, uint8_t bitmask);
    
protected:
    void begin(long);
};

#endif // Serial_h
