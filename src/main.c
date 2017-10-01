#include <stdbool.h>
#include <stdint.h>

#include <nrf_delay.h>
#include <nrf_gpio.h>

#include "pins.h"


int main(void)
{
    nrf_gpio_cfg_output(H1_SLP);

    while (true) {
        nrf_gpio_pin_write(H1_SLP, 1);
        nrf_delay_ms(500);
        nrf_gpio_pin_write(H1_SLP, 0);
        nrf_delay_ms(500);
    }
}
