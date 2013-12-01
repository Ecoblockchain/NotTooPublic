#include "NotTooPublic.h"

#define OSC_RCVR_IP 8000

//--------------------------------------------------------------
void NotTooPublic::setup(){
	ofSetVerticalSync(true);
	ofEnableSmoothing();
	ofBackground(0);

    myOscReceiver.setup(OSC_RCVR_IP);

    ofxOscSender myOscSender;
    myOscSender.setup("localhost", 8888);
    ofxOscMessage m;
    m.setAddress("/NotTooPublic/subscribe");
    m.addIntArg(OSC_RCVR_IP);
    myOscSender.sendMessage(m);

    creditImage.loadImage("Credits.png");

    fboTitle.allocate(ofGetWidth(), ofGetHeight()*0.15);
    fboCanvas.allocate(ofGetWidth(), ofGetHeight()-fboTitle.getHeight());

    fboCanvas.begin();
    ofEnableSmoothing();
    fboCanvas.end();

    nowMillis = ofGetElapsedTimeMillis();
    lastStateChangeMillis = nowMillis;
    startMillis = nowMillis;
    currentFadeValue = -255;
    currentState = STATE_INTRO;

    // TEST/DEBUG
    newMessages.push_back(pair<string,string>("Not Too Public", "RB RB JJ"));
    //newMessages.push_back(pair<string,string>("I like to bare myself in public sometimes", "RB RB JJ"));
    //newMessages.push_back(pair<string,string>("A fun and safe 2014 for everybody", "RB RB JJ"));
    //newMessages.push_back(pair<string,string>("I can't believe it's not butter", "RB RB JJ"));
}

void NotTooPublic::loadIntroImages(string filenamePrefix){
    ofDirectory dir(ofToDataPath(""));
    dir.listDir();
    for(int i=0; i<dir.size(); i++){
        string fname = dir.getFile(i).getFileName();
        if(fname.size() >= filenamePrefix.size()){
            if(fname.compare(0, filenamePrefix.size(), filenamePrefix) == 0){
                ofImage f;
                f.loadImage(ofToDataPath(fname));
                introImages.push_back(f);
            }
        }
    }
}

void NotTooPublic::loadTitleFbo(string filename){
    fboTitle.begin();
    ofEnableSmoothing();
    ofEnableAlphaBlending();
    ofBackground(255);
    ofSetColor(0,255);
    ofImage tt;
    tt.loadImage(filename);
    ofPushMatrix();
    float scaleVal = min(fboTitle.getWidth()/tt.width, fboTitle.getHeight()/tt.height);
    tt.resize(scaleVal*tt.width, scaleVal*tt.height);
    ofTranslate((fboTitle.getWidth()-tt.width)/2, (fboTitle.getHeight()-tt.height)/2);
    tt.draw(0,0);
    ofPopMatrix();
    ofDisableAlphaBlending();
    fboTitle.end();
}

void NotTooPublic::drawIntro(){
    fboCanvas.begin();
    ofEnableAlphaBlending();
    ofBackground(0);
    ofSetColor(255);
    ofPushMatrix();
    float scaleVal = min(fboCanvas.getWidth()/introImages.front().width, fboCanvas.getHeight()/introImages.front().height);
    introImages.front().resize(scaleVal*introImages.front().width, scaleVal*introImages.front().height);
    ofTranslate((fboCanvas.getWidth()-introImages.front().width)/2, (fboCanvas.getHeight()-introImages.front().height)/2);
    introImages.front().draw(0,0);
    ofPopMatrix();
    ofDisableAlphaBlending();
    fboCanvas.end();
}

void NotTooPublic::drawCredits(){
    fboCanvas.begin();
    ofEnableAlphaBlending();
    ofBackground(0);
    ofSetColor(255);
    ofPushMatrix();
    float scaleVal = min(fboCanvas.getWidth()/creditImage.width, fboCanvas.getHeight()/creditImage.height);
    creditImage.resize(scaleVal*creditImage.width, scaleVal*creditImage.height);
    ofTranslate((fboCanvas.getWidth()-creditImage.width)/2, (fboCanvas.getHeight()-creditImage.height)/2);
    creditImage.draw(0,0);
    ofPopMatrix();
    ofDisableAlphaBlending();
    fboCanvas.end();
}

void NotTooPublic::stateLogicIntro(){
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
    else if((currentFadeValue >= 0) && (nowMillis - lastStateChangeMillis > 30000)){
        currentFadeValue = min(currentFadeValue+FADE_DELTA, 255);
    }
    // fading in...
    else if(currentFadeValue < 0){
        lastStateChangeMillis = nowMillis;
        currentFadeValue = min(currentFadeValue+FADE_DELTA, 0);
    }
}

void NotTooPublic::stateLogicOutro(){
    if(currentFadeValue >= 255){
        lastStateChangeMillis = nowMillis;
        currentFadeValue = 255;
    }
    // fading out...
    else if((currentFadeValue >= 0) && (nowMillis - lastStateChangeMillis > 10000)){
        currentFadeValue = min(currentFadeValue+FADE_DELTA, 255);
    }
    // fading in...
    else if(currentFadeValue < 0){
        lastStateChangeMillis = nowMillis;
        currentFadeValue = min(currentFadeValue+FADE_DELTA, 0);
    }
}

void NotTooPublic::stateLogicBlank(){
    if(currentFadeValue < 0){
        lastStateChangeMillis = nowMillis;
        currentFadeValue = min(currentFadeValue+FADE_DELTA, 0);
    }
    else if(currentFadeValue > 0){
        currentFadeValue = 0;
    }
    else if((nowMillis - lastStateChangeMillis > 1000) && (nowMillis - startMillis > 360000)){
        currentFadeValue = -255;
        currentState = STATE_OUTRO;
        lastStateChangeMillis = nowMillis;
    }
    else if(nowMillis - lastStateChangeMillis > 1000){
        if(newMessages.size() > 0){
            currentMessage = newMessages.front().first;
            currentPosTags = newMessages.front().second;
            oldMessages.push_back(newMessages.front());
            newMessages.pop_front();
        }
        else{
            currentMessage = oldMessages.front().first;
            currentPosTags = oldMessages.front().second;
            oldMessages.push_back(oldMessages.front());
            oldMessages.pop_front();
        }
        handleNewMessage();
    }
}

//--------------------------------------------------------------
void NotTooPublic::update(){
    // get OSC: check for waiting messages
	while(myOscReceiver.hasWaitingMessages()){
		myOscReceiver.getNextMessage(&myOscMessage);
		if(myOscMessage.getAddress().compare("/NotTooPublic/message") == 0){
            newMessages.push_back(pair<string, string>(myOscMessage.getArgAsString(0),myOscMessage.getArgAsString(1)));
            if(newMessages.back().first.compare("") == 0){
                newMessages.pop_back();
            }
		}
    }
    nowMillis = ofGetElapsedTimeMillis();
}

//--------------------------------------------------------------
void NotTooPublic::draw(){
    ofBackground(0);
    ofSetColor(255-abs(currentFadeValue));
    fboCanvas.draw(0, 0);

    ofSetColor((currentState == STATE_BLANK)?255-abs(currentFadeValue):255);
    if((currentState != STATE_INTRO) && (currentState != STATE_OUTRO)){
        fboTitle.draw(0,fboCanvas.getHeight());
    }

    // TEST/DEBUG
    //ofDrawBitmapStringHighlight(ofToString(ofGetFrameRate()), 0,30);
}

//--------------------------------------------------------------
void NotTooPublic::keyPressed(int key){
    if(key == ' '){
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

//--------------------------------------------------------------
void NotTooPublic::keyReleased(int key){
}

//--------------------------------------------------------------
void NotTooPublic::mouseMoved(int x, int y ){
}

//--------------------------------------------------------------
void NotTooPublic::mouseDragged(int x, int y, int button){
}

//--------------------------------------------------------------
void NotTooPublic::mousePressed(int x, int y, int button){
}

//--------------------------------------------------------------
void NotTooPublic::mouseReleased(int x, int y, int button){
}

//--------------------------------------------------------------
void NotTooPublic::windowResized(int w, int h){
}

//--------------------------------------------------------------
void NotTooPublic::gotMessage(ofMessage msg){
}

//--------------------------------------------------------------
void NotTooPublic::dragEvent(ofDragInfo dragInfo){
}
