#pragma once
#include "Texture.h"
#include "gradient.h"
#include <string>

class Face {
public:
    Face();

    Face(const std::string& name, const std::string& path);
    Texture GetTexture() const;

    void SetColourOverride(std::vector<std::vector<TZColor>>);
     
    bool hasOverride = false;
    std::vector<std::vector<TZColor>> override;
private:
    std::string name;
    Texture texture;
};