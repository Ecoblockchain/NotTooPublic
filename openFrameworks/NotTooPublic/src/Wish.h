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
    enum State {STATE_INTRO, STATE_BLANK, STATE_SPERM, STATE_PAUSE, STATE_LINES, STATE_CLEAR, STATE_OUTRO};
    State currentState;
    string currentMessage;
    vector<ofPath> currentMessagePath;
    float currentDistance, originalDistance;
};
