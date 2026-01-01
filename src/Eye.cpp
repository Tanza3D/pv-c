#include "Eye.h"
#include "gradient.h"


Eye::Eye()
        : name(""), 
          eyeTexture("", "", ColorChannel::BLUE),
          pupilAreaTexture("", "", ColorChannel::RED),
          pupilShapeTexture("", "", ColorChannel::GREEN) {}

Eye::Eye(const std::string& name, const std::string& path)
        : name(name), 
          eyeTexture(path, name, ColorChannel::BLUE),
          pupilAreaTexture(path, name, ColorChannel::RED),
          pupilShapeTexture(path, name, ColorChannel::GREEN) {}

Texture Eye::GetEyeTexture() const {
    return eyeTexture;
}

Texture Eye::GetPupilAreaTexture() const {
    return pupilAreaTexture;
}

Texture Eye::GetPupilShapeTexture() const {
    return pupilShapeTexture;
}

void Eye::SetColourOverride(std::vector<std::vector<TZColor>> gradient) {
#ifndef ESP_PLATFORM
    hasOverride = true;
    override = gradient;
#endif
}

void Eye::SetPupilColourOverride(std::vector<std::vector<TZColor>> gradient) {
#ifndef ESP_PLATFORM
    hasPupilOverride = true;
    pupilOverride = gradient;
#endif
}
