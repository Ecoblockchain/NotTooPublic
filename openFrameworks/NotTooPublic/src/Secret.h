#pragma once

#include "NotTooPublic.h"

class Secret : public NotTooPublic{
    public:
        void setup();
        void update();
        void draw();
    private:
        const int FADE_DELTA = 5;
        ofTrueTypeFont myFont;
        long long int nowMillis, lastStateChangeMillis, startMillis;
        enum State {STATE_INTRO, STATE_BLANK, STATE_MESSAGE, STATE_OUTRO};
        State currentState;
        int currentFadeValue;
        int numWordsPlaced;
        ofFbo fboTitle, fboCanvas;
        vector<string> currentMessage;
        int currentImportantWordIndex;
        deque<ofImage> introImages;
};
