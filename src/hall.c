#include "hall.h"
#include "../config/pins.h"

#include <nrf_gpio.h>
#include <nrf_drv_saadc.h>
#include <app_error.h>


static void saadc_callback(const nrf_drv_saadc_evt_t* evt)
{
}

void hall_init()
{
    nrf_gpio_cfg_output(PIN_H1_SLP);
    // nrf_gpio_cfg_output(PIN_H2_SLP);
    nrf_gpio_cfg_output(PIN_H3_SLP);

    hall_set_sleep(HALL_1, true);
    // hall_set_sleep(HALL_2, true);
    hall_set_sleep(HALL_3, true);

    ret_code_t err;
    err = nrf_drv_saadc_init(NULL, saadc_callback);

    nrf_saadc_channel_config_t cfg0 = NRF_DRV_SAADC_DEFAULT_CHANNEL_CONFIG_SE(NRF_SAADC_INPUT_AIN0);
    err = nrf_drv_saadc_channel_init(0, &cfg0);
    APP_ERROR_CHECK(err);

    // nrf_saadc_channel_config_t cfg1 = NRF_DRV_SAADC_DEFAULT_CHANNEL_CONFIG_SE(NRF_SAADC_INPUT_AIN1);
    // err = nrf_drv_saadc_channel_init(1, &cfg1);
    // APP_ERROR_CHECK(err);

    nrf_saadc_channel_config_t cfg2 = NRF_DRV_SAADC_DEFAULT_CHANNEL_CONFIG_SE(NRF_SAADC_INPUT_AIN2);
    err = nrf_drv_saadc_channel_init(2, &cfg2);
    APP_ERROR_CHECK(err);
}

void hall_set_sleep(hall_sensor_t sensor, bool sleep)
{
    void (*fn)(uint32_t) = sleep ? nrf_gpio_pin_clear : nrf_gpio_pin_set;
    switch (sensor) {
    case HALL_1: fn(PIN_H1_SLP); break;
    // case HALL_2: fn(PIN_H2_SLP); break;
    case HALL_3: fn(PIN_H3_SLP); break;
    }
}

uint16_t hall_sample(hall_sensor_t sensor)
{
    uint8_t channel = 0;
    switch (sensor) {
    case HALL_1: channel = 2; break;
    // case HALL_2: channel = 1; break;
    case HALL_3: channel = 0; break;
    }

    nrf_saadc_value_t val;
    nrf_drv_saadc_sample_convert(channel, &val);

    return (uint16_t)val;
}
