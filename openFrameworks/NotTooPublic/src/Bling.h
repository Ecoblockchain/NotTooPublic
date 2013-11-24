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
        float noiseScale;
        long long int nowMillis, lastStateChangeMillis, startMillis;
        enum State {STATE_INTRO, STATE_BLANK, STATE_GOLD, STATE_PAUSE, STATE_DIAMOND, STATE_FADEOUT, STATE_OUTRO};
        State currentState;
        int currentFadeValue, blingFadeValue;
        ofColor blingColor;
        ofFbo fboTitle, fboCanvas;
        vector<ofPath> currentMessagePath;
};
