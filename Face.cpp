#include "Face.h"
#include "Texture.h"
#include <iostream>

void Face::Loop() {
    // input?
}

void Face::Init() {
    std::cout << "good morning?" << std::endl;
    textures["neutral"] = Texture("tex/face", "neutral");
    textures["happy"] = Texture("tex/face", "happy");
    textures["sad"] = Texture("tex/face", "sad");
}

Texture Face::GetTexture() {
    return (textures.find("neutral")->second);
}
