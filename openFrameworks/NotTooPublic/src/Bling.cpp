#include "Bling.h"

void Bling::setup(){
    NotTooPublic::setup();
    myFont.loadFont("Garamond-Regular.ttf",100,true,true,true);
    noiseScale = INITIAL_NOISE_SCALE;
    currentMessage = "Not\nToo\nPublic";
    myMessages.push_back(pair<string,string>(currentMessage, currentMessage));
    currentMessage = "Tell me\nsomething\nGoood...";
    myMessages.push_back(pair<string,string>(currentMessage, currentMessage));
    currentMessage = "I am\nfucking\nAwesome!";
    myMessages.push_back(pair<string,string>(currentMessage, currentMessage));
    nowMillis = ofGetElapsedTimeMillis();
    lastStateChangeMillis = nowMillis;
    currentState = STATE_INTRO;
    currentFadeValue = -255;

    fboTitle.allocate(ofGetWidth(), ofGetHeight()*0.15);
    fboCanvas.allocate(ofGetWidth(), ofGetHeight()-fboTitle.getHeight());

    fboTitle.begin();
    ofEnableSmoothing();
    ofEnableAlphaBlending();
    ofBackground(0,0);
    ofSetColor(255,255);
    string greetings[] = {"Oakland: send us your gold", "#NotTooPublic on Twitter"};
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
}

void Bling::update(){
    NotTooPublic::update();
    nowMillis = ofGetElapsedTimeMillis();

    if(currentState == STATE_INTRO){
        if(currentFadeValue >= 255){
            currentState = STATE_BLANK;
            lastStateChangeMillis = nowMillis;
            currentFadeValue = 255;
        }
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
        else if((nowMillis - lastStateChangeMillis > 2000) && (!myMessages.empty())){
            currentMessage = myMessages.front().first;
            myMessages.pop_front();
            currentMessagePath = myFont.getStringAsPoints(currentMessage);
            currentState = STATE_GOLD;
            noiseScale = INITIAL_NOISE_SCALE;
            lastStateChangeMillis = nowMillis;
        }
    }
    else if(currentState == STATE_GOLD){
        if((nowMillis - lastStateChangeMillis > 2000) && (noiseScale < 128)){
            currentState = STATE_PAUSE;
            lastStateChangeMillis = nowMillis;
        }
        else{
            noiseScale *= 0.96;
            blingColor = ofColor(218,188,68);
            blingColor.lerp(ofColor(255,255,255), ofClamp((float)(nowMillis - lastStateChangeMillis)/2000.0f, 0, 1));
        }
    }
    else if(currentState == STATE_PAUSE){
        if(nowMillis - lastStateChangeMillis > 3000){
            currentState = STATE_DIAMOND;
            blingFadeValue = 0;
            noiseScale = 32;
            lastStateChangeMillis = nowMillis;
        }
        else{
            ///
        }
    }
    else if(currentState == STATE_DIAMOND){
        if(blingFadeValue >= 1024){
            currentState = STATE_BLANK;
            lastStateChangeMillis = nowMillis;
        }
        else{
            noiseScale *= 1.01;
            blingColor = ofColor(255,255,255);
            if((nowMillis - lastStateChangeMillis > 3000) && (noiseScale > 10*INITIAL_NOISE_SCALE)){
                blingFadeValue = min(blingFadeValue+FADE_DELTA, 1024);
            }
        }
    }

    if(currentState == STATE_PAUSE){
        fboCanvas.begin();
        ofEnableAlphaBlending();
        ofPushMatrix();
        ofTranslate((fboCanvas.getWidth()-myFont.stringWidth(currentMessage))/2,
                    (fboCanvas.getHeight()-myFont.stringHeight(currentMessage))/2+myFont.stringHeight("Tell"));
        ofSetLineWidth(2);
        ofSetColor(255,(int)ofMap(nowMillis - lastStateChangeMillis, 1500, 3000, 0, 128,true));
        ofNoFill();
        for(int i=0; i<currentMessagePath.size(); i++){
            ofBeginShape();
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

    if(currentState == STATE_INTRO){
        fboCanvas.begin();
        ofEnableAlphaBlending();
        ofBackground(0);
        ofSetColor(255-abs(currentFadeValue));
        ofPushMatrix();
        string foo = "GREETINGS\nOAKLAND!\nSUP?";
        ofTranslate((fboCanvas.getWidth()-myFont.stringWidth(foo))/2, (fboCanvas.getHeight()-myFont.stringHeight(foo))/2);
        myFont.drawString(foo, 0, 0);
        ofPopMatrix();
        ofDisableAlphaBlending();
        fboCanvas.end();
    }

    if((currentState == STATE_DIAMOND) || (currentState == STATE_BLANK)){
        fboCanvas.begin();
        ofEnableAlphaBlending();
        ofSetColor(0,2*FADE_DELTA);
        ofRect(0,0,fboCanvas.getWidth(),fboCanvas.getHeight());
        ofDisableAlphaBlending();
        fboCanvas.end();
    }

    if((currentState == STATE_GOLD) || (currentState == STATE_PAUSE) || (currentState == STATE_DIAMOND)){
        fboCanvas.begin();
        ofEnableAlphaBlending();
        ofPushMatrix();
        ofTranslate((fboCanvas.getWidth()-myFont.stringWidth(currentMessage))/2,
                    (fboCanvas.getHeight()-myFont.stringHeight(currentMessage))/2+myFont.stringHeight("Tell"));
        ofSetLineWidth(2);
        for(int i=0; i<currentMessagePath.size(); i++){
            for(int j=0; j<currentMessagePath.at(i).getOutline().size(); j++){
                for(int k=0; k<currentMessagePath.at(i).getOutline().at(j).size(); k++){
                    ofPoint tp = currentMessagePath.at(i).getOutline().at(j).getVertices().at(k);
                    int complexity = 512;
                    float dx = ofNoise(complexity*tp.x/ofGetWidth()+PI, complexity*tp.y/ofGetHeight()+PI, (float)(ofGetFrameNum())/128.0f+PI);
                    float dy = ofNoise(complexity*tp.x/ofGetWidth()-PI, complexity*tp.y/ofGetHeight()-PI, (float)(ofGetFrameNum())/132.0f-PI);
                    float a = noiseScale/2*ofNoise(dy,dx,(float)(ofGetFrameNum())/100.0f);
                    ofFill();
                    if(!(k%1)){
                        ofSetColor(blingColor,64);
                        ofCircle(tp.x-a+2*a*dy, tp.y-a+2*a*dx, 1);
                    }
                }
            }
        }
        ofPopMatrix();
        ofDisableAlphaBlending();
        fboCanvas.end();
    }
}
void Bling::draw(){
    NotTooPublic::draw();
    ofBackground(0);
    ofSetColor(255,255);
    if((currentState != STATE_INTRO) && (currentState != STATE_OUTRO)){
        fboTitle.draw(0,0);
    }
    fboCanvas.draw(0, fboTitle.getHeight());
}

void Bling::drawOld(){
    noiseScale *= (nowMillis < 2000)?0.96:(nowMillis < 5000)?1.0:1.01;
    ofSetColor(255,255);
    fboTitle.draw(0,0);
    nowMillis = ofGetElapsedTimeMillis();
    ofSetColor(255,128);
    ofDrawBitmapStringHighlight(ofToString(ofGetFrameRate()),0,20);

    ofColor mc = ofColor(218,188,68);
    mc.lerp(ofColor(255,255,255), ofClamp((float)(nowMillis)/3000.0f,0,1));
    vector<ofPath> p = myFont.getStringAsPoints(currentMessage);

    if(nowMillis > 5000){
        ofSetColor(0,10);
        ofRect(0,0,ofGetWidth(),ofGetHeight());
    }
    else if(nowMillis > 4000){
        ofPushMatrix();
        ofTranslate((ofGetWidth()-myFont.stringWidth(currentMessage))/2, (ofGetHeight()-myFont.stringHeight(currentMessage))/2+myFont.stringHeight("Not"));
        ofSetLineWidth(2);
        ofSetColor(mc,ofMap(nowMillis, 4000, 5000, 0, 128,true));
        ofNoFill();
        for(int i=0; i<p.size(); i++){
            ofBeginShape();
            // shapes
            for(int j=0; j<p.at(i).getOutline().size(); j++){
                ofNextContour();
                // points
                for(int k=0; k<p.at(i).getOutline().at(j).size(); k++){
                    ofPoint tp = p.at(i).getOutline().at(j).getVertices().at(k);
                    ofVertex(tp.x, tp.y);
                }
            }
            ofEndShape(true);
        }
        ofPopMatrix();
    }


    ofPushMatrix();
    ofTranslate((ofGetWidth()-myFont.stringWidth(currentMessage))/2, (ofGetHeight()-myFont.stringHeight(currentMessage))/2+myFont.stringHeight("Not"));
    ofSetLineWidth(2);

    // letters
    for(int i=0; i<p.size(); i++){
        // shapes
        for(int j=0; j<p.at(i).getOutline().size(); j++){
            // points
            for(int k=0; k<p.at(i).getOutline().at(j).size(); k++){
                ofPoint tp = p.at(i).getOutline().at(j).getVertices().at(k);
                int complexity = 512;
                float dx = ofNoise(complexity*tp.x/ofGetWidth()+PI, complexity*tp.y/ofGetHeight()+PI, (float)(ofGetFrameNum())/128.0f+PI);
                float dy = ofNoise(complexity*tp.x/ofGetWidth()-PI, complexity*tp.y/ofGetHeight()-PI, (float)(ofGetFrameNum())/132.0f-PI);
                float a = noiseScale/2*ofNoise(dy,dx,(float)(ofGetFrameNum())/100.0f);
                ofFill();
                if(!(k%1)){
                    ofSetColor(mc,64);
                    ofCircle(tp.x-a+2*a*dy, tp.y-a+2*a*dx, 1);
                }
            }
        }
    }
}
