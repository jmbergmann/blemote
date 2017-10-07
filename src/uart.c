#include "uart.h"
#include "../config/pins.h"
#include "../config/app_config.h"

#include <app_uart.h>
#include <app_error.h>


static void uart_error_handler(app_uart_evt_t* event)
{
    if (event->evt_type == APP_UART_COMMUNICATION_ERROR)
    {
        APP_ERROR_HANDLER(event->data.error_communication);
    }
    else if (event->evt_type == APP_UART_FIFO_ERROR)
    {
        APP_ERROR_HANDLER(event->data.error_code);
    }
}

void uart_init()
{
    const app_uart_comm_params_t cfg = {
        PIN_FTDI_TXD,
        PIN_FTDI_RXD,
        PIN_FTDI_CTS,
        PIN_FTDI_RTS,
        UART_FLOW_CONTROL_ENABLED ? APP_UART_FLOW_CONTROL_ENABLED : APP_UART_FLOW_CONTROL_DISABLED,
        false,
        UART_BAUDRATE
    };

    uint32_t err;
    APP_UART_FIFO_INIT(&cfg, UART_RX_BUFFER_SIZE, UART_TX_BUFFER_SIZE, uart_error_handler,
        APP_IRQ_PRIORITY_LOWEST, err);
    APP_ERROR_CHECK(err);
}
