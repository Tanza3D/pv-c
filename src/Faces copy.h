#ifndef FACE_H
#define FACE_H

#include <map>
#include <vector>
#include "TextureSource.h"
#include "Texture.h"

class Faces : public TextureSource {
private:
    std::map<std::string, Texture> textures;

public:
std::string texture;
    void Loop() override;
    void Init() override;
    Texture GetTexture() override;
};

#endif
