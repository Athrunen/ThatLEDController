#ifndef CONFIG_H
#define CONFIG_H

#include <array>
#include <Arduino.h>

namespace config
{

    //#define FULLMODE

    #ifdef FULLMODE

    const byte
        SWITCH_PIN(35),
        UP_PIN(33),
        DOWN_PIN(32);
    const int screen_steps = 255;
    
    #endif

    const byte led_pins[4] = {16, 17, 18, 19}; // {15, 27, 2, 16}
    const int resolution = 16;
    const int resolution_factor = pow(2, resolution) - 1;
    const bool debug = true;
    const std::array<std::string, 4> modes = {"manual", "hsv", "hsi"};
}

#endif