#include "config.h"

template<std::size_t SIZE>
void Display::drawColorInput(int selector_position, std::array<int, SIZE> fill, std::array<String, SIZE> names, float spacing) {
    int amount = fill.size();
    float totalspacing = (amount - 1) * spacing;
    int element_width = round((Display::width - 1 - Display::outerdist * 2 - totalspacing) / amount);
    for (size_t i = 0; i < amount; i++)
    {
        dp->setColor(WHITE);
        float fillpercent = fill[i] / float(config::resolution_factor);
        float elementoffset = element_width * i;
        float spacingdist = spacing * i;
        float positionX = round(Display::outerdist + spacingdist + elementoffset);
        float positionY = round(Display::height * 0.2);
        float maxheight = round(Display::height * 0.6);
        float fillheight = round(maxheight * fillpercent);
        dp->drawRect(positionX, positionY, element_width, maxheight);
        dp->fillRect(positionX, positionY + maxheight - fillheight, element_width, fillheight);
        dp->setTextAlignment(TEXT_ALIGN_CENTER);
        dp->drawString(positionX + element_width / 2, 0, SSTR(fill[i]).c_str());
        if (i == selector_position) {
            dp->fillRect(positionX + element_width * 0.3, positionY * 4.2, 8, 9);
            dp->setColor(BLACK);
        }
        dp->drawString(positionX + element_width / 2, positionY * 4, names[i]);
    }
}