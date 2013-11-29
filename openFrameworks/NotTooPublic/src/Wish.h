#pragma once

#include "NotTooPublic.h"

class Wish : public NotTooPublic{
    public:
        void setup();
        void update();
        void draw();
    private:
        const string myFontName = "cooperBlack.ttf";
        string currentMessage;
        vector<ofPath> currentMessagePath;
        float currentMessageScaling;
        int currentMessagePoints;
        float currentDistance, originalDistance;
        void handleNewMessage();
};
