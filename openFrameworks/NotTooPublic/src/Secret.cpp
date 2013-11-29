#include "Secret.h"

void Secret::setup(){
    NotTooPublic::setup();
    myFont.loadFont(myFontName,100,true,true,true);
    numWordsPlaced = 0;
    currentImportantWordIndex = -1;
    currentState = STATE_INTRO;

    fboTitle.allocate(ofGetWidth(), ofGetHeight()*0.15);
    fboCanvas.allocate(ofGetWidth(), ofGetHeight()-fboTitle.getHeight());

    loadIntroImages("SecretTitle");
    loadTitleFbo("SecretTweetAt.png");

    // TEST/DEBUG
    myMessages.push_back(pair<string,string>("Do not take cakes", "NN VBN VB NNP"));
    myMessages.push_back(pair<string,string>("Ko has tons of underwear", "NN VBI NN JJ NN"));
    myMessages.push_back(pair<string,string>("I had sex with my boss's wife for money", "II VBP NN CN MM NN NN CC NN"));
}

void Secret::update(){
    NotTooPublic::update();

    if(currentState == STATE_INTRO){
        stateLogicIntro();
    }
    else if(currentState == STATE_BLANK){
        if(currentFadeValue < 0){
            lastStateChangeMillis = nowMillis;
            currentFadeValue = min(currentFadeValue+FADE_DELTA, 0);
        }
        else if(currentFadeValue > 0){
            currentFadeValue = 0;
        }
        else if((nowMillis - lastStateChangeMillis > 1000) && (nowMillis - startMillis > 240000)){
            currentFadeValue = -255;
            currentState = STATE_OUTRO;
            lastStateChangeMillis = nowMillis;
        }
        else if((nowMillis - lastStateChangeMillis > 1000) && (!myMessages.empty())){
            // tokenize strings
            currentMessage.clear();
            istringstream iss(myMessages.front().first);
            copy(istream_iterator<string>(iss), istream_iterator<string>(), back_inserter<vector<string> >(currentMessage));
            iss.clear();
            vector<string> currentMessagePosTags;
            iss.str(myMessages.front().second);
            copy(istream_iterator<string>(iss), istream_iterator<string>(), back_inserter<vector<string> >(currentMessagePosTags));

            // push PDT between messages
            if(myMessages.front().first.compare("Please Don't Tell") != 0){
                myMessages.pop_front();
                myMessages.push_front(pair<string,string>("Please Don't Tell", ""));
            }
            else{
                myMessages.pop_front();
            }

            // figure out most important word (largest noun or verb)
            currentImportantWordIndex = -1;
            int largestImportantWordSize = 0;
            for(int i=0; (currentMessage.size() == currentMessagePosTags.size())&&(i<currentMessage.size()); i++){
                if(((currentMessagePosTags.at(i).compare(0,2,"NN") &&
                    currentMessagePosTags.at(i).compare(0,2,"VB")) == 0) &&
                   (currentMessage.at(i).size() > largestImportantWordSize)){
                    largestImportantWordSize = currentMessage.at(i).size();
                    currentImportantWordIndex = i;
                }
            }

            // resize font
            float newFontSize = (float)(myFont.getSize())*fboCanvas.getHeight()/myFont.getLineHeight()/currentMessage.size();
            myFont.loadFont(myFontName,(int)newFontSize,true,true,true);

            currentState = STATE_MESSAGE;
            lastStateChangeMillis = nowMillis;
        }
    }
    else if(currentState == STATE_MESSAGE) {
        if(numWordsPlaced > 720){
            currentFadeValue = min(currentFadeValue+FADE_DELTA/2, 255);
        }
        if(currentFadeValue >= 255){
            fboCanvas.begin();
            ofEnableAlphaBlending();
            ofBackground(0);
            ofDisableAlphaBlending();
            fboCanvas.end();
            currentFadeValue = 0;
            numWordsPlaced = 0;
            currentState = STATE_BLANK;
            lastStateChangeMillis = nowMillis;
        }
    }
    else if(currentState == STATE_OUTRO){
        stateLogicOutro();
    }

    ///// actual drawings
    if(currentState == STATE_INTRO){
        drawIntro();
    }

    if(currentState == STATE_OUTRO){
        drawCredits();
    }

    if(currentState == STATE_MESSAGE){
        fboCanvas.begin();
        ofEnableAlphaBlending();
        for(int i=0; i<currentMessage.size(); i++){
            ofPushMatrix();
            float xoff = max(myFont.stringWidth("I"),
                             min(abs((ofGetWidth()-myFont.stringWidth(currentMessage.at(i)))/2), myFont.stringWidth(currentMessage.at(i))/2));
            ofTranslate((ofGetWidth()-myFont.stringWidth(currentMessage.at(i)))/2,
                        i*myFont.getLineHeight()+myFont.stringHeight("P"));
            if(ofRandom(1) < 0.5){
                ofSetColor(255, 4);
                myFont.drawString(currentMessage.at(i), ofRandom(-xoff, xoff), 0);

                // place some legible words
                if((numWordsPlaced > 450) && (numWordsPlaced < 500) && (currentImportantWordIndex != i)){
                    ofSetColor(255, 8);
                    myFont.drawString(currentMessage.at(i), 0, 0);
                }
            }
            ofPopMatrix();
        }
        numWordsPlaced++;
        ofDisableAlphaBlending();
        fboCanvas.end();
    }
}

void Secret::draw(){
    NotTooPublic::draw();
}

