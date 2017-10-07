#include "gauge.h"
#include "twi.h"
#include "../config/pins.h"
#include "../config/app_config.h"

#define ADDRESS TWI_LC709203_ADDRESS
#define REG_THERMISTOR_B 0x06
#define REG_CELL_VOLTAGE 0x09
#define CHECKED(x) if ((x) == false) { return false; }


static uint8_t calculate_crc(uint8_t crc, uint8_t data)
{
    uint16_t tmp = (uint16_t)(crc ^ data);
    tmp <<= 8;

    for (int i = 0; i < 8; ++i) {
        if (tmp & 0x8000) {
            tmp ^= 0x8380;
        }

        tmp <<= 1;
    }

    uint8_t newCrc = (uint8_t)(tmp >> 8);
    return newCrc;
}

static bool read_register(uint8_t reg, uint16_t* val)
{
    CHECKED(twi_write_no_stop(ADDRESS, &reg, 1));

    uint8_t bytes[3];
    CHECKED(twi_read(ADDRESS, bytes, sizeof(bytes)));

    *val = bytes[1];
    *val <<= 8;
    *val |= bytes[0];

    uint8_t crc = 0;
    crc = calculate_crc(crc, ADDRESS);
    crc = calculate_crc(crc, reg);
    crc = calculate_crc(crc, ADDRESS | 1);
    crc = calculate_crc(crc, bytes[0]);
    crc = calculate_crc(crc, bytes[1]);

    return crc == bytes[2];
}

static bool write_register(uint8_t reg, uint16_t val)
{
    uint8_t bytes[] = {reg, (uint8_t)val, (uint8_t)(val >> 8), 0};

    uint8_t crc = 0;
    crc = calculate_crc(crc, ADDRESS);
    crc = calculate_crc(crc, reg);
    crc = calculate_crc(crc, bytes[1]);
    crc = calculate_crc(crc, bytes[2]);
    bytes[3] = crc;

    return twi_write(ADDRESS, bytes, sizeof(bytes));
}

void gauge_init()
{
}

uint16_t gauge_read_thermistor_b()
{
    uint16_t b = 0;
    if (!read_register(REG_THERMISTOR_B, &b)) {
        return 0;
    }

    return b;
}

bool gauge_write_thermistor_b(uint16_t b)
{
    return write_register(REG_THERMISTOR_B, b);
}

float gauge_read_voltage()
{
    uint16_t voltageInMs = 0;
    if (!read_register(REG_CELL_VOLTAGE, &voltageInMs)) {
        return -1;
    }

    float voltage = (float)voltageInMs / 1000;

    return voltage;
}
