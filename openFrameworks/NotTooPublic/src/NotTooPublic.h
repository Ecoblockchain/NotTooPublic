#pragma once

#include "ofMain.h"
#include "ofxOsc.h"

class NotTooPublic : public ofBaseApp{
	public:
		void setup();
		void update();
		void draw();

		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

	protected:
        const int FADE_DELTA = 5;
        ofxOscReceiver myOscReceiver;
        ofxOscMessage myOscMessage;
        deque< pair<string,string> > myMessages;
        ofImage creditImage;
        deque<ofImage> introImages;
        ofTrueTypeFont myFont;
        long long int nowMillis, lastStateChangeMillis, startMillis;
        int currentFadeValue;
        ofFbo fboTitle, fboCanvas;
        enum State {STATE_INTRO, STATE_BLANK, STATE_GOLD, STATE_PAUSE, STATE_DIAMOND, STATE_MESSAGE,
                    STATE_SPERM, STATE_LINES, STATE_CLEAR,STATE_OUTRO};
        State currentState;
        void loadIntroImages(string filenamePrefix);
        void loadTitleFbo(string filename);
        void drawIntro();
        void drawCredits();
        void stateLogicIntro();
        void stateLogicOutro();
        void stateLogicBlank();
        virtual void handleNewMessage()=0;
};
