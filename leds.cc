#include "leds.hh"
#include "../config/pins.h"
#include "../config/app_config.h"
using namespace lib::literals;

#include <nrf_log.h>

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


LedController::LedController(lib::Twi& twi)
: m_twi(twi)
{
    set_all(LED_NONE);
}

LedController::~LedController()
{
    set_all(LED_NONE);
}

void LedController::set(led_states_t mask, led_states_t states)
{
    m_states &= ~mask;
    switch_on(states);
}

void LedController::set_all(led_states_t states)
{
    m_states = states;
    update();
}

void LedController::switch_on(led_states_t leds)
{
    m_states |= leds;
    update();
}

void LedController::switch_off(led_states_t leds)
{
    m_states &= ~leds;
    update();
}

void LedController::toggle(led_states_t leds)
{
    m_states ^= leds;
    update();
}

void LedController::write_lsx_registers(uint8_t address, const uint8_t* ls)
{
    uint8_t data[] = {REG_LS0 | AUTO_INCREMENT_FLAG, ls[0], ls[1], ls[2], ls[3]};
    bool ok = m_twi.write(address, data, sizeof(data));
    if (!ok) {
        NRF_LOG_ERROR("Could not write register in LED driver chip");
    }
}

void LedController::update()
{
    uint8_t src = SRC_ON;

    uint8_t u1Ls[] = {0, 0, 0, 0};
    uint8_t u2Ls[] = {0, 0, 0, 0};

    if (m_states & LED_USB_G)            { u1Ls[3] |= src << 0; }
    if (m_states & LED_USB_Y)            { u1Ls[3] |= src << 2; }
    if (m_states & LED_BAT_BOTTOM_R)     { u2Ls[0] |= src << 0; }
    if (m_states & LED_BAT_BOTTOM_Y1)    { u2Ls[0] |= src << 2; }
    if (m_states & LED_BAT_BOTTOM_Y2)    { u2Ls[0] |= src << 4; }
    if (m_states & LED_BAT_BOTTOM_G1)    { u2Ls[0] |= src << 6; }
    if (m_states & LED_BAT_BOTTOM_G2)    { u2Ls[1] |= src << 0; }
    if (m_states & LED_BAT_TOP_R)        { u2Ls[3] |= src << 0; }
    if (m_states & LED_BAT_TOP_Y1)       { u2Ls[2] |= src << 6; }
    if (m_states & LED_BAT_TOP_Y2)       { u2Ls[2] |= src << 4; }
    if (m_states & LED_BAT_TOP_G1)       { u2Ls[2] |= src << 2; }
    if (m_states & LED_BAT_TOP_G2)       { u2Ls[2] |= src << 0; }
    if (m_states & LED_SEG_DOT_TOP)      { u1Ls[0] |= src << 6; }
    if (m_states & LED_SEG_DOT_BOTTOM)   { u1Ls[2] |= src << 6; }
    if (m_states & LED_SEG_TOP)          { u1Ls[0] |= src << 2; }
    if (m_states & LED_SEG_TOP_LEFT)     { u1Ls[0] |= src << 4; }
    if (m_states & LED_SEG_TOP_RIGHT)    { u1Ls[0] |= src << 0; }
    if (m_states & LED_SEG_MIDDLE)       { u1Ls[1] |= src << 0; }
    if (m_states & LED_SEG_BOTTOM)       { u1Ls[2] |= src << 2; }
    if (m_states & LED_SEG_BOTTOM_LEFT)  { u1Ls[2] |= src << 0; }
    if (m_states & LED_SEG_BOTTOM_RIGHT) { u1Ls[2] |= src << 4; }

    write_lsx_registers(U1_ADDRESS, u1Ls);
    write_lsx_registers(U2_ADDRESS, u2Ls);
}

Led::Led(LedController& controller, LedController::led_t led)
: m_controller(controller)
, m_metronome(100_ms, [](void* self) { static_cast<decltype(this)>(self)->on_tick(); }, this)
, m_led(led)
{
    set_pattern(OFF);
}

void Led::set_pattern(pattern_t pattern)
{
    if (m_pattern == pattern) {
        return;
    }

    m_pattern = pattern;
    m_step = 0;

    switch (pattern) {
    case OFF:
    case SOLID:
        m_metronome.stop();
        break;

    default:
        m_metronome.start();
        break;
    }

    update_led();
}

void Led::update_led()
{
    switch (m_pattern) {
    case OFF:
        m_controller.switch_off(m_led);
        break;

    case SOLID:
        m_controller.switch_on(m_led);
        break;
        
    case FLASH_FAST:
        if (m_step % 2) {
            m_controller.switch_on(m_led);
        }
        else {
            m_controller.switch_off(m_led);
        }
        break;

    case FLASH_SLOW:
        if ((m_step % 8) < 4) {
            m_controller.switch_on(m_led);
        }
        else {
            m_controller.switch_off(m_led);
        }
        break;
    }

    ++m_step;
}

void Led::on_tick()
{
    update_led();
}

TopDotLed::TopDotLed(LedController& controller)
: Led(controller, LedController::LED_SEG_DOT_TOP)
{
}

BottomDotLed::BottomDotLed(LedController& controller)
: Led(controller, LedController::LED_SEG_DOT_BOTTOM)
{
}

UsbGreenLed::UsbGreenLed(LedController& controller)
: Led(controller, LedController::LED_USB_G)
{
}

UsbYellowLed::UsbYellowLed(LedController& controller)
: Led(controller, LedController::LED_USB_Y)
{
}

BatteryGauge::BatteryGauge(LedController& controller, int shiftVal)
: m_controller(controller)
, m_metronome(100_ms, [](void* self) { static_cast<decltype(this)>(self)->on_tick(); }, this)
, m_shiftVal(shiftVal)
{
    set_pattern(OFF);
}

void BatteryGauge::set_pattern(pattern_t pattern)
{
    if (m_pattern == pattern) {
        return;
    }

    m_pattern = pattern;
    m_step = 0;

    switch (pattern) {
    case OFF:
    case SOC_100:
    case SOC_80:
    case SOC_60:
    case SOC_40:
    case SOC_20:
        m_metronome.stop();
        break;

    default:
        m_metronome.start();
        break;
    }

    update_leds();
}

void BatteryGauge::update_leds()
{
    switch (m_pattern) {
    case OFF:
        m_controller.switch_off(LED_ALL << m_shiftVal);
        break;

    case SOC_100:
        m_controller.switch_on(LED_ALL << m_shiftVal);
        break;

    case SOC_80:
        m_controller.set(LED_ALL << m_shiftVal, (LED_R | LED_Y1 | LED_Y2 | LED_G1) << m_shiftVal);
        break;

    case SOC_60:
        m_controller.set(LED_ALL << m_shiftVal, (LED_R | LED_Y1 | LED_Y2) << m_shiftVal);
        break;

    case SOC_40:
        m_controller.set(LED_ALL << m_shiftVal, (LED_R | LED_Y1) << m_shiftVal);
        break;

    case SOC_20:
        m_controller.set(LED_ALL << m_shiftVal, LED_R << m_shiftVal);
        break;

    case CRITICALLY_LOW:
        if (m_step % 4 < 2) {
            m_controller.set(LED_ALL << m_shiftVal, LED_R << m_shiftVal);
        }
        else {
            m_controller.switch_off(LED_ALL << m_shiftVal);
        }
        break;

    case CHARGING_COMPLETE:
        m_controller.switch_on(LED_ALL << m_shiftVal);
        break;

    case CHARGING_100:
        switch ((m_step % 22) / 2) {
        case 0: m_controller.switch_off(LED_ALL << m_shiftVal); break;
        case 1: m_controller.set(LED_ALL << m_shiftVal, LED_R  << m_shiftVal); break;
        case 2: m_controller.set(LED_ALL << m_shiftVal, LED_Y1 << m_shiftVal); break;
        case 3: m_controller.set(LED_ALL << m_shiftVal, LED_Y2 << m_shiftVal); break;
        case 4: m_controller.set(LED_ALL << m_shiftVal, LED_G1 << m_shiftVal); break;
        case 5: m_controller.set(LED_ALL << m_shiftVal, LED_G2 << m_shiftVal); break;
        case 6: m_controller.switch_on(LED_ALL << m_shiftVal); break;
        }
        break;

    case CHARGING_80:
        switch ((m_step % 22) / 2) {
        case 0: m_controller.switch_off(LED_ALL << m_shiftVal); break;
        case 1: m_controller.set(LED_ALL << m_shiftVal, LED_R  << m_shiftVal); break;
        case 2: m_controller.set(LED_ALL << m_shiftVal, LED_Y1 << m_shiftVal); break;
        case 3: m_controller.set(LED_ALL << m_shiftVal, LED_Y2 << m_shiftVal); break;
        case 4: m_controller.set(LED_ALL << m_shiftVal, LED_G1 << m_shiftVal); break;
        case 5: m_controller.set(LED_ALL << m_shiftVal, LED_G2 << m_shiftVal); break;
        case 6: m_controller.set(LED_ALL << m_shiftVal, (LED_R | LED_Y1 | LED_Y2 | LED_G1) << m_shiftVal);
        }
        break;

    case CHARGING_60:
        switch ((m_step % 22) / 2) {
        case 0: m_controller.switch_off(LED_ALL << m_shiftVal); break;
        case 1: m_controller.set(LED_ALL << m_shiftVal, LED_R  << m_shiftVal); break;
        case 2: m_controller.set(LED_ALL << m_shiftVal, LED_Y1 << m_shiftVal); break;
        case 3: m_controller.set(LED_ALL << m_shiftVal, LED_Y2 << m_shiftVal); break;
        case 4: m_controller.set(LED_ALL << m_shiftVal, LED_G1 << m_shiftVal); break;
        case 5: m_controller.set(LED_ALL << m_shiftVal, LED_G2 << m_shiftVal); break;
        case 6: m_controller.set(LED_ALL << m_shiftVal, (LED_R | LED_Y1 | LED_Y2) << m_shiftVal);
        }
        break;

    case CHARGING_40:
        switch ((m_step % 22) / 2) {
        case 0: m_controller.switch_off(LED_ALL << m_shiftVal); break;
        case 1: m_controller.set(LED_ALL << m_shiftVal, LED_R  << m_shiftVal); break;
        case 2: m_controller.set(LED_ALL << m_shiftVal, LED_Y1 << m_shiftVal); break;
        case 3: m_controller.set(LED_ALL << m_shiftVal, LED_Y2 << m_shiftVal); break;
        case 4: m_controller.set(LED_ALL << m_shiftVal, LED_G1 << m_shiftVal); break;
        case 5: m_controller.set(LED_ALL << m_shiftVal, LED_G2 << m_shiftVal); break;
        case 6: m_controller.set(LED_ALL << m_shiftVal, (LED_R | LED_Y1) << m_shiftVal);
        }
        break;

    case CHARGING_20:
        switch ((m_step % 22) / 2) {
        case 0: m_controller.switch_off(LED_ALL << m_shiftVal); break;
        case 1: m_controller.set(LED_ALL << m_shiftVal, LED_R  << m_shiftVal); break;
        case 2: m_controller.set(LED_ALL << m_shiftVal, LED_Y1 << m_shiftVal); break;
        case 3: m_controller.set(LED_ALL << m_shiftVal, LED_Y2 << m_shiftVal); break;
        case 4: m_controller.set(LED_ALL << m_shiftVal, LED_G1 << m_shiftVal); break;
        case 5: m_controller.set(LED_ALL << m_shiftVal, LED_G2 << m_shiftVal); break;
        case 6: m_controller.set(LED_ALL << m_shiftVal, LED_R << m_shiftVal);
        }
        break;

    case SEARCHING:
        switch (m_step % 8) {
        case 0: m_controller.set(LED_ALL << m_shiftVal, LED_R  << m_shiftVal); break;
        case 1: m_controller.set(LED_ALL << m_shiftVal, LED_Y1 << m_shiftVal); break;
        case 2: m_controller.set(LED_ALL << m_shiftVal, LED_Y2 << m_shiftVal); break;
        case 3: m_controller.set(LED_ALL << m_shiftVal, LED_G1 << m_shiftVal); break;
        case 4: m_controller.set(LED_ALL << m_shiftVal, LED_G2 << m_shiftVal); break;
        case 5: m_controller.set(LED_ALL << m_shiftVal, LED_G1 << m_shiftVal); break;
        case 6: m_controller.set(LED_ALL << m_shiftVal, LED_Y2 << m_shiftVal); break;
        case 7: m_controller.set(LED_ALL << m_shiftVal, LED_Y1 << m_shiftVal); break;
        }
        break;

    case FLASHING:
        if (m_step % 2) {
            m_controller.switch_on(LED_ALL << m_shiftVal);
        }
        else {
            m_controller.switch_off(LED_ALL << m_shiftVal);
        }
        break;
    }

    ++m_step;
}

void BatteryGauge::on_tick()
{
    update_leds();
}

FunctionDisplay::FunctionDisplay(LedController& controller)
: m_controller(controller)
, m_metronome(100_ms, [](void* self) { static_cast<decltype(this)>(self)->on_tick(); }, this)
{
    set_character(CHAR_NONE);
    set_character_flash_pattern(FLASH_SOLID);
}

void FunctionDisplay::set_character(character_t character)
{
    if (m_character == character) {
        return;
    }

    m_character = character;
    m_step = 0;

    update_leds();
}

void FunctionDisplay::set_character_flash_pattern(flash_pattern_t pattern)
{    
    if (m_characterFlashPattern == pattern) {
        return;
    }

    m_characterFlashPattern = pattern;
    m_step = 0;

    if (pattern == FLASH_SOLID) {
        m_metronome.stop();
    }
    else {
        m_metronome.start();
    }

    update_leds();
}

void FunctionDisplay::update_leds()
{
    switch (m_characterFlashPattern) {
    case FLASH_SOLID:
        m_controller.set(CHAR_8, m_character);
        break;

    case FLASH_FAST:
        if (m_step % 2) {
            m_controller.set(CHAR_8, m_character);
        }
        else {
            m_controller.switch_off(CHAR_8);
        }
        break;

    case FLASH_SLOW:
        if (m_step % 8 < 4) {
            m_controller.set(CHAR_8, m_character);
        }
        else {
            m_controller.switch_off(CHAR_8);
        }
        break;
    }

    ++m_step;
}

void FunctionDisplay::on_tick()
{
    update_leds();
}
