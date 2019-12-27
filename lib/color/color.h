#pragma once
#ifndef COLOR_H
#define COLOR_H

#include <math.h>
#include <array>

const double M_PI_3 = M_PI / 3.;
const double TWOPI = M_PI * 2.;

namespace color {

    std::array<double, 3> hsv2rgb(std::array<double, 3> color);
    std::array<double, 3> rgb2hsv(std::array<double, 3> color);

    std::array<double, 3> hsi2rgb(std::array<double, 3> color);
    std::array<double, 3> rgb2hsi(std::array<double, 3> color);

}

#endif