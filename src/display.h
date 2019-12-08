#pragma once
#ifndef DISPLAY_H
#define DISPLAY_H

#include "SSD1306Wire.h"
#include "helpers.h"
#include <array>

class Display {
    private:
        float outerdist = 5;
        float height;
        float width;
        SSD1306Wire* dp = 0;

    public:
        Display(uint8_t address, uint8_t sda, uint8_t scl, OLEDDISPLAY_GEOMETRY g = GEOMETRY_128_64);

        void setup();

        template<std::size_t SIZE>
        void drawColorInput(int selector_position, std::array<int, SIZE> fill, std::array<String, SIZE> names, float spacing = 13);

        template<std::size_t SIZE>
        void drawMenu(int selector_position, std::array<int, SIZE> text);

        void drawRGBW(int selector_position, std::array<int, 4> fill);
        void drawHSVW(int selector_position, std::array<int, 4> fill);

        void clear();
        void display();


};

#include "display.tpp"

#endif