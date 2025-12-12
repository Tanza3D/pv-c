#ifndef FACE_H
#define FACE_H

#include <map>
#include <vector>
#include "Texture.h"
#include "Face.h"

class Faces {
private:
    std::string currentFace;
    std::map<std::string, Face> faces;

public:
    void Loop();
    void Init();
    Face GetCurrent();

    std::string GetCurrentName();
    std::vector<std::string> getTextureNames();
    bool SetCurrent(const std::string& name);
};

#endif
