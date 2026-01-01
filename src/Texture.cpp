#ifndef ESP_PLATFORM
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#endif

#include "Texture.h"
#include <iostream>

void Texture::loadTexture(const std::string& filePath, std::vector<std::vector<double>>& textureMap, ColorChannel channel) {
#ifndef ESP_PLATFORM
    int width, height, channels;
    unsigned char* data = stbi_load(filePath.c_str(), &width, &height, &channels, 0);
    if (data) {
        textureMap.resize(height, std::vector<double>(width));
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                int index = (y * width + x) * channels;
                unsigned char r = data[index];
                unsigned char g = (channels >= 2) ? data[index + 1] : 0;
                unsigned char b = (channels >= 3) ? data[index + 2] : 0;
                unsigned char a = (channels == 4) ? data[index + 3] : 255;
                
                // Extract the specified channel (only if pixel is not fully transparent)
                double value = 0.0;
                if (a > 0) {
                    switch (channel) {
                        case ColorChannel::RED:
                            value = (r == 255) ? 1.0 : 0.0;
                            break;
                        case ColorChannel::GREEN:
                            value = (g == 255) ? 1.0 : 0.0;
                            break;
                        case ColorChannel::BLUE:
                            value = (b == 255) ? 1.0 : 0.0;
                            break;
                    }
                }
                textureMap[y][x] = value;
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
    (void)channel;
#endif
}

Texture::Texture(const std::string& path, const std::string& file, ColorChannel channel) {
#ifndef ESP_PLATFORM
    std::string normal = path + + "/" +  file + ".png";
    loadTexture(normal, textureMap, channel);
#else
    (void)path;
    (void)file;
    (void)channel;
#endif
}