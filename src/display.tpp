template<std::size_t SIZE>
void Display::drawColorInput(int selector_position, std::array<int, SIZE> fill, std::array<String, SIZE> names, float spacing) {
    int amount = fill.size();
    float totalspacing = (amount - 1) * spacing;
    int radius = round((width - 1 - outerdist * 2 - totalspacing) / (2 * amount));
    for (size_t i = 1; i < amount + 1; i++)
    {
        dp->setColor(WHITE);
        float fillheight = 1 / 255.0 * fill[i - 1];
        float circledist = radius * (1 + 2 * (i - 1));
        float spacingdist = spacing * (i - 1);
        float positionX = round(outerdist + spacingdist + circledist);
        float positionY = round(height / 5);
        float calcheight = round(height / 5.0 * 3.0);
        float dist = round(calcheight * fillheight);
        dp->drawRect(positionX - radius, positionY, radius * 2, calcheight);
        dp->fillRect(positionX - radius, positionY + calcheight - dist, radius * 2, dist);
        dp->setTextAlignment(TEXT_ALIGN_CENTER);
        dp->drawString(positionX, 0, SSTR(fill[i - 1]).c_str());
        if (i == selector_position + 1) {
            dp->fillRect(positionX - radius / 2.5, positionY * 4.2, 8, 9);
            dp->setColor(BLACK);
        }
        dp->drawString(positionX, positionY * 4, names[i - 1]);
    }
}