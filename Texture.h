#ifndef TEXTURE_H
#define TEXTURE_H

#include <vector>
#include <string>

class Texture {
public:
    std::vector<std::vector<int>> textureMap; // int is on/off 0-255
    Texture() = default;
    Texture(const std::string& path);
};

#endif // TEXTURE_H
