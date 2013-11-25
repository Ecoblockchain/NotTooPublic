#pragma once

#include "NotTooPublic.h"

class Secret : public NotTooPublic{
    public:
        void setup();
        void update();
        void draw();
    private:
        const int FADE_DELTA = 5;
        enum State {STATE_INTRO, STATE_BLANK, STATE_MESSAGE, STATE_OUTRO};
        State currentState;
        int numWordsPlaced;
        vector<string> currentMessage;
        int currentImportantWordIndex;
};
