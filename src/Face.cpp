#include "Face.h"

Face::Face(const std::string& name, const std::string& path)
    : name(name), texture(path, name, ColorChannel::ALPHA), texture_open(path, name + "_open", ColorChannel::ALPHA) {};
Face::Face()
    : name(""), texture("", "", ColorChannel::ALPHA), texture_open("", "", ColorChannel::ALPHA) {}  // default constructor


Texture Face::GetTexture(bool open) const {
    Texture t = open ? texture_open : texture;
    return t;
}

void Face::SetColourOverride(std::vector<std::vector<TZColor>> gradient) {
#ifndef ESP_PLATFORM
    hasOverride = true;
    override = gradient;
#endif
}

