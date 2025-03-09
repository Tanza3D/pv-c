#include "Eyes.h"
#include "Texture.h"

void Eyes::Loop() {
    // input?
}

void Eyes::Init() {
    textures["neutral"] = Texture("wa");
}

Texture Eyes::GetTexture() {
    return (textures.find("neutral")->second);
}
