// Faces.cpp
#include "Faces.h"
#include "Face.h"

void Faces::Init() {
    faces["neutral"] = Face("neutral", "tex/face");
    faces["happy"] = Face("happy", "tex/face");
    faces["sad"] = Face("sad", "tex/face");


    currentFace = "happy";
}

void Faces::Loop() {
    
}

Face Faces::GetCurrent()  {
    return faces.at(currentFace);
}

std::string Faces::GetCurrentName() {
    return currentFace;
}

std::vector<std::string> Faces::getTextureNames() {
    std::vector<std::string> names;
    for (const auto& pair : faces) {
        names.push_back(pair.first);
    }
    return names;
}

bool Faces::SetCurrent(const std::string& name) {
    if (faces.find(name) != faces.end()) {
        currentFace = name;
        return true;
    }
    return false;
}
