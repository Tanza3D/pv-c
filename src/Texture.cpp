#ifndef ESP_PLATFORM
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#endif

#include "Texture.h"
#include <iostream>

void Texture::loadTexture(const std::string& filePath, std::vector<std::vector<double>>& textureMap) {
#ifndef ESP_PLATFORM
    int width, height, channels;
    unsigned char* data = stbi_load(filePath.c_str(), &width, &height, &channels, 0);
    if (data) {
        textureMap.resize(height, std::vector<double>(width));
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                int index = (y * width + x) * channels;
                unsigned char r = data[index];
                unsigned char g = data[index + 1];
                unsigned char b = data[index + 2];
                unsigned char a = (channels == 4) ? data[index + 3] : 255;
                textureMap[y][x] = static_cast<double>(a) / 255.0; // Store alpha as a double in range [0, 1]
            }
        }
        stbi_image_free(data);
    } else {
        std::cerr << "Failed to load texture from: " << filePath << std::endl;
    }
#else
    // On ESP32, we don't load textures - just need the names for the menu
    std::cerr << "Skipping texture load from: " << filePath << " because we're on ESP platform" << std::endl;
    (void)filePath;
    (void)textureMap;
#endif
}

Texture::Texture(const std::string& path, const std::string& file) {
#ifndef ESP_PLATFORM
    std::string normal = path + + "/" +  file + ".png";
    loadTexture(normal, textureMap);
#else
    (void)path;
    (void)file;
#endif
}