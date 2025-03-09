#ifndef GRADIENT_H
#define GRADIENT_H

#include <vector>

struct TZColor {
    double r, g, b;
};

struct TZHSL {
    double h, s, l;
};

class Gradient {
public:
    static TZHSL tz_rgbToHsl(const TZColor &color);
    static TZColor tz_hslToRgb(const TZHSL &hsl);
    static TZColor tz_interpolateColor(const TZColor &c1, const TZColor &c2, double t);
    static std::vector<std::vector<TZColor>> preprocessGradient(const std::vector<std::vector<TZColor>> &gradientMap, int width, int height);
};

#endif // GRADIENT_H
