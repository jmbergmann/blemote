#include "twi.h"
#include "../config/pins.h"
#include "../config/app_config.h"

#include <app_twi.h>
#include <app_error.h>


static const nrf_drv_twi_t s_twi = NRF_DRV_TWI_INSTANCE(0);

void twi_init()
{
    uint32_t err;
    const nrf_drv_twi_config_t cfg = {
        .scl                = PIN_SCL,
        .sda                = PIN_SDA,
        .frequency          = TWI_FREQUENCY,
        .interrupt_priority = APP_IRQ_PRIORITY_HIGH,
        .clear_bus_init     = false
     };

     err = nrf_drv_twi_init(&s_twi, &cfg, NULL, NULL);
     APP_ERROR_CHECK(err);
     nrf_drv_twi_enable(&s_twi);
}

bool twi_read(uint8_t address, uint8_t* buffer, uint8_t size)
{
    ret_code_t err = nrf_drv_twi_rx(&s_twi, address >> 1, buffer, size);
    return err == NRF_SUCCESS;
}

bool twi_write(uint8_t address, const uint8_t* buffer, uint8_t size)
{
    ret_code_t err = nrf_drv_twi_tx(&s_twi, address >> 1, buffer, size, false);
    return err == NRF_SUCCESS;
}

bool twi_write_no_stop(uint8_t address, const uint8_t* buffer, uint8_t size)
{
    ret_code_t err = nrf_drv_twi_tx(&s_twi, address >> 1, buffer, size, true);
    return err == NRF_SUCCESS;
}
