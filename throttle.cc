#include "throttle.hh"
#include "config/pins.h"
#include "config/app_config.h"
using namespace lib::literals;

#include <nrf_gpio.h>
#include <nrf_drv_saadc.h>
#include <app_error.h>
#include <nrf_log.h>


Throttle::Throttle()
: m_metronome(UPDATE_INTERVAL_THROTTLE, [](void* self) { static_cast<decltype(this)>(self)->on_tick(); }, this)
, m_pwrOnDelayTimer(HALL_SENSOR_POWER_UP_DELAY, [](void* self) { static_cast<decltype(this)>(self)->on_pwr_on_delay_expired(); }, this)
{
    nrf_gpio_cfg_output(PIN_H1_SLP);
    nrf_gpio_cfg_output(PIN_H2_SLP);
    nrf_gpio_cfg_output(PIN_H3_SLP);

    set_sleep(true);

    auto err = nrf_drv_saadc_init(NULL, [](auto*) {});
    APP_ERROR_CHECK(err);

    if (H1_FITTED) {
        nrf_saadc_channel_config_t cfg0 = NRF_DRV_SAADC_DEFAULT_CHANNEL_CONFIG_SE(PIN_H1_AIN);
        err = nrf_drv_saadc_channel_init(0, &cfg0);
        APP_ERROR_CHECK(err);
    }

    if (H2_FITTED) {
        nrf_saadc_channel_config_t cfg1 = NRF_DRV_SAADC_DEFAULT_CHANNEL_CONFIG_SE(PIN_H2_AIN);
        err = nrf_drv_saadc_channel_init(1, &cfg1);
        APP_ERROR_CHECK(err);
    }

    if (H3_FITTED) {
        nrf_saadc_channel_config_t cfg2 = NRF_DRV_SAADC_DEFAULT_CHANNEL_CONFIG_SE(PIN_H3_AIN);
        err = nrf_drv_saadc_channel_init(2, &cfg2);
        APP_ERROR_CHECK(err);
    }

    m_metronome.start();
}

Throttle::~Throttle()
{
    m_pwrOnDelayTimer.stop();
    m_metronome.stop();
    set_sleep(true);
}

void Throttle::set_sleep(bool sleep)
{
    if (sleep) {
        nrf_gpio_pin_clear(PIN_H1_SLP);
        nrf_gpio_pin_clear(PIN_H2_SLP);
        nrf_gpio_pin_clear(PIN_H3_SLP);
    }
    else {
        nrf_gpio_pin_set(PIN_H1_SLP);
        nrf_gpio_pin_set(PIN_H2_SLP);
        nrf_gpio_pin_set(PIN_H3_SLP);
    }
}

float Throttle::read_position()
{
    nrf_saadc_value_t raw1;
    auto err = nrf_drv_saadc_sample_convert(0, &raw1);
    APP_ERROR_CHECK(err);

    nrf_saadc_value_t raw3;
    err = nrf_drv_saadc_sample_convert(2, &raw3);
    APP_ERROR_CHECK(err);

    NRF_LOG_INFO("H1: %d H3: %d", (int)raw1, (int)raw3);

    return 0.0f;
}

void Throttle::on_tick()
{
    set_sleep(false);
    m_pwrOnDelayTimer.start();
}

void Throttle::on_pwr_on_delay_expired()
{
    auto pos = read_position();
    set_sleep(true);
}
