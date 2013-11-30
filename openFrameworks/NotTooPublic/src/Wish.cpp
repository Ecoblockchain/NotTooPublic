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
}

void Wish::handleNewMessage(){
    // resize font and format string
    float fontScale = 0.5*sqrt((fboCanvas.getHeight()*fboCanvas.getWidth())/(myFont.getLineHeight()*myFont.stringWidth(currentMessage)));
    float newFontSize = (float)(myFont.getSize())*fontScale;
    myFont.loadFont(myFontName,(int)newFontSize,true,true,true);
    int numberOfLines = (int)(ceil(0.5*fboCanvas.getHeight()/myFont.getLineHeight()));

    for(int i=1; i<numberOfLines; i++){
        int sp = currentMessage.find(" ",(i*currentMessage.size()/numberOfLines));
        if(sp != string::npos){
            currentMessage.replace(sp,1,"\n");
        }
    }

    // center align
    int startPos = 0;
    while(startPos < (currentMessage.size()-1)){
        int endPos = currentMessage.find_first_of("\n", startPos);
        endPos = (endPos == string::npos)?currentMessage.size():endPos;
        currentMessage.insert(startPos,
                              (int)max(0.0f,0.5f*(myFont.stringWidth(currentMessage)-myFont.stringWidth(currentMessage.substr(startPos, endPos-startPos)))/myFont.stringWidth("p")),
                              ' ');
        startPos = currentMessage.find_first_of("\n", startPos);
        startPos = (startPos == string::npos)?currentMessage.size():startPos+1;
    }

    currentMessageScaling = 0.8*min(fboCanvas.getWidth()/myFont.stringWidth(currentMessage),
                                    fboCanvas.getHeight()/myFont.stringHeight(currentMessage));

    currentMessagePath = myFont.getStringAsPoints(currentMessage);
    currentDistance = 0.9*originalDistance/currentMessageScaling;

    currentMessagePoints = 0;
    for(int i=0; i<currentMessagePath.size(); i++){
        currentMessagePath.at(i).simplify();
        for(int j=0; j<currentMessagePath.at(i).getOutline().size(); j++){
            for(int k=0; k<currentMessagePath.at(i).getOutline().at(j).size(); k++){
                currentMessagePoints++;
            }
        }
    }
    currentState = STATE_SPERM;
    lastStateChangeMillis = nowMillis;
}

void Wish::update(){
    NotTooPublic::update();

    if(currentState == STATE_INTRO){
        stateLogicIntro();
    }
    else if(currentState == STATE_BLANK){
        stateLogicBlank();
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
        stateLogicOutro();
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
        // SPERM: ofSetColor(0, 16);
        ofSetColor(0, 64);
        ofRect(0,0, fboCanvas.getWidth(), fboCanvas.getHeight());
        ofPushMatrix();
        ofScale(currentMessageScaling, currentMessageScaling);
        ofTranslate((fboCanvas.getWidth()/currentMessageScaling-myFont.stringWidth(currentMessage))/2,
                    (fboCanvas.getHeight()/currentMessageScaling-myFont.stringHeight(currentMessage))/2+myFont.stringHeight("Tell"));

        ofFill();
        for(int i=0; i<currentMessagePath.size(); i++){
            ofVec3f center = currentMessagePath.at(i).getTessellation().getCentroid();
            for(int j=0; j<currentMessagePath.at(i).getOutline().size(); j++){
                for(int k=0; k<currentMessagePath.at(i).getOutline().at(j).size(); k++){
                    ofPoint tp = currentMessagePath.at(i).getOutline().at(j).getVertices().at(k);
                    ofVec3f direction = (tp-center).normalize();
                    tp += direction*max(0.0f,currentDistance);
                    if(!(k%(int)ceil(currentMessagePoints/1000.0f))){
                        // SPERM: int complexity = 16;
                        int complexity = 4;
                        float magnitude = 8.0f;
                        float dx = ofNoise(complexity*tp.x/fboCanvas.getWidth()+PI,
                                           complexity*tp.y/fboCanvas.getHeight()+PI,
                                           (float)(ofGetFrameNum())/120.0f+PI);
                        float dy = ofNoise(complexity*tp.x/fboCanvas.getWidth()-PI,
                                           complexity*tp.y/fboCanvas.getHeight()-PI,
                                           (float)(ofGetFrameNum())/130.0f-PI);
                        float a = magnitude*ofNoise(dy,dx,(float)(ofGetFrameNum())/100.0f);
                        ofSetColor(255);
                        ofCircle(tp.x-a+2*a*dx, tp.y-a+2*a*dy, 1.0);
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
        ofPushMatrix();
        ofScale(currentMessageScaling, currentMessageScaling);
        ofTranslate((fboCanvas.getWidth()/currentMessageScaling-myFont.stringWidth(currentMessage))/2,
                    (fboCanvas.getHeight()/currentMessageScaling-myFont.stringHeight(currentMessage))/2+myFont.stringHeight("Tell"));

        ofFill();
        ofSetColor(0,8);
        myFont.drawString(currentMessage, 0, 0);

        ofNoFill();
        ofSetColor(255,8);
        ofSetLineWidth(2);
        for(int i=0; i<currentMessagePath.size(); i++){
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
        ofScale(currentMessageScaling, currentMessageScaling);
        ofTranslate((fboCanvas.getWidth()/currentMessageScaling-myFont.stringWidth(currentMessage))/2,
                    (fboCanvas.getHeight()/currentMessageScaling-myFont.stringHeight(currentMessage))/2+myFont.stringHeight("Tell"));

        ofNoFill();
        ofSetLineWidth(1.5);
        for(int i=0; i<currentMessagePath.size(); i++){
            ofVec3f center = currentMessagePath.at(i).getTessellation().getCentroid();
            for(int j=0; j<currentMessagePath.at(i).getOutline().size(); j++){
                for(int k=0; k<currentMessagePath.at(i).getOutline().at(j).size(); k++){
                    ofPoint tp = currentMessagePath.at(i).getOutline().at(j).getVertices().at(k);
                    ofVec3f direction = (tp-center).normalize();
                    tp += direction*max(0.0f,currentDistance);
                    if(!(k%(int)ceil(currentMessagePoints/1000.0f))){
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
        ofScale(currentMessageScaling, currentMessageScaling);
        ofTranslate((fboCanvas.getWidth()/currentMessageScaling-myFont.stringWidth(currentMessage))/2,
                    (fboCanvas.getHeight()/currentMessageScaling-myFont.stringHeight(currentMessage))/2+myFont.stringHeight("Tell"));

        ofNoFill();
        ofSetLineWidth(1.5);
        for(int i=0; i<currentMessagePath.size(); i++){
            ofVec3f center = currentMessagePath.at(i).getTessellation().getCentroid();
            for(int j=0; j<currentMessagePath.at(i).getOutline().size(); j++){
                for(int k=0; k<currentMessagePath.at(i).getOutline().at(j).size(); k++){
                    ofPoint tp = currentMessagePath.at(i).getOutline().at(j).getVertices().at(k);
                    ofVec3f direction = (tp-center).normalize();
                    tp += direction*max(0.0f,currentDistance);
                    ofPoint maxDistance = tp+direction*originalDistance;
                    if(!(k%(int)ceil(currentMessagePoints/1000.0f))){
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
}

