#ifndef TEXTURE_H
#define TEXTURE_H

#include <vector>
#include <string>

enum class ColorChannel {
    RED,    // Extract red channel (r == 255)
    GREEN,  // Extract green channel (g == 255)
    BLUE    // Extract blue channel (b == 255)
};

class Texture {
public:
    std::vector<std::vector<double>> textureMap; // 0-1
    Texture() = default;
    Texture(const std::string& path, const std::string& file, ColorChannel channel);
private:
    void loadTexture(const std::string& filePath, std::vector<std::vector<double>>& textureMap, ColorChannel channel);
};

#endif // TEXTURE_H
