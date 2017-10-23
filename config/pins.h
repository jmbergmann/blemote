#pragma once

// Buttons
#define PIN_BTN_TOP 19
#define PIN_BTN_BOTTOM 20

// TWI
#define PIN_SCL 13
#define PIN_SDA 14

// Hall Sensors
#define H1_FITTED true
#define PIN_H1_AIN NRF_SAADC_INPUT_AIN0
#define PIN_H1_SLP 31

#define H2_FITTED false
#define PIN_H2_AIN NRF_SAADC_INPUT_AIN1
#define PIN_H2_SLP 00

#define H3_FITTED true
#define PIN_H3_AIN NRF_SAADC_INPUT_AIN2
#define PIN_H3_SLP 30

// LC709203 (Gas Gauge)
#define PIN_ALARMB 15

// MCP73837 (Charger)
#define PIN_STAT1 12
#define PIN_STAT2 11
#define PIN_PG 17
#define PIN_THERM 18

// FT231 (USB to UART)
#define PIN_FTDI_RTS 25
#define PIN_FTDI_RXD 26
#define PIN_FTDI_CTS 27
#define PIN_FTDI_TXD 29

#define PIN_PWREN 28
#define PIN_SLEEP 21
