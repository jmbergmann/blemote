#pragma once

#include "lib/twi.hh"
#include "lib/time.hh"

#include <stdbool.h>
#include <stdint.h>


class LedController final
{
public:
    typedef uint32_t led_states_t;

    enum {
        LED_NONE             = 0,

        LED_USB_G            = (1<<0),
        LED_USB_Y            = (1<<1),

        LED_BAT_BOTTOM_R     = (1<<2),
        LED_BAT_BOTTOM_Y1    = (1<<3),
        LED_BAT_BOTTOM_Y2    = (1<<4),
        LED_BAT_BOTTOM_G1    = (1<<5),
        LED_BAT_BOTTOM_G2    = (1<<6),

        LED_BAT_TOP_R        = (1<<7),
        LED_BAT_TOP_Y1       = (1<<8),
        LED_BAT_TOP_Y2       = (1<<9),
        LED_BAT_TOP_G1       = (1<<10),
        LED_BAT_TOP_G2       = (1<<11),

        LED_SEG_DOT_TOP      = (1<<12),
        LED_SEG_DOT_BOTTOM   = (1<<13),
        LED_SEG_TOP          = (1<<14),
        LED_SEG_TOP_LEFT     = (1<<15),
        LED_SEG_TOP_RIGHT    = (1<<16),
        LED_SEG_MIDDLE       = (1<<17),
        LED_SEG_BOTTOM       = (1<<18),
        LED_SEG_BOTTOM_LEFT  = (1<<19),
        LED_SEG_BOTTOM_RIGHT = (1<<20),
    };

    LedController(lib::Clock& clock, lib::Twi& twi);
    ~LedController();

    lib::Clock& clock()
    {
        return m_clock;
    }

    void set_all(led_states_t states);
    void set(led_states_t mask, led_states_t states);
    void switch_on(led_states_t leds);
    void switch_off(led_states_t leds);
    void toggle(led_states_t leds);

private:
    lib::Clock&  m_clock;
    lib::Twi&    m_twi;
    led_states_t m_states;

    void write_lsx_registers(uint8_t address, const uint8_t* ls);
    void update();
};

class BatteryGauge
{
public:
    enum pattern_t {
        OFF,
        SOC_100,
        SOC_80,
        SOC_60,
        SOC_40,
        SOC_20,
        CRITICALLY_LOW,
        CHARGING_COMPLETE,
        CHARGING_100,
        CHARGING_80,
        CHARGING_60,
        CHARGING_40,
        CHARGING_20,
        SEARCHING,
        FLASHING,
    };

    void set_pattern(pattern_t pattern);

protected:
    BatteryGauge(LedController& controller, int shiftVal);

private:
    enum led_t {
        LED_NONE = 0,
        LED_R  = (1<<0),
        LED_Y1 = (1<<1),
        LED_Y2 = (1<<2),
        LED_G1 = (1<<3),
        LED_G2 = (1<<4),
        LED_ALL = LED_R | LED_Y1 | LED_Y2 | LED_G1 | LED_G2,
    };

    LedController& m_controller;
    lib::Metronome m_metronome;
    const int      m_shiftVal;
    pattern_t      m_pattern;
    int            m_step;

    void update_leds();
    void on_tick();
};

class BoardBatteryGauge : public BatteryGauge
{
public:
    BoardBatteryGauge(LedController& controller)
    : BatteryGauge(controller, 7)
    {
    }
};

class RemoteBatteryGauge : public BatteryGauge
{
public:
    RemoteBatteryGauge(LedController& controller)
    : BatteryGauge(controller, 2)
    {
    }
};

class FunctionDisplay
{
public:
    enum character_t {
        CHAR_NONE = LedController::LED_NONE,
        CHAR_0 = LedController::LED_SEG_TOP
               | LedController::LED_SEG_TOP_LEFT
               | LedController::LED_SEG_TOP_RIGHT
               | LedController::LED_SEG_BOTTOM
               | LedController::LED_SEG_BOTTOM_LEFT
               | LedController::LED_SEG_BOTTOM_RIGHT,
        CHAR_1 = LedController::LED_SEG_TOP_RIGHT
               | LedController::LED_SEG_BOTTOM_RIGHT,
        CHAR_2 = LedController::LED_SEG_TOP
               | LedController::LED_SEG_TOP_RIGHT
               | LedController::LED_SEG_MIDDLE
               | LedController::LED_SEG_BOTTOM_LEFT
               | LedController::LED_SEG_BOTTOM,
        CHAR_3 = LedController::LED_SEG_TOP
               | LedController::LED_SEG_TOP_RIGHT
               | LedController::LED_SEG_MIDDLE
               | LedController::LED_SEG_BOTTOM
               | LedController::LED_SEG_BOTTOM_RIGHT,
        CHAR_4 = LedController::LED_SEG_TOP_LEFT
               | LedController::LED_SEG_TOP_RIGHT
               | LedController::LED_SEG_MIDDLE
               | LedController::LED_SEG_BOTTOM_RIGHT,
        CHAR_5 = LedController::LED_SEG_TOP
               | LedController::LED_SEG_TOP_LEFT
               | LedController::LED_SEG_MIDDLE
               | LedController::LED_SEG_BOTTOM
               | LedController::LED_SEG_BOTTOM_RIGHT,
        CHAR_6 = LedController::LED_SEG_TOP
               | LedController::LED_SEG_TOP_LEFT
               | LedController::LED_SEG_MIDDLE
               | LedController::LED_SEG_BOTTOM
               | LedController::LED_SEG_BOTTOM_LEFT
               | LedController::LED_SEG_BOTTOM_RIGHT,
        CHAR_7 = LedController::LED_SEG_TOP
               | LedController::LED_SEG_TOP_RIGHT
               | LedController::LED_SEG_BOTTOM_RIGHT,
        CHAR_8 = LedController::LED_SEG_TOP
               | LedController::LED_SEG_TOP_LEFT
               | LedController::LED_SEG_TOP_RIGHT
               | LedController::LED_SEG_MIDDLE
               | LedController::LED_SEG_BOTTOM
               | LedController::LED_SEG_BOTTOM_LEFT
               | LedController::LED_SEG_BOTTOM_RIGHT,
        CHAR_9 = LedController::LED_SEG_TOP
               | LedController::LED_SEG_TOP_LEFT
               | LedController::LED_SEG_TOP_RIGHT
               | LedController::LED_SEG_MIDDLE
               | LedController::LED_SEG_BOTTOM
               | LedController::LED_SEG_BOTTOM_RIGHT,
        CHAR_A = LedController::LED_SEG_TOP
               | LedController::LED_SEG_TOP_LEFT
               | LedController::LED_SEG_TOP_RIGHT
               | LedController::LED_SEG_MIDDLE
               | LedController::LED_SEG_BOTTOM_LEFT
               | LedController::LED_SEG_BOTTOM_RIGHT,
        CHAR_C = LedController::LED_SEG_TOP
               | LedController::LED_SEG_TOP_LEFT
               | LedController::LED_SEG_BOTTOM
               | LedController::LED_SEG_BOTTOM_LEFT,
        CHAR_E = LedController::LED_SEG_TOP
               | LedController::LED_SEG_TOP_LEFT
               | LedController::LED_SEG_MIDDLE
               | LedController::LED_SEG_BOTTOM
               | LedController::LED_SEG_BOTTOM_LEFT,
        CHAR_F = LedController::LED_SEG_TOP
               | LedController::LED_SEG_TOP_LEFT
               | LedController::LED_SEG_MIDDLE
               | LedController::LED_SEG_BOTTOM_LEFT,
        CHAR_H = LedController::LED_SEG_TOP_LEFT
               | LedController::LED_SEG_TOP_RIGHT
               | LedController::LED_SEG_MIDDLE
               | LedController::LED_SEG_BOTTOM_LEFT
               | LedController::LED_SEG_BOTTOM_RIGHT,
        CHAR_L = LedController::LED_SEG_TOP_LEFT
               | LedController::LED_SEG_BOTTOM
               | LedController::LED_SEG_BOTTOM_LEFT,
        CHAR_P = LedController::LED_SEG_TOP
               | LedController::LED_SEG_TOP_LEFT
               | LedController::LED_SEG_TOP_RIGHT
               | LedController::LED_SEG_MIDDLE
               | LedController::LED_SEG_BOTTOM_LEFT,
        CHAR_U = LedController::LED_SEG_TOP_LEFT
               | LedController::LED_SEG_TOP_RIGHT
               | LedController::LED_SEG_BOTTOM
               | LedController::LED_SEG_BOTTOM_LEFT
               | LedController::LED_SEG_BOTTOM_RIGHT,
    };

    enum dot_t {
        DOT_TOP = LedController::LED_SEG_DOT_TOP,
        DOT_BOTTOM = LedController::LED_SEG_DOT_BOTTOM
    };

    enum flash_pattern_t {
        FLASH_SOLID,
        FLASH_FAST,
        FLASH_SLOW,
    };

    FunctionDisplay(LedController& controller);

    void set_character(character_t character);
    void set_character_flash_pattern(flash_pattern_t pattern);

private:
    LedController&  m_controller;
    lib::Metronome  m_metronome;
    character_t     m_character;
    flash_pattern_t m_characterFlashPattern;
    int             m_step;

    void update_leds();
    void on_tick();
};
