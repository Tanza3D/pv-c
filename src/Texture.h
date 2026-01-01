#ifndef TEXTURE_H
#define TEXTURE_H

#include <vector>
#include <string>

enum class ColorChannel {
    RED,    // Extract red channel (r == 255) with alpha
    GREEN,  // Extract green channel (g == 255) with alpha
    BLUE,   // Extract blue channel (b == 255) with alpha
    ALPHA   // Extract alpha channel only (for Face textures)
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
