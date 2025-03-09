#ifndef EYES_H
#define EYES_H

#include <map>
#include <vector>
#include "TextureSource.h"
#include "Texture.h"

class Eyes : public TextureSource {
private:
    std::map<std::string, Texture> textures;

public:
    void Loop() override;
    void Init() override;
    Texture GetTexture() override;
};

#endif
