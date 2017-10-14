#include "Twi.hh"
#include "../config/pins.h"
#include "../config/app_config.h"

#include <app_error.h>
#include <app_twi.h>
#include <nrf_log.h>


namespace {

const nrf_drv_twi_t g_twiInst = {
    .reg          = {(NRF_TWI_Type*)NRF_TWI0},
    .drv_inst_idx = TWI0_INSTANCE_INDEX,
    .use_easy_dma = TWI_USE_EASY_DMA(0)
};

} // anonymous namespace


Twi::Twi()
{
    const nrf_drv_twi_config_t cfg = {
        .scl                = PIN_SCL,
        .sda                = PIN_SDA,
        .frequency          = TWI_FREQUENCY,
        .interrupt_priority = APP_IRQ_PRIORITY_HIGH,
        .clear_bus_init     = false
     };

     auto err = nrf_drv_twi_init(&g_twiInst, &cfg, NULL, NULL);
     APP_ERROR_CHECK(err);
     nrf_drv_twi_enable(&g_twiInst);
}

Twi::~Twi()
{
    nrf_drv_twi_disable(&g_twiInst);
    nrf_drv_twi_uninit(&g_twiInst);
}

bool Twi::read(uint8_t address, uint8_t* buffer, uint8_t size)
{
    auto err = nrf_drv_twi_rx(&g_twiInst, address >> 1, buffer, size);
    if (err != NRF_SUCCESS) {
        NRF_LOG_ERROR("TWI: Failed to read data from device %X", (int)address);
    }

    return err == NRF_SUCCESS;
}

bool Twi::write(uint8_t address, const uint8_t* buffer, uint8_t size)
{
    auto err = nrf_drv_twi_tx(&g_twiInst, address >> 1, buffer, size, false);
    if (err != NRF_SUCCESS) {
        NRF_LOG_ERROR("TWI: Failed to write data to device %X", (int)address);
    }

    return err == NRF_SUCCESS;
}

bool Twi::write_no_stop(uint8_t address, const uint8_t* buffer, uint8_t size)
{
    auto err = nrf_drv_twi_tx(&g_twiInst, address >> 1, buffer, size, true);
    if (err != NRF_SUCCESS) {
        NRF_LOG_ERROR("TWI: Failed to write data to device %X", (int)address);
    }

    return err == NRF_SUCCESS;
}
