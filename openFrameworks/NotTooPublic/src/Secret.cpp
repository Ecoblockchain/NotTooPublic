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
}

void Secret::handleNewMessage(){
    // tokenize strings
    currentMessageTokens.clear();
    istringstream iss(currentMessage);
    copy(istream_iterator<string>(iss), istream_iterator<string>(), back_inserter<vector<string> >(currentMessageTokens));
    iss.clear();
    vector<string> currentMessagePosTags;
    iss.str(currentPosTags);
    copy(istream_iterator<string>(iss), istream_iterator<string>(), back_inserter<vector<string> >(currentMessagePosTags));

    // figure out most important word
    // largest noun or verb if posTags are good
    // largest word otherwise
    currentImportantWordIndex = -1;
    int largestImportantWordSize = 0;
    for(int i=0; i<currentMessageTokens.size(); i++){
        if(((currentMessageTokens.size() != currentMessagePosTags.size()) ||
            (currentMessagePosTags.at(i).compare(0,2,"NN") == 0) ||
            (currentMessagePosTags.at(i).compare(0,2,"VB") == 0)) &&
           (currentMessageTokens.at(i).size() > largestImportantWordSize)){
            largestImportantWordSize = currentMessageTokens.at(i).size();
            currentImportantWordIndex = i;
        }
    }

    // resize font
    float newFontSize = (float)(myFont.getSize())*fboCanvas.getHeight()/myFont.getLineHeight()/currentMessageTokens.size();
    myFont.loadFont(myFontName,(int)newFontSize,true,true,true);

    currentState = STATE_MESSAGE;
    lastStateChangeMillis = nowMillis;
}

void Secret::update(){
    NotTooPublic::update();

    if(currentState == STATE_INTRO){
        stateLogicIntro();
    }
    else if(currentState == STATE_BLANK){
        stateLogicBlank();
        numWordsPlaced = 0;
    }
    else if(currentState == STATE_MESSAGE) {
        if(numWordsPlaced > 800){
            currentFadeValue = min(currentFadeValue+FADE_DELTA/2, 255);
        }
        if(currentFadeValue >= 255){
            fboCanvas.begin();
            ofEnableAlphaBlending();
            ofBackground(0);
            ofDisableAlphaBlending();
            fboCanvas.end();
            currentFadeValue = 0;
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
        for(int i=0; i<currentMessageTokens.size(); i++){
            ofPushMatrix();
            float xoff = max(myFont.stringWidth("I"),
                             min(abs((ofGetWidth()-myFont.stringWidth(currentMessageTokens.at(i)))/2),
                                 myFont.stringWidth(currentMessageTokens.at(i))/2));
            ofTranslate((ofGetWidth()-myFont.stringWidth(currentMessageTokens.at(i)))/2,
                        i*myFont.getLineHeight()+myFont.stringHeight("P"));
            if(ofRandom(1) < 0.75){
                ofSetColor(255, 2);
                myFont.drawString(currentMessageTokens.at(i), (int)ofRandom(-xoff, xoff), 0);
            }
            // place some legible words
            if((numWordsPlaced > 600) && (numWordsPlaced < 700) && (currentImportantWordIndex != i)){
                ofSetColor(255, 2);
                myFont.drawString(currentMessageTokens.at(i), 0, 0);
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

