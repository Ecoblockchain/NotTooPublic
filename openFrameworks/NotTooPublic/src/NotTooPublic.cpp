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

    nowMillis = ofGetElapsedTimeMillis();
    lastStateChangeMillis = nowMillis;
    startMillis = nowMillis;
    currentFadeValue = -255;

    fboCanvas.begin();
    ofEnableSmoothing();
    fboCanvas.end();
}

//--------------------------------------------------------------
void NotTooPublic::update(){
    // get OSC: check for waiting messages
	while(myOscReceiver.hasWaitingMessages()){
		myOscReceiver.getNextMessage(&myOscMessage);
		if(myOscMessage.getAddress().compare("/NotTooPublic/message") == 0){
            myMessages.push_back(pair<string, string>(myOscMessage.getArgAsString(0),myOscMessage.getArgAsString(1)));
		}
    }
    nowMillis = ofGetElapsedTimeMillis();
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

void NotTooPublic::loadTitleFbo(string filenamePrefix){

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


//--------------------------------------------------------------
void NotTooPublic::draw(){
}

//--------------------------------------------------------------
void NotTooPublic::keyPressed(int key){
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
