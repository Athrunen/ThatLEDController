#include "color.h"

std::array<double, 3> color::hsv2rgb(std::array<double, 3> color)
{
    double hh, p, q, t, ff;
    long i;

    double h = color[0];
    double s = color[1];
    double v = color[2];

    double r, g, b;

    if(s <= 0.0) {       // < is bogus, just shuts up warnings
        r = v;
        g = v;
        b = v;
        return {r, b, g};
    }
    hh = h;
    if(hh >= 360.0) hh = 0.0;
    hh /= 60.0;
    i = (long)hh;
    ff = hh - i;
    p = v * (1.0 - s);
    q = v * (1.0 - (s * ff));
    t = v * (1.0 - (s * (1.0 - ff)));

    switch(i) {
    case 0:
        r = v;
        g = t;
        b = p;
        break;
    case 1:
        r = q;
        g = v;
        b = p;
        break;
    case 2:
        r = p;
        g = v;
        b = t;
        break;

    case 3:
        r = p;
        g = q;
        b = v;
        break;
    case 4:
        r = t;
        g = p;
        b = v;
        break;
    case 5:
    default:
        r = v;
        g = p;
        b = q;
        break;
    }

    return {r, g, b};
}

std::array<double, 3> color::rgb2hsv(std::array<double, 3> color)
{
    double r = color[0];
    double g = color[1];
    double b = color[2];

    double      h, s, v;
    double      min, max, delta;

    min = r < g ? r : g;
    min = min  < b ? min  : b;

    max = r > g ? r : g;
    max = max  > b ? max  : b;

    v = max;                                // v
    delta = max - min;
    if (delta < 0.00001)
    {
        s = 0;
        h = 0; // undefined, maybe nan?
        return {h, s, v};
    }
    if( max > 0.0 ) { // NOTE: if Max is == 0, this divide would cause a crash
        s = (delta / max);                  // s
    } else {
        // if max is 0, then r = g = b = 0              
        // s = 0, h is undefined
        s = 0.0;
        h = 0.0;  //NAN;                            // its now undefined
        return {h, s, v};
    }
    if( r >= max )                           // > is bogus, just keeps compilor happy
        h = ( g - b ) / delta;        // between yellow & magenta
    else
    if( g >= max )
        h = 2.0 + ( b - r ) / delta;  // between cyan & yellow
    else
        h = 4.0 + ( r - g ) / delta;  // between magenta & cyan

    h *= 60.0;                              // degrees

    if( h < 0.0 )
        h += 360.0;

    return {h, s, v};
}

std::array<double, 3> color::hsi2rgb(std::array<double, 3> color)
{
    double h = fmod(color[0], 360);
    double s = color[1] > 0 ? (color[1] < 1 ? color[1] : 1) : 0;
    double i = color[2] > 0 ? (color[2] < 1 ? color[2] : 1) : 0;

    double r, g, b;

    if(s == 0) 
        return {i, i, i};
    
    double x = i / 3 * (1 - s);
    double y = i / 3 * (1 + s * cos(h) / cos(M_PI_3 - h));
    double z = i / 3 * (1 + s * (1 - cos(h) / cos(M_PI_3 - h)));

    if (h < 2 * M_PI_3) {
        r = y;
        g = z;
        b = x;
    } else if (h < 4 * M_PI_3) {
        r = x;
        g = y;
        b = z;
    } else {
        r = z;
        g = x;
        b = y;
    }

    return {r, g, b};
}

std::array<double, 3> color::rgb2hsi(std::array<double, 3> color) {
    double r = color[0];
    double g = color[1];
    double b = color[2];

    double h, s, i;

    double s = r + g + b;

    i = s / 3.;

    double rn = r / s;
    double gn = g / s;
    double bn = b / s;

    s = 1 - 3 * std::min(rn, std::min(gn, bn));
    
    h = acos((0.5 * ((rn - gn) + (rn - bn))) / (sqrt((rn - gn) * (rn - gn) + (rn - bn) * (gn - bn))));

    if(b > g)
	{
		h = TWOPI - h;
	}

    return {h, s, i};
}