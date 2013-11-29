#pragma once

#include "NotTooPublic.h"

class Secret : public NotTooPublic{
    public:
        void setup();
        void update();
        void draw();
    private:
        const string myFontName = "TrajanPro-Regular.ttf";
        int numWordsPlaced;
        vector<string> currentMessageTokens;
        int currentImportantWordIndex;
        void handleNewMessage();
};
