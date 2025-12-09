#include "Face.h"
#include "Texture.h"
#include <iostream>

void Faces::Loop() {
    // input?
}

void Faces::Init() {
    texture = "happy";
    textures["neutral"] = Texture("tex/face", "neutral");
    textures["happy"] = Texture("tex/face", "happy");
    textures["sad"] = Texture("tex/face", "sad");
}

Texture Faces::GetTexture() {
    return (textures.find(texture)->second);
}
