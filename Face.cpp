#include "Face.h"
#include "Texture.h"

void Face::Loop() {
    // input?
}

void Face::Init() {
    textures["neutral"] = Texture("wa");
}

Texture Face::GetTexture() {
    return (textures.find("neutral")->second);
}
