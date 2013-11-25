#include "Secret.h"

void Secret::setup(){
    NotTooPublic::setup();
    myFont.loadFont("TrajanPro-Regular.ttf",100,true,true,true);
    nowMillis = ofGetElapsedTimeMillis();
    lastStateChangeMillis = nowMillis;
    startMillis = nowMillis;
    numWordsPlaced = 0;
    currentFadeValue = -255;
    currentImportantWordIndex = -1;

    fboTitle.allocate(ofGetWidth(), ofGetHeight()*0.15);
    fboCanvas.allocate(ofGetWidth(), ofGetHeight()-fboTitle.getHeight());

    fboTitle.begin();
    ofEnableSmoothing();
    ofEnableAlphaBlending();
    ofBackground(0,0);
    ofSetColor(255,255);
    string greetings[] = {"Oakland: send us your secrets", "#NotTooPublic"};
    float scale = (2.0f/3.0f*fboTitle.getHeight()-10)/myFont.stringHeight(greetings[0]);
    ofPushMatrix();
    ofTranslate(10,myFont.stringHeight(greetings[0])*scale);
    ofScale(scale, scale);
    myFont.drawString(greetings[0], 0,0);
    ofPopMatrix();

    ofPushMatrix();
    ofTranslate(10,myFont.stringHeight(greetings[0])*scale+myFont.stringHeight(greetings[1])*scale*0.5+10);
    ofScale(scale*0.5, scale*0.5);
    myFont.drawString(greetings[1], 0,0);
    ofPopMatrix();

    ofNoFill();
    ofSetColor(255,255);
    ofRect(1,1,fboTitle.getWidth()-2,fboTitle.getHeight()-2);
    ofDisableAlphaBlending();
    fboTitle.end();

    fboCanvas.begin();
    ofEnableSmoothing();
    fboCanvas.end();

    myMessages.push_back(pair<string,string>("Do not take cakes", "NN VBN VB NNP"));
}

void Secret::update(){
    NotTooPublic::update();
    nowMillis = ofGetElapsedTimeMillis();

    if(currentState == STATE_INTRO){
        if(currentFadeValue >= 255){
            currentState = STATE_BLANK;
            lastStateChangeMillis = nowMillis;
            currentFadeValue = 255;
        }
        // fading out...
        else if((currentFadeValue >= 0) && (nowMillis - lastStateChangeMillis > 2000)){
            currentFadeValue = min(currentFadeValue+FADE_DELTA, 255);
        }
        // fading in...
        else if(currentFadeValue < 0){
            lastStateChangeMillis = nowMillis;
            currentFadeValue = min(currentFadeValue+FADE_DELTA, 0);
        }
    }
    else if(currentState == STATE_BLANK){
        if(currentFadeValue > 0){
            currentFadeValue = 0;
            fboCanvas.begin();
            ofEnableAlphaBlending();
            ofBackground(0);
            ofDisableAlphaBlending();
            fboCanvas.end();
        }
        else if((nowMillis - lastStateChangeMillis > 2000) && (nowMillis - startMillis > 240000)){
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
        if(currentFadeValue >= 255){
            lastStateChangeMillis = nowMillis;
            currentFadeValue = 255;
        }
        // fading out...
        else if((currentFadeValue >= 0) && (nowMillis - lastStateChangeMillis > 5000)){
            currentFadeValue = min(currentFadeValue+FADE_DELTA, 255);
        }
        // fading in...
        else if(currentFadeValue < 0){
            lastStateChangeMillis = nowMillis;
            currentFadeValue = min(currentFadeValue+FADE_DELTA, 0);
        }
    }

    ///// actual drawings
    if(currentState == STATE_INTRO){
        fboCanvas.begin();
        ofEnableAlphaBlending();
        ofBackground(0);
        ofSetColor(255);
        ofPushMatrix();
        string foo = "GREETINGS\nOAKLAND!\nSECRETS?";
        ofTranslate((fboCanvas.getWidth()-myFont.stringWidth(foo))/2, (fboCanvas.getHeight()-myFont.stringHeight(foo))/2);
        myFont.drawString(foo, 0, 0);
        ofPopMatrix();
        ofDisableAlphaBlending();
        fboCanvas.end();
    }

    if(currentState == STATE_OUTRO){
        fboCanvas.begin();
        ofEnableAlphaBlending();
        ofBackground(0);
        ofSetColor(255);
        ofPushMatrix();
        string foo = "Not Too Public\nKo Nakatsu\nThiago Hersan";
        ofTranslate((fboCanvas.getWidth()-myFont.stringWidth(foo))/2, (fboCanvas.getHeight()-myFont.stringHeight(foo))/2);
        myFont.drawString(foo, 0, 0);
        ofPopMatrix();
        ofDisableAlphaBlending();
        fboCanvas.end();
    }

    if(currentState == STATE_MESSAGE){
        fboCanvas.begin();
        ofEnableAlphaBlending();
        for(int i=0; i<currentMessage.size(); i++){
            ofPushMatrix();
            float xoff = max(myFont.stringWidth("I"),
                             min(abs((ofGetWidth()-myFont.stringWidth(currentMessage.at(i)))/2), myFont.stringWidth(currentMessage.at(i))/2));
            ofTranslate((ofGetWidth()-myFont.stringWidth(currentMessage.at(i)))/2,
                        (i+1)*myFont.getLineHeight());
            if(ofRandom(1) < 0.5){
                // place legible words
                if((numWordsPlaced > 450) && (numWordsPlaced < 500) && (currentImportantWordIndex != i)){
                    ofSetColor(255, 8);
                    myFont.drawString(currentMessage.at(i), 0, 0);
                }
                //else {
                    ofSetColor(255, 4);
                    myFont.drawString(currentMessage.at(i), ofRandom(-xoff, xoff), 0);
                //}
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
    ofBackground(0);
    ofSetColor(255,255);
    if((currentState != STATE_INTRO) && (currentState != STATE_OUTRO)){
        fboTitle.draw(0,0);
    }
    ofSetColor(255-abs(currentFadeValue));
    fboCanvas.draw(0, fboTitle.getHeight());
}

