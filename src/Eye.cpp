#include "Eye.h"
#include "gradient.h"


Eye::Eye()
        : name(""), texture("", ""), pupilAreaTexture("", "_pe") {}
Eye::Eye(const std::string& name, const std::string& path)
        : name(name), texture(path, name), pupilAreaTexture(path, name + "_pe") {}

Texture Eye::GetTexture() const {
    Texture t = texture;
    return t;
}

void Eye::SetColourOverride(std::vector<std::vector<TZColor>> gradient) {
    hasOverride = true;
    override = gradient;
}

void Eye::SetPupilColourOverride(std::vector<std::vector<TZColor>> gradient) {
    hasPupilOverride = true;
    pupilOverride = gradient;
}



Texture Eye::GetPupilAreaTexture() const {
    Texture t = pupilAreaTexture;
    return t;
}
