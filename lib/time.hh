#pragma once

#include "../config/sdk_config.h"
#include "ratio.hh"

#include <app_timer.h>

#include <stdbool.h>
#include <stdint.h>


namespace lib {

template <typename Period>
class Duration
{
public:
    typedef Period period;

    template <typename OtherPeriod>
    constexpr Duration(Duration<OtherPeriod> other)
    : m_count{other.count() * ((OtherPeriod::num * period::den) / (OtherPeriod::den * period::num))}
    {
    }

    explicit constexpr Duration(uint32_t count)
    : m_count{count}
    {
    }

    constexpr uint32_t count() const
    {
        return m_count;
    }

private:
    const uint32_t m_count;
};

typedef Duration<Micro>    Microseconds;
typedef Duration<Milli>    Milliseconds;
typedef Duration<Ratio<1>> Seconds;

namespace literals {

constexpr Microseconds operator"" _us(unsigned long long count)
{
    return Microseconds(count);
}

constexpr Milliseconds operator"" _ms(unsigned long long count)
{
    return Milliseconds(count);
}

constexpr Seconds operator"" _s(unsigned long long count)
{
    return Seconds(count);
}

} // namespace literals

class Clock final
{
public:
    Clock();
    ~Clock();
};

class Timer final
{
public:
    typedef Duration<Ratio<1, 32768 / (1 + APP_TIMER_CONFIG_RTC_FREQUENCY)>> duration_type;
    typedef void (*callback_fn)(void* context);

    Timer(Clock& clock, bool periodic, callback_fn fn, void* context = nullptr);
    ~Timer();

    void start(duration_type interval);
    void stop();

private:
    app_timer_t    m_timer;
    app_timer_id_t m_id;
    void*          m_context;
};

class FixedIntervalTimer
{
public:
    typedef Timer::duration_type duration_type;
    typedef Timer::callback_fn   callback_fn;

    FixedIntervalTimer(Clock& clock, bool periodic, duration_type interval, callback_fn fn,
        void* context = nullptr);

    void start();
    void stop();

private:
    Timer               m_timer;
    const duration_type m_interval;
};

class DeadlineTimer final : public FixedIntervalTimer
{
public:
    DeadlineTimer(Clock& clock, duration_type interval, callback_fn fn, void* context = nullptr)
    : FixedIntervalTimer(clock, false, interval, fn, context)
    {
    }
};

class Metronome final : public FixedIntervalTimer
{
public:
    Metronome(Clock& clock, duration_type interval, callback_fn fn, void* context = nullptr)
    : FixedIntervalTimer(clock, true, interval, fn, context)
    {
    }
};

} // namespace lib
