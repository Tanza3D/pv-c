#ifndef TEXTURESOURCE_H
#define TEXTURESOURCE_H

#include "Texture.h"

class TextureSource {
public:
    virtual void Loop() = 0;
    virtual void Init() = 0;

    TextureSource();
};

#endif // TEXTURESOURCE_H
