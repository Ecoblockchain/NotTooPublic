#pragma once

#include "NotTooPublic.h"

class Wish : public NotTooPublic{
public:
    void setup();
    void update();
    void draw();
private:
    const int FADE_DELTA = 5;
    const string myFontName = "cooperBlack.ttf";
    string currentMessage;
    vector<ofPath> currentMessagePath;
    float currentDistance, originalDistance;
};
