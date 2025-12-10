#include "Face.h"

Face::Face(const std::string& name, const std::string& path)
    : name(name), texture(path, name) {};
Face::Face()
        : name(""), texture("", "") {}  // default constructor


Texture Face::GetTexture() const {
    Texture t = texture;
    return t;
}

void Face::SetColourOverride(std::vector<std::vector<TZColor>> gradient) {
#ifndef ESP_PLATFORM
    hasOverride = true;
    override = gradient;
#endif
}

