#pragma once

#define UART_FLOW_CONTROL_ENABLED 1
#define UART_BAUDRATE UART_BAUDRATE_BAUDRATE_Baud115200
#define UART_RX_BUFFER_SIZE 256
#define UART_TX_BUFFER_SIZE 256

#define TWI_FREQUENCY NRF_TWI_FREQ_100K
#define TWI_LC709203_ADDRESS 0x16
#define TWI_PCA9532_U1_ADDRESS 0xC2
#define TWI_PCA9532_U2_ADDRESS 0xC0