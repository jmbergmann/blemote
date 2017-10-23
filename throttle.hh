#pragma once

#include "lib/time.hh"

#include <stdbool.h>


class Throttle final
{
public:
    Throttle();
    ~Throttle();

private:
    lib::Metronome     m_metronome;
    lib::DeadlineTimer m_pwrOnDelayTimer;

    void set_sleep(bool sleep);
    float read_position();
    void on_tick();
    void on_pwr_on_delay_expired();
};
