#include "leds.h"
#include "twi.h"
#include "../config/pins.h"
#include "../config/app_config.h"

#define U1_ADDRESS TWI_PCA9532_U1_ADDRESS
#define U2_ADDRESS TWI_PCA9532_U2_ADDRESS

#define AUTO_INCREMENT_FLAG (1<<4)

#define REG_INPUT0 0x0
#define REG_INPUT1 0x1
#define REG_PSC0   0x2
#define REG_PWM0   0x3
#define REG_PSC1   0x4
#define REG_PWM1   0x5
#define REG_LS0    0x6
#define REG_LS1    0x7
#define REG_LS2    0x8
#define REG_LS3    0x9

#define SRC_OFF    0b00
#define SRC_ON     0b01
#define SRC_PWM0   0b10
#define SRC_PWM1   0b11

#define CHECKED(x) if ((x) == false) { return false; }


static bool write_lsx_registers(uint8_t address, const uint8_t* ls)
{
    uint8_t data[] = {REG_LS0 | AUTO_INCREMENT_FLAG, ls[0], ls[1], ls[2], ls[3]};
    return twi_write(address, data, sizeof(data));
}

void leds_init()
{
}

bool leds_set_all(uint32_t states)
{
    uint8_t src = SRC_ON;

    uint8_t u1Ls[] = {0, 0, 0, 0};
    uint8_t u2Ls[] = {0, 0, 0, 0};

    if (states & LED_USB_G) {
        u1Ls[3] |= src << 0;
    }

    if (states & LED_USB_Y) {
        u1Ls[3] |= src << 2;
    }

    if (states & LED_BAT_BOTTOM_R) {
        u2Ls[0] |= src << 0;
    }

    if (states & LED_BAT_BOTTOM_Y1) {
        u2Ls[0] |= src << 2;
    }

    if (states & LED_BAT_BOTTOM_Y2) {
        u2Ls[0] |= src << 4;
    }

    if (states & LED_BAT_BOTTOM_G1) {
        u2Ls[0] |= src << 6;
    }

    if (states & LED_BAT_BOTTOM_G2) {
        u2Ls[1] |= src << 0;
    }

    if (states & LED_BAT_TOP_R) {
        u2Ls[3] |= src << 0;
    }

    if (states & LED_BAT_TOP_Y1) {
        u2Ls[2] |= src << 6;
    }

    if (states & LED_BAT_TOP_Y2) {
        u2Ls[2] |= src << 4;
    }

    if (states & LED_BAT_TOP_G1) {
        u2Ls[2] |= src << 2;
    }

    if (states & LED_BAT_TOP_G2) {
        u2Ls[2] |= src << 0;
    }

    if (states & LED_SEG_DOT_TOP) {
        u1Ls[0] |= src << 6;
    }

    if (states & LED_SEG_DOT_BOTTOM) {
        u1Ls[2] |= src << 6;
    }

    if (states & LED_SEG_TOP) {
        u1Ls[0] |= src << 2;
    }

    if (states & LED_SEG_TOP_LEFT) {
        u1Ls[0] |= src << 4;
    }

    if (states & LED_SEG_TOP_RIGHT) {
        u1Ls[0] |= src << 0;
    }

    if (states & LED_SEG_MIDDLE) {
        u1Ls[1] |= src << 0;
    }

    if (states & LED_SEG_BOTTOM) {
        u1Ls[2] |= src << 2;
    }
    if (states & LED_SEG_BOTTOM_LEFT) {
        u1Ls[2] |= src << 0;
    }

    if (states & LED_SEG_BOTTOM_RIGHT) {
        u1Ls[2] |= src << 4;
    }

    CHECKED(write_lsx_registers(U1_ADDRESS, u1Ls));
    CHECKED(write_lsx_registers(U2_ADDRESS, u2Ls));

    return true;
}
