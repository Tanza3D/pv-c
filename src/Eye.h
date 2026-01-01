#pragma once
#include "Texture.h"
#include "gradient.h"
#include <string>

class Eye {
public:
    Eye();

    Eye(const std::string& name, const std::string& path);
    Texture GetEyeTexture() const;
    Texture GetPupilAreaTexture() const;
    Texture GetPupilShapeTexture() const;

    void SetColourOverride(std::vector<std::vector<TZColor>>);
    void SetPupilColourOverride(std::vector<std::vector<TZColor>>);

    bool hasOverride = false;
    bool hasPupilOverride = false;

    std::vector<std::vector<TZColor>> override;
    std::vector<std::vector<TZColor>> pupilOverride;
private:
    std::string name;
    Texture eyeTexture;        // Blue channel - the eye shape
    Texture pupilAreaTexture;  // Red channel - where the pupil can move
    Texture pupilShapeTexture; // Green channel - the pupil shape itself
};