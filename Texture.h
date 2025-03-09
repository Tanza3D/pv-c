#ifndef TEXTURE_H
#define TEXTURE_H

#include <vector>
#include <string>

class Texture {
public:
    std::vector<std::vector<double>> textureMap; // 0-1
    std::vector<std::vector<double>> textureMap_Open; // 0-1
    Texture() = default;
    Texture(const std::string& path, const std::string& file);
private:
    void loadTexture(const std::string& filePath, std::vector<std::vector<double>>& textureMap);
};

#endif // TEXTURE_H
