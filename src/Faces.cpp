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
