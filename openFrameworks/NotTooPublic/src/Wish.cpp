#include "Wish.h"

void Wish::setup(){
    NotTooPublic::setup();
    myFont.loadFont(myFontName,100,true,true,true);
    currentState = STATE_INTRO;

    fboTitle.allocate(ofGetWidth(), ofGetHeight()*0.15);
    fboCanvas.allocate(ofGetWidth(), ofGetHeight()-fboTitle.getHeight());

    originalDistance = max(fboCanvas.getWidth(), fboCanvas.getHeight());
    currentDistance = originalDistance;

    loadIntroImages("WishTitle");
    loadTitleFbo("WishTweetAt.png");

    // TEST/DEBUG
    myMessages.push_back(pair<string,string>("Do not\ntake cakes", "NN VBN VB NNP"));
    myMessages.push_back(pair<string,string>("Ko has\ntons of\nunderwear", "NN VBI NN JJ NN"));
    myMessages.push_back(pair<string,string>("I had sex\nwith my boss's\nwife for money", "II VBP NN CN MM NN NN CC NN"));
}

void Wish::update(){
    NotTooPublic::update();

    if(currentState == STATE_INTRO){
        if(currentFadeValue >= 255){
            introImages.pop_front();
            currentFadeValue = -255;
            lastStateChangeMillis = nowMillis;
            if(introImages.size() > 0){
                currentState = STATE_INTRO;
            }
            else{
                fboCanvas.begin();
                ofEnableAlphaBlending();
                ofEnableSmoothing();
                ofBackground(0);
                ofDisableAlphaBlending();
                fboCanvas.end();
                currentState = STATE_BLANK;
            }
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
            currentMessage = myMessages.front().first;
            myMessages.pop_front();

            // TODO: resize font
            //float newFontSize = (float)(myFont.getSize())*fboCanvas.getHeight()/myFont.getLineHeight()/currentMessage.size();
            //myFont.loadFont(myFontName,(int)newFontSize,true,true,true);

            currentMessagePath = myFont.getStringAsPoints(currentMessage);
            currentDistance = originalDistance;
            currentState = STATE_SPERM;
            lastStateChangeMillis = nowMillis;
        }
    }
    else if(currentState == STATE_SPERM) {
        if(currentDistance <= 0.0){
            currentDistance = 0;
            lastStateChangeMillis = nowMillis;
            currentState = STATE_PAUSE;
        }
        else{
            currentDistance -= 1.0;
        }
    }
    else if(currentState == STATE_PAUSE) {
        if(nowMillis - lastStateChangeMillis > 2000){
            currentDistance = 0;
            lastStateChangeMillis = nowMillis;
            currentState = STATE_LINES;
        }
        else{
            //
        }
    }
    else if(currentState == STATE_LINES) {
        if(currentDistance > originalDistance){
            currentDistance = 0;
            lastStateChangeMillis = nowMillis;
            currentState = STATE_CLEAR;
        }
        else{
            currentDistance += 4.0f;
        }
    }
    else if(currentState == STATE_CLEAR) {
        if(currentDistance > originalDistance){
            currentDistance = originalDistance;
            lastStateChangeMillis = nowMillis;
            currentState = STATE_BLANK;
        }
        else{
            currentDistance += 4.0f;
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
        drawIntro();
    }

    if(currentState == STATE_OUTRO){
        drawCredits();
    }

    if(currentState == STATE_SPERM){
        fboCanvas.begin();
        ofEnableAlphaBlending();
        ofFill();
        ofSetColor(0, 32);
        ofRect(0,0, fboCanvas.getWidth(), fboCanvas.getHeight());
        ofPushMatrix();
        ofTranslate((fboCanvas.getWidth()-myFont.stringWidth(currentMessage))/2,
                    (fboCanvas.getHeight()-myFont.stringHeight(currentMessage))/2+myFont.stringHeight("Tell"));

        ofNoFill();
        for(int i=0; i<currentMessagePath.size(); i++){
            currentMessagePath.at(i).simplify();
            ofVec3f center = currentMessagePath.at(i).getTessellation().getCentroid();
            for(int j=0; j<currentMessagePath.at(i).getOutline().size(); j++){
                for(int k=0; k<currentMessagePath.at(i).getOutline().at(j).size(); k++){
                    ofPoint tp = currentMessagePath.at(i).getOutline().at(j).getVertices().at(k);
                    ofVec3f direction = (tp-center).normalize();
                    tp += direction*max(0.0f,currentDistance);
                    if(!(k%1)){
                        ofSetColor(255);
                        int complexity = 16;
                        float magnitude = 8.0f;
                        float dx = ofNoise(complexity*tp.x/fboCanvas.getWidth()+PI,
                                           complexity*tp.y/fboCanvas.getHeight()+PI,
                                           (float)(ofGetFrameNum())/120.0f+PI);
                        float dy = ofNoise(complexity*tp.x/fboCanvas.getWidth()-PI,
                                           complexity*tp.y/fboCanvas.getHeight()-PI,
                                           (float)(ofGetFrameNum())/130.0f-PI);
                        float a = magnitude*ofNoise(dy,dx,(float)(ofGetFrameNum())/100.0f);
                        ofCircle(tp.x-a+2*a*dx, tp.y-a+2*a*dy, 0.2);
                    }
                }
            }
        }
        ofPopMatrix();
        ofDisableAlphaBlending();
        fboCanvas.end();
    }

    if(currentState == STATE_PAUSE){
        fboCanvas.begin();
        ofEnableAlphaBlending();
        ofFill();
        ofSetColor(0, 8);
        //ofRect(0,0, fboCanvas.getWidth(), fboCanvas.getHeight());
        ofPushMatrix();
        ofTranslate((fboCanvas.getWidth()-myFont.stringWidth(currentMessage))/2,
                    (fboCanvas.getHeight()-myFont.stringHeight(currentMessage))/2+myFont.stringHeight("Tell"));

        ofNoFill();
        ofSetColor(255,8);
        ofSetLineWidth(2);
        for(int i=0; i<currentMessagePath.size(); i++){
            //currentMessagePath.at(i).simplify();
            ofBeginShape();
            ofVec3f center = currentMessagePath.at(i).getTessellation().getCentroid();
            for(int j=0; j<currentMessagePath.at(i).getOutline().size(); j++){
                ofNextContour();
                for(int k=0; k<currentMessagePath.at(i).getOutline().at(j).size(); k++){
                    ofPoint tp = currentMessagePath.at(i).getOutline().at(j).getVertices().at(k);
                    ofVertex(tp.x, tp.y);
                }
            }
            ofEndShape(true);
        }
        ofPopMatrix();
        ofDisableAlphaBlending();
        fboCanvas.end();
    }

    if(currentState == STATE_LINES){
        fboCanvas.begin();
        ofEnableAlphaBlending();
        ofFill();
        ofSetColor(0, 32);
        ofRect(0,0, fboCanvas.getWidth(), fboCanvas.getHeight());
        ofPushMatrix();
        ofTranslate((fboCanvas.getWidth()-myFont.stringWidth(currentMessage))/2,
                    (fboCanvas.getHeight()-myFont.stringHeight(currentMessage))/2+myFont.stringHeight("Tell"));

        ofNoFill();
        ofSetLineWidth(1.5);
        for(int i=0; i<currentMessagePath.size(); i++){
            currentMessagePath.at(i).simplify();
            ofVec3f center = currentMessagePath.at(i).getTessellation().getCentroid();
            for(int j=0; j<currentMessagePath.at(i).getOutline().size(); j++){
                for(int k=0; k<currentMessagePath.at(i).getOutline().at(j).size(); k++){
                    ofPoint tp = currentMessagePath.at(i).getOutline().at(j).getVertices().at(k);
                    ofVec3f direction = (tp-center).normalize();
                    tp += direction*max(0.0f,currentDistance);
                    if(!(k%1)){
                        ofSetColor(255,128);
                        ofLine(center, tp);
                    }
                }
            }
        }
        ofSetColor(0);
        myFont.drawString(currentMessage, 0, 0);
        ofPopMatrix();
        ofDisableAlphaBlending();
        fboCanvas.end();
    }

    if(currentState == STATE_CLEAR){
        fboCanvas.begin();
        ofEnableAlphaBlending();
        ofFill();
        ofSetColor(0, 32);
        ofRect(0,0, fboCanvas.getWidth(), fboCanvas.getHeight());
        ofPushMatrix();
        ofTranslate((fboCanvas.getWidth()-myFont.stringWidth(currentMessage))/2,
                    (fboCanvas.getHeight()-myFont.stringHeight(currentMessage))/2+myFont.stringHeight("Tell"));

        ofNoFill();
        ofSetLineWidth(1.5);
        for(int i=0; i<currentMessagePath.size(); i++){
            currentMessagePath.at(i).simplify();
            ofVec3f center = currentMessagePath.at(i).getTessellation().getCentroid();
            for(int j=0; j<currentMessagePath.at(i).getOutline().size(); j++){
                for(int k=0; k<currentMessagePath.at(i).getOutline().at(j).size(); k++){
                    ofPoint tp = currentMessagePath.at(i).getOutline().at(j).getVertices().at(k);
                    ofVec3f direction = (tp-center).normalize();
                    tp += direction*max(0.0f,currentDistance);
                    ofPoint maxDistance = tp+direction*originalDistance;
                    if(!(k%1)){
                        ofSetColor(255,128);
                        ofLine(maxDistance, tp);
                    }
                }
            }
        }
        ofSetColor(0);
        myFont.drawString(currentMessage, 0, 0);
        ofPopMatrix();
        ofDisableAlphaBlending();
        fboCanvas.end();
    }
}

void Wish::draw(){
    NotTooPublic::draw();
    ofBackground(0);
    ofSetColor((currentState == STATE_BLANK)?255-abs(currentFadeValue):255);
    if((currentState != STATE_INTRO) && (currentState != STATE_OUTRO)){
        fboTitle.draw(0,0);
    }
    ofSetColor(255-abs(currentFadeValue));
    fboCanvas.draw(0, fboTitle.getHeight());
}

