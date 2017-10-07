#include "buttons.h"
#include "../config/pins.h"

#include <nrf_gpio.h>


void buttons_init()
{
    nrf_gpio_cfg_sense_input(PIN_BTN_TOP, NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_SENSE_LOW);
    nrf_gpio_cfg_sense_input(PIN_BTN_BOTTOM, NRF_GPIO_PIN_PULLUP, NRF_GPIO_PIN_SENSE_LOW);
}

bool buttons_is_top_pressed()
{
    return nrf_gpio_pin_read(PIN_BTN_TOP) ? false : true;
}

bool buttons_is_bottom_pressed()
{
    return nrf_gpio_pin_read(PIN_BTN_BOTTOM) ? false : true;
}
