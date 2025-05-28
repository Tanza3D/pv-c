#include "Eyes.h"
#include "Texture.h"
#include <iostream>

void Eyes::Loop() {
    // input?
}

void Eyes::Init() {
    texture = "neutral";
    textures["neutral"] = Texture("tex/eyes", "neutral");
}

Texture Eyes::GetTexture() {
    return (textures.find(texture)->second);
}
