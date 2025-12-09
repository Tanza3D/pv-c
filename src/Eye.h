#pragma once
#include "Texture.h"
#include "gradient.h"
#include <string>

class Eye {
public:
    Eye();

    Eye(const std::string& name, const std::string& path);
    Texture GetTexture() const;
    Texture GetPupilAreaTexture() const;

    void SetColourOverride(std::vector<std::vector<TZColor>>);
    void SetPupilColourOverride(std::vector<std::vector<TZColor>>);

    bool hasOverride = false;
    bool hasPupilOverride = false;

    std::vector<std::vector<TZColor>> override;
    std::vector<std::vector<TZColor>> pupilOverride;
private:
    std::string name;
    Texture texture;
    Texture pupilAreaTexture;
};