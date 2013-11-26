#pragma once

#include "NotTooPublic.h"

class Bling : public NotTooPublic{
    public:
        void setup();
        void update();
        void draw();
        void drawOld();

    private:
        const int FADE_DELTA = 5;
        const float INITIAL_NOISE_SCALE = 900;
        const string myFontName = "Garamond-Regular.ttf";
        float noiseScale;
        enum State {STATE_INTRO, STATE_BLANK, STATE_GOLD, STATE_PAUSE, STATE_DIAMOND, STATE_OUTRO};
        State currentState;
        int blingFadeValue;
        ofColor blingColor;
        string currentMessage;
        float currentMessageScaling;
        vector<ofPath> currentMessagePath;
};
