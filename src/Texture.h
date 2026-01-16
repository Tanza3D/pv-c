#ifndef TEXTURE_H
#define TEXTURE_H

#include <vector>
#include <string>

enum class ColorChannel {
    RED,
    GREEN,
    BLUE,
    ALPHA
};

class Texture {
public:
    std::vector<std::vector<double> > textureMap; // 0-1
    Texture() = default;

    Texture(const std::string &path, const std::string &file, ColorChannel channel);

private:
    void loadTexture(const std::string &filePath, std::vector<std::vector<double> > &textureMap, ColorChannel channel);
};

#endif // TEXTURE_H
