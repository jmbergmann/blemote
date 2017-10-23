#include "config/app_config.h"
#include "config/pins.h"
#include "remote_fuel_gauge.hh"
using namespace lib::literals;

#include <nrf_gpio.h>
#include <nrf_log.h>

#define ADDRESS TWI_LC709203_ADDRESS

#define REG_BEFORE_RSOC 0x04
#define REG_THERMISTOR_B 0x06
#define REG_INITIAL_RSOC 0x07
#define REG_CELL_TEMPERATURE 0x08
#define REG_CELL_VOLTAGE 0x09
#define REG_CURRENT_DIRECTION 0x0A
#define REG_APA 0x0B
#define REG_APT 0x0C
#define REG_RSOC 0x0D
#define REG_ITE 0x0F
#define REG_IC_VERSION 0x11
#define REG_CHANGE_OF_THE_PARAMETER 0x12
#define REG_ALARM_LOW_RSOC 0x13
#define REG_ALARM_LOW_CELL_VOLTAGE 0x14
#define REG_IC_POWER_MODE 0x15
#define REG_STATUS_BIT 0x16
#define REG_NUMBER_OF_THE_PARAMETER 0x1A


RemoteFuelGauge::RemoteFuelGauge(lib::Twi& twi, LedController& ledController)
: m_twi(twi)
, m_display(ledController)
, m_metronome(UPDATE_INTERVAL_REMOTE_FUEL_GAUGE, [](void* self) { static_cast<decltype(this)>(self)->on_tick(); }, this)
{
    nrf_gpio_cfg_sense_input(PIN_STAT1, NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_SENSE_LOW);
    nrf_gpio_cfg_sense_input(PIN_STAT2, NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_SENSE_LOW);
    nrf_gpio_cfg_sense_input(PIN_PG, NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_SENSE_LOW);

    write_register(REG_THERMISTOR_B, 4000);

    m_metronome.start();
}

RemoteFuelGauge::~RemoteFuelGauge()
{
    m_metronome.stop();
}

float RemoteFuelGauge::read_cell_voltage()
{
    return read_register(REG_CELL_VOLTAGE) / 1000.0f;
}

float RemoteFuelGauge::read_cell_temperature()
{
    return (read_register(REG_CELL_TEMPERATURE) - 0x09E4) / 10.0f - 20;
}

int RemoteFuelGauge::read_soc()
{
    return read_register(REG_RSOC);
}

uint8_t RemoteFuelGauge::calculate_crc(uint8_t crc, uint8_t data)
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

void RemoteFuelGauge::on_tick()
{
    float v = read_cell_voltage();
    float t = read_cell_temperature();
    int soc = read_soc();

    NRF_LOG_INFO("Voltage: " NRF_LOG_FLOAT_MARKER, NRF_LOG_FLOAT(v));
    NRF_LOG_INFO("Temp: " NRF_LOG_FLOAT_MARKER, NRF_LOG_FLOAT(t));
    NRF_LOG_INFO("SOC: %i", soc);

    bool stat1 = !!nrf_gpio_pin_read(PIN_STAT1);
    bool stat2 = !!nrf_gpio_pin_read(PIN_STAT2);
    bool pg = !!nrf_gpio_pin_read(PIN_PG);
    NRF_LOG_INFO("STAT1: %d STAT2: %d PG: %d", (int)stat1, (int)stat2, (int)pg);

    RemoteBatteryGauge::pattern_t pattern;
    if (soc <= 10) {
        pattern = RemoteBatteryGauge::CRITICALLY_LOW;
    }
    else if (soc <= 20) {
        pattern = RemoteBatteryGauge::SOC_20;
    }
    else if (soc <= 40) {
        pattern = RemoteBatteryGauge::SOC_40;
    }
    else if (soc <= 60) {
        pattern = RemoteBatteryGauge::SOC_60;
    }
    else if (soc <= 80) {
        pattern = RemoteBatteryGauge::SOC_80;
    }
    else {
        pattern = RemoteBatteryGauge::SOC_100;
    }

    m_display.set_pattern(pattern);
}

uint16_t RemoteFuelGauge::read_register(uint8_t reg)
{
    m_twi.write_no_stop(ADDRESS, &reg, 1);

    uint8_t bytes[3];
    m_twi.read(ADDRESS, bytes, sizeof(bytes));

    uint16_t val = bytes[1];
    val <<= 8;
    val |= bytes[0];

    uint8_t crc = 0;
    crc = calculate_crc(crc, ADDRESS);
    crc = calculate_crc(crc, reg);
    crc = calculate_crc(crc, ADDRESS | 1);
    crc = calculate_crc(crc, bytes[0]);
    crc = calculate_crc(crc, bytes[1]);
    bool crcMatches = crc == bytes[2];

    APP_ERROR_CHECK_BOOL(crcMatches);

    return val;
}

void RemoteFuelGauge::write_register(uint8_t reg, uint16_t val)
{
    uint8_t bytes[] = {reg, (uint8_t)val, (uint8_t)(val >> 8), 0};

    uint8_t crc = 0;
    crc = calculate_crc(crc, ADDRESS);
    crc = calculate_crc(crc, reg);
    crc = calculate_crc(crc, bytes[1]);
    crc = calculate_crc(crc, bytes[2]);
    bytes[3] = crc;

    m_twi.write(ADDRESS, bytes, sizeof(bytes));
}
