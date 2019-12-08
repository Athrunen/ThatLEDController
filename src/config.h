#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <array>

namespace config
{
    const byte
        SWITCH_PIN(35),
        UP_PIN(33),
        DOWN_PIN(32);
    const byte led_pins[4] = {15, 27, 2, 16};
    const byte touchpin = 13;
    const int touchthd = 6;
}

#endif