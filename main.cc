#include <nrf52832_peripherals.h>
#include <nrf_log.h>
#include <nrf_log_ctrl.h>
#include <nrf_log_default_backends.h>
#include <nrf_sdh.h>
#include <nrf_gpio.h>
#include <nrf_drv_gpiote.h>
#include <app_scheduler.h>

#include "lib/time.hh"

#include "types.hh"
#include "leds.hh"
#include "remote_fuel_gauge.hh"
#include "throttle.hh"
#include "config/pins.h"


namespace {

void init_log()
{
    auto err = NRF_LOG_INIT(nullptr);
    APP_ERROR_CHECK(err);

    NRF_LOG_DEFAULT_BACKENDS_INIT();
}

void init_scheduler()
{
    APP_SCHED_INIT(APP_TIMER_SCHED_EVENT_DATA_SIZE, 50);

}

void init_softdevice()
{
    auto err = nrf_sdh_enable_request();
    APP_ERROR_CHECK(err);
}

void init_timers()
{
    auto err = app_timer_init();
    APP_ERROR_CHECK(err);
}

void init_gpiote()
{
    auto err = nrf_drv_gpiote_init();
    APP_ERROR_CHECK(err);
}

void on_button_state_changed(nrf_drv_gpiote_pin_t pin)
{
    if (pin == PIN_BTN_TOP) {
        NRF_LOG_INFO("Top button pressed");
    }
    else if (pin == PIN_BTN_BOTTOM) {
        NRF_LOG_INFO("Bottom button pressed");
    }
    else {
        NRF_LOG_INFO("Something else happened");
    }
}

void init_buttons()
{
    nrf_drv_gpiote_in_config_t cfg = {
        .sense = NRF_GPIOTE_POLARITY_HITOLO,
        .pull = NRF_GPIO_PIN_PULLUP,
        .is_watcher = false,
        .hi_accuracy = true,
    };

    auto fn = [](nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action) {
        auto err = app_sched_event_put(&pin, sizeof(pin), [](void* data, uint16_t size) {
            auto pin = *reinterpret_cast<nrf_drv_gpiote_pin_t*>(data);
            on_button_state_changed(pin);
        });
        APP_ERROR_CHECK(err);
    };

    auto err = nrf_drv_gpiote_in_init(PIN_BTN_TOP, &cfg, fn);
    APP_ERROR_CHECK(err);
    nrf_drv_gpiote_in_event_enable(PIN_BTN_TOP, true);

    err = nrf_drv_gpiote_in_init(PIN_BTN_BOTTOM, &cfg, fn);
    APP_ERROR_CHECK(err);
    nrf_drv_gpiote_in_event_enable(PIN_BTN_BOTTOM, true);
}

void power_manage(void)
{
    ret_code_t err_code = sd_app_evt_wait();
    APP_ERROR_CHECK(err_code);
}

} // anonymous namespace


int main(void)
{
    auto state = SYS_INITIALISING;

    init_log();
    init_scheduler();
    init_gpiote();
    init_buttons();
    init_softdevice();
    init_timers();

    state = SYS_SLEEPING;

    using namespace lib;
    using namespace lib::literals;


    Twi twi;
    LedController leds(twi);

    RemoteFuelGauge remoteFuelGauge(twi, leds);
    Throttle throttle();

    NRF_LOG_INFO("Application started.");
    while (true) {
        app_sched_execute();
        if (NRF_LOG_PROCESS() == false) {
            power_manage();
        }
    }
}
