#include "display.h"

Display::Display(uint8_t address, uint8_t sda, uint8_t scl, OLEDDISPLAY_GEOMETRY g) {
    dp = new SSD1306Wire (address, sda, scl, g);
}

void Display::setup() {
    width = dp->getWidth();
    height = dp->getHeight();
    dp->init();
    dp->setContrast(255);
}

void Display::clear() {
    dp->clear();
}

void Display::display() {
    dp->display();
}

void Display::drawRGBW(int selector_position, std::array<int, 4> fill) {
    dp->clear();
    drawColorInput(selector_position, fill, {"R", "G", "B", "W"});
    dp->display();
}
void Display::drawHSVW(int selector_position, std::array<int, 4> fill) {
    dp->clear();
    drawColorInput(selector_position, fill, {"H", "S", "V", "W"});
    dp->display();
}