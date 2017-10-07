#include "config/pins.h"
#include "config/app_config.h"

#include "uart.h"
#include "buttons.h"
#include "twi.h"
#include "hall.h"
#include "gauge.h"
#include "leds.h"

#include <nrf_delay.h>
#include <nrf.h>
#include <nrf_gpio.h>

#include <stdio.h>


int main(void)
{
    uart_init();
    printf("\n\rInitialising...\n\r");

    buttons_init();
    twi_init();
    gauge_init();
    hall_init();
    leds_init();

    printf("App up and running.\n\r");

    while (true) {
        // if (buttons_is_top_pressed()) {
            // for (int i = 0; i < 1000 && !buttons_is_bottom_pressed(); ++i) {
                hall_set_sleep(HALL_1, false);
                hall_set_sleep(HALL_3, false);

                uint16_t hall1 = hall_sample(HALL_1);
                uint16_t hall3 = hall_sample(HALL_3);
                float voltage = gauge_read_voltage();
                printf("H1: %d H3: %d BAT: %.3fV\n\r", (int)hall1, (int)hall3, voltage);

                nrf_delay_ms(100);
            // }
        // }

        // if (buttons_is_bottom_pressed()) {
            // for (int i = 0; i < 5 && !buttons_is_top_pressed(); ++i) {
                int delay = 50;
                leds_set_all(LED_BAT_BOTTOM_R); nrf_delay_ms(delay);
                leds_set_all(LED_BAT_BOTTOM_Y1); nrf_delay_ms(delay);
                leds_set_all(LED_BAT_BOTTOM_Y2); nrf_delay_ms(delay);
                leds_set_all(LED_BAT_BOTTOM_G1); nrf_delay_ms(delay);
                leds_set_all(LED_BAT_BOTTOM_G2); nrf_delay_ms(delay);

                leds_set_all(LED_BAT_TOP_R); nrf_delay_ms(delay);
                leds_set_all(LED_BAT_TOP_Y1); nrf_delay_ms(delay);
                leds_set_all(LED_BAT_TOP_Y2); nrf_delay_ms(delay);
                leds_set_all(LED_BAT_TOP_G1); nrf_delay_ms(delay);
                leds_set_all(LED_BAT_TOP_G2); nrf_delay_ms(delay);

                leds_set_all(LED_USB_Y); nrf_delay_ms(delay);
                leds_set_all(LED_USB_G); nrf_delay_ms(delay);

                leds_set_all(LED_SEG_DOT_BOTTOM); nrf_delay_ms(delay);
                leds_set_all(LED_SEG_0); nrf_delay_ms(delay * 3);
                leds_set_all(LED_SEG_1); nrf_delay_ms(delay * 3);
                leds_set_all(LED_SEG_2); nrf_delay_ms(delay * 3);
                leds_set_all(LED_SEG_3); nrf_delay_ms(delay * 3);
                leds_set_all(LED_SEG_4); nrf_delay_ms(delay * 3);
                leds_set_all(LED_SEG_5); nrf_delay_ms(delay * 3);
                leds_set_all(LED_SEG_6); nrf_delay_ms(delay * 3);
                leds_set_all(LED_SEG_7); nrf_delay_ms(delay * 3);
                leds_set_all(LED_SEG_8); nrf_delay_ms(delay * 3);
                leds_set_all(LED_SEG_9); nrf_delay_ms(delay * 3);
                leds_set_all(LED_SEG_DOT_TOP); nrf_delay_ms(delay);
                leds_set_all(LED_NONE); nrf_delay_ms(delay);
            // }
        // }
    }
}
