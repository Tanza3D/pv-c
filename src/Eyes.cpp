#include "Eyes.h"
#include "Eye.h"
#include "Texture.h"
#include <iostream>

void Eyes::Loop() {
    // input?
}

void Eyes::Init() {
    eyes["neutral"] = Eye("neutral", "tex/eyes");
    currentEye = "neutral";

#ifndef ESP_PLATFORM
    // ESP doesn't have enough ram for the gradient maps, but we don't need them anyway so it's fine
    auto gradientMap = Gradient::easyGradient("#00ff00", "#00ffff", "#ffff00", "#ffffff");
    eyes["neutral"].SetColourOverride(Gradient::preprocessGradient(gradientMap));
    auto gradientMap2 = Gradient::easyGradient("#ff0000", "#ff0000", "#ff0000", "#ff0000");
    eyes["neutral"].SetPupilColourOverride(Gradient::preprocessGradient(gradientMap2));
#endif
}

Eye Eyes::GetCurrent() {
    return eyes.at(currentEye);
}

std::string Eyes::GetCurrentName() {
    return currentEye;
}

std::vector<std::string> Eyes::getTextureNames() {
    std::vector<std::string> names;
    for (const auto& pair : eyes) {
        names.push_back(pair.first);
    }
    return names;
}