#pragma once

#include "NotTooPublic.h"

class Bling : public NotTooPublic{
    public:
        void setup();
        void update();
        void draw();
        void drawOld();

    private:
        const float INITIAL_NOISE_SCALE = 900;
        const string myFontName = "Garamond-Regular.ttf";
        float noiseScale;
        int blingFadeValue;
        ofColor blingColor;
        float currentMessageScaling;
        vector<ofPath> currentMessagePath;
        void handleNewMessage();
};
