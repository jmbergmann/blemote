#pragma once

#include <stdint.h>


class Twi final
{
public:
    Twi();
    ~Twi();

    bool read(uint8_t address, uint8_t* buffer, uint8_t size);
    bool write(uint8_t address, const uint8_t* buffer, uint8_t size);
    bool write_no_stop(uint8_t address, const uint8_t* buffer, uint8_t size);
};
