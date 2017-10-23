#pragma once

#include "lib/twi.hh"
#include "lib/time.hh"
#include "leds.hh"


class RemoteFuelGauge
{
public:
    RemoteFuelGauge(lib::Twi& twi, LedController& ledController);
    ~RemoteFuelGauge();

    float read_cell_voltage();
    float read_cell_temperature();
    int read_soc();

private:
    lib::Twi&          m_twi;
    RemoteBatteryGauge m_display;
    lib::Metronome     m_metronome;

    static uint8_t calculate_crc(uint8_t crc, uint8_t data);

    void on_tick();
    uint16_t read_register(uint8_t reg);
    void write_register(uint8_t reg, uint16_t val);
};
