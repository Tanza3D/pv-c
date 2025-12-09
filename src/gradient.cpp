#include "gradient.h"
#include <algorithm>
#include <cmath>

#include <string>
#include <sstream>
#include <vector>

TZColor hexToTZColor(const std::string &hex) {
    unsigned int r, g, b;
    if (hex[0] == '#') {
        std::stringstream ss;
        ss << std::hex << hex.substr(1, 2);
        ss >> r;
        ss.clear();
        ss << std::hex << hex.substr(3, 2);
        ss >> g;
        ss.clear();
        ss << std::hex << hex.substr(5, 2);
        ss >> b;
    } else { // without #
        std::stringstream ss;
        ss << std::hex << hex.substr(0, 2);
        ss >> r;
        ss.clear();
        ss << std::hex << hex.substr(2, 2);
        ss >> g;
        ss.clear();
        ss << std::hex << hex.substr(4, 2);
        ss >> b;
    }
    return {static_cast<double>(static_cast<int>(r)), static_cast<double>(static_cast<int>(g)), static_cast<double>(static_cast<int>(b))};
}



TZHSL Gradient::tz_rgbToHsl(const TZColor &color) {
    double r = color.r / 255.0;
    double g = color.g / 255.0;
    double b = color.b / 255.0;

    double max = std::max(r, std::max(g, b));
    double min = std::min(r, std::min(g, b));

    double h, s, l = (max + min) / 2.0;

    if (max == min) {
        h = s = 0.0; // achromatic
    } else {
        double d = max - min;
        s = (l > 0.5) ? d / (2.0 - max - min) : d / (max + min);

        if (max == r) {
            h = (g - b) / d + (g < b ? 6.0 : 0.0);
        } else if (max == g) {
            h = (b - r) / d + 2.0;
        } else {
            h = (r - g) / d + 4.0;
        }
        h /= 6.0;
    }
    return {h, s, l};
}

TZColor Gradient::tz_hslToRgb(const TZHSL &hsl) {
    double r, g, b;
    if (hsl.s == 0) {
        r = g = b = hsl.l; // achromatic
    } else {
        auto hue2rgb = [](double p, double q, double t) {
            if (t < 0) t += 1;
            if (t > 1) t -= 1;
            if (t < 1 / 6.0) return p + (q - p) * 6 * t;
            if (t < 1 / 2.0) return q;
            if (t < 2 / 3.0) return p + (q - p) * (2 / 3.0 - t) * 6;
            return p;
        };

        double q = (hsl.l < 0.5) ? hsl.l * (1 + hsl.s) : hsl.l + hsl.s - hsl.l * hsl.s;
        double p = 2 * hsl.l - q;
        r = hue2rgb(p, q, hsl.h + 1.0 / 3.0);
        g = hue2rgb(p, q, hsl.h);
        b = hue2rgb(p, q, hsl.h - 1.0 / 3.0);
    }
    return {std::round(r * 255), std::round(g * 255), std::round(b * 255)};
}

TZColor Gradient::tz_interpolateColor(const TZColor &c1, const TZColor &c2, double t) {
    TZHSL hsl1 = tz_rgbToHsl(c1);
    TZHSL hsl2 = tz_rgbToHsl(c2);

    double h = hsl1.h + (hsl2.h - hsl1.h) * t;
    double s = hsl1.s + (hsl2.s - hsl1.s) * t;
    double l = hsl1.l + (hsl2.l - hsl1.l) * t;

    return tz_hslToRgb({h, s, l});
}

std::vector<std::vector<TZColor>> Gradient::preprocessGradient(const std::vector<std::vector<TZColor>> &gradientMap, int width, int height) {
    std::vector<std::vector<TZColor>> result(height, std::vector<TZColor>(width));

    TZColor topLeft = gradientMap[0][0];
    TZColor topRight = gradientMap[0][1];
    TZColor bottomLeft = gradientMap[1][0];
    TZColor bottomRight = gradientMap[1][1];

    for (int y = 0; y < height; ++y) {
        double ty = y / static_cast<double>(height - 1);
        for (int x = 0; x < width; ++x) {
            double tx = x / static_cast<double>(width - 1);
            TZColor topColor = tz_interpolateColor(topLeft, topRight, tx);
            TZColor bottomColor = tz_interpolateColor(bottomLeft, bottomRight, tx);
            result[y][x] = tz_interpolateColor(topColor, bottomColor, ty);
        }
    }
    return result;
}

std::vector<std::vector<TZColor>> Gradient::easyGradient(const std::string &topLeftHex,
                                               const std::string &topRightHex,
                                               const std::string &bottomLeftHex,
                                               const std::string &bottomRightHex) {
    return {
            {hexToTZColor(topLeftHex), hexToTZColor(topRightHex)},
            {hexToTZColor(bottomLeftHex), hexToTZColor(bottomRightHex)}
    };
}
