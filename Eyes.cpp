#include "Eyes.h"
#include "Texture.h"
#include <iostream>

void Eyes::Loop() {
    // input?
}

void Eyes::Init() {
    textures["neutral"] = Texture("tex/eyes", "neutral");
}

Texture Eyes::GetTexture() {
    return (textures.find("neutral")->second);
}
