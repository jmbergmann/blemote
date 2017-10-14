#include "time.hh"

#include <nrf_log.h>


namespace lib {

Clock::Clock()
{
    ret_code_t err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);
}

Clock::~Clock()
{
}

Timer::Timer(Clock& clock, bool periodic, callback_fn fn, void* context)
: m_timer{{0}}
, m_id{&m_timer}
, m_context(context)
{
    auto mode = periodic
              ? APP_TIMER_MODE_REPEATED
              : APP_TIMER_MODE_SINGLE_SHOT;

    auto res = app_timer_create(&m_id, mode, fn);
    APP_ERROR_CHECK(res);
}

Timer::~Timer()
{
    stop();
}

void Timer::start(duration_type interval)
{
    auto res = app_timer_start(m_id, interval.count(), m_context);
    APP_ERROR_CHECK(res);
}

void Timer::stop()
{
    auto res = app_timer_stop(m_id);
    APP_ERROR_CHECK(res);
}

FixedIntervalTimer::FixedIntervalTimer(Clock& clock, bool periodic, duration_type interval,
    callback_fn fn, void* context)
: m_timer(clock, periodic, fn, context)
, m_interval(interval)
{
}

void FixedIntervalTimer::start()
{
    m_timer.start(m_interval);
}

void FixedIntervalTimer::stop()
{
    m_timer.stop();
}

} // namespace lib
