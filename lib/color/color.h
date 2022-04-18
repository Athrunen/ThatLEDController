#pragma once
#ifndef COLOR_H
#define COLOR_H

#include <math.h>
#include <array>
#include <algorithm>
#include <string>
#include <ColorSpace.h>

#ifdef UNIT_TEST

namespace config {

    const int resolution = 16;
    const int resolution_factor = pow(2, resolution) - 1;

}

#endif

#ifndef UNIT_TEST

#include "config.h"

#endif

const double M_PI_3 = M_PI / 3.;
const double TWOPI = M_PI * 2.;

namespace color {
    std::array<int, 4> calculateColor(std::array<int, 4> ifill, std::string imode = "manual", bool autowhite = false, int resolution_factor = config::resolution_factor);
    std::array<int, 4> lerp_color(std::array<int, 4> startfill, std::array<int, 4> endfill, std::string startmode, std::string endmode, double t);

    std::array<double, 3> hsv2rgb(std::array<double, 3> color);
    std::array<double, 3> rgb2hsv(std::array<double, 3> color);

    std::array<double, 3> hsi2rgb(std::array<double, 3> color);
    std::array<double, 3> rgb2hsi(std::array<double, 3> color);

}

#endif