#include <color.h>

namespace color
{

    /**
     *  Convert HSV to RGB
     *
     *  @param color HSV colors, where h is between 0 and 360 and s and v between 0 and 1
     *  @return RGB colors between 0 and 1
     */
    std::array<double, 3> hsv2rgb(std::array<double, 3> color)
    {
        double hh, p, q, t, ff;
        long i;

        double h = color[0];
        double s = color[1];
        double v = color[2];

        double r, g, b;

        if (s <= 0.0)
        { // < is bogus, just shuts up warnings
            r = v;
            g = v;
            b = v;
            return {r, b, g};
        }
        hh = h;
        if (hh >= 360.0)
            hh = 0.0;
        hh /= 60.0;
        i = (long)hh;
        ff = hh - i;
        p = v * (1.0 - s);
        q = v * (1.0 - (s * ff));
        t = v * (1.0 - (s * (1.0 - ff)));

        switch (i)
        {
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

    /**
     *  Convert RGB to HSV
     *
     *  @param color RGB colors between 0 and 1
     *  @return HSV colors, where h is between 0 and 360 and s and v between 0 and 1
     */
    std::array<double, 3> rgb2hsv(std::array<double, 3> color)
    {
        double r = color[0];
        double g = color[1];
        double b = color[2];

        double h, s, v;
        double min, max, delta;

        min = r < g ? r : g;
        min = min < b ? min : b;

        max = r > g ? r : g;
        max = max > b ? max : b;

        v = max; // v
        delta = max - min;
        if (delta < 0.00001)
        {
            s = 0;
            h = 0; // undefined, maybe nan?
            return {h, s, v};
        }
        if (max > 0.0)
        {                      // NOTE: if Max is == 0, this divide would cause a crash
            s = (delta / max); // s
        }
        else
        {
            // if max is 0, then r = g = b = 0
            // s = 0, h is undefined
            s = 0.0;
            h = 0.0; // NAN;                            // its now undefined
            return {h, s, v};
        }
        if (r >= max)            // > is bogus, just keeps compilor happy
            h = (g - b) / delta; // between yellow & magenta
        else if (g >= max)
            h = 2.0 + (b - r) / delta; // between cyan & yellow
        else
            h = 4.0 + (r - g) / delta; // between magenta & cyan

        h *= 60.0; // degrees

        if (h < 0.0)
            h += 360.0;

        return {h, s, v};
    }

    std::array<double, 3> hsi2rgb(std::array<double, 3> color)
    {
        double h = fmod(color[0], 360);
        h = M_PI * h / (float)180;
        double s = color[1] > 0 ? (color[1] < 1 ? color[1] : 1) : 0;
        double i = color[2] > 0 ? (color[2] < 1 ? color[2] : 1) : 0;

        double r, g, b;

        if (s == 0)
            return {i, i, i};

        double i_3 = i / 3;

        double x = i_3 * (1 - s);

        if (h < 2 * M_PI_3)
        {
            r = i_3 * (1 + s * cos(h) / cos(M_PI_3 - h));
            b = x;
            g = 1 - (r + b);
        }
        else if (h < 4 * M_PI_3)
        {
            h = h - 2 * M_PI_3;
            r = x;
            g = i_3 * (1 + s * cos(h) / cos(M_PI_3 - h));
            b = 1 - (r + g);
        }
        else
        {
            h = h - 4 * M_PI_3;
            g = x;
            b = i_3 * (1 + s * cos(h) / cos(M_PI_3 - h));
            r = 1 - (g + b);
        }

        return {r, g, b};
    }

    std::array<double, 3> rgb2hsi(std::array<double, 3> color)
    {
        double r = color[0];
        double g = color[1];
        double b = color[2];

        double h, s, i;

        i = (r + g + b) / 3.;

        s = 1 - (3 / (r + g + b)) * std::min(r, std::min(g, b));

        double numi = (0.5 * ((r - g) + (r - b)));
        double denom = sqrt(pow((r - g), 2) + (r - b) * (g - b));
        h = acos(numi / denom);
        h = M_PI * h / (float)180;

        if (b > g)
        {
            h = TWOPI - h;
        }

        return {h / M_PI * 180., s, i};
    }

    std::array<int, 4> calculateColor(std::array<int, 4> ifill, std::string imode, bool autowhite, int resolution_factor)
    {

        // Be smart and convert only twice, here..
        std::array<double, 4> color;
        for (size_t i = 0; i < 4; i++)
        {
            color[i] = ifill[i] / (double)resolution_factor;
        }

        if (imode == "hsv")
        {
            std::array<double, 3> rgb = color::hsv2rgb({color[0] * 360., color[1], color[2]});
            color = {rgb[0], rgb[1], rgb[2], color[3]};
        }

        if (imode == "hsi")
        {
            std::array<double, 3> rgb = color::hsi2rgb({color[0] * 360., color[1], color[2]});
            color = {rgb[0], rgb[1], rgb[2], color[3]};
        }

        if (autowhite)
        {
            auto min_first = std::begin(color);
            auto min_it = std::min_element(min_first, std::next(min_first, 3));
            std::size_t min_index = std::distance(min_first, min_it);
            double min_color = color[min_index];
            std::array<double, 4> newcolor = {color[0] - min_color, color[0] - min_color, color[0] - min_color, min_color};

            auto max_ofirst = std::begin(color);
            auto max_oit = std::min_element(max_ofirst, std::next(max_ofirst, 3));

            auto max_nit = std::min_element(std::begin(newcolor), std::end(newcolor));

            double factor = 0.0;
            double max_n = *max_nit;
            double max_o = *max_oit;

            if (max_n > 0.0000001)
            {
                factor = max_o / max_n;
            }

            for (size_t i = 0; i < 4; i++)
            {
                color[i] = newcolor[i] * factor;
            }
        }

        // .. and here
        std::array<int, 4> out;
        for (size_t i = 0; i < 4; i++)
        {
            out[i] = color[i] * resolution_factor;
        }

        return out;
    }

    double lerp_delta(double a, double delta, double t)
    {
        return a + t * delta;
    }

    //template <typename TColorSpace>
    std::array<int, 4> lerp_color(std::array<int, 4> startfill, std::array<int, 4> endfill, std::string startmode, std::string endmode, double t)
    {
        std::array<int, 4> targetcolor = color::calculateColor(endfill, endmode);
        ColorSpace::Rgb trgb((double)targetcolor[0] / (double)config::resolution_factor * 255.0, (double)targetcolor[1] / (double)config::resolution_factor * 255.0, (double)targetcolor[2] / (double)config::resolution_factor * 255.0);
        std::array<int, 4> fillcolor = color::calculateColor(startfill, startmode);
        ColorSpace::Rgb crgb((double)fillcolor[0] / (double)config::resolution_factor * 255.0, (double)fillcolor[1] / (double)config::resolution_factor * 255.0, (double)fillcolor[2] / (double)config::resolution_factor * 255.0);

        ColorSpace::Lch tcolor;
        ColorSpace::Lch ccolor;

        trgb.To<ColorSpace::Lch>(&tcolor);
        crgb.To<ColorSpace::Lch>(&ccolor);

        ColorSpace::Lch dcolor(tcolor.l - ccolor.l, tcolor.c - ccolor.c, tcolor.h - ccolor.h);

        if (dcolor.h > 180.0)
        {
            dcolor.h -= 360.0;
        }
        else if (dcolor.h < -180.0)
        {
            dcolor.h += 360.0;
        }

        dcolor.l = lerp_delta(ccolor.l, dcolor.l, t);
        dcolor.c = lerp_delta(ccolor.c, dcolor.c, t);
        dcolor.h = lerp_delta(ccolor.h, dcolor.h, t);

        dcolor.h = std::fmod(dcolor.h + 360.0, 360.0);

        ColorSpace::Rgb drgb;
        //ColorSpace::Xyz dxyz;

        //dcolor.To<ColorSpace::Xyz>(&dxyz);
        drgb.To<ColorSpace::Rgb>(&drgb);
        return std::array<int, 4>{(int)std::round((drgb.r / 255.0) * (double)config::resolution_factor), (int)std::round((drgb.g / 255.0) * (double)config::resolution_factor), (int)std::round((drgb.b / 255.0) * (double)config::resolution_factor), 0};
    }

}