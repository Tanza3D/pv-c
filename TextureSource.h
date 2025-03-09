#ifndef TEXTURESOURCE_H
#define TEXTURESOURCE_H

#include "Texture.h"

class TextureSource {
public:
    virtual void Loop();
    virtual void Init();
    virtual Texture GetTexture();

    TextureSource();
};

#endif // TEXTURESOURCE_H
