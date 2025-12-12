#ifndef EYES_H
#define EYES_H

#include <map>
#include <vector>
#include "Texture.h"
#include "Eye.h"

class Eyes  {
private:
    std::string currentEye;
    std::map<std::string, Eye> eyes;

public:
    void Loop();
    void Init();
    Eye GetCurrent();

    std::string GetCurrentName();
    std::vector<std::string> getTextureNames();
    bool SetCurrent(const std::string& name);
};

#endif
