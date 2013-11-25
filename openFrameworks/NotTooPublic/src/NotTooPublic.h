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
        ofxOscReceiver myOscReceiver;
        ofxOscMessage myOscMessage;
        deque< pair<string,string> > myMessages;
        ofImage creditImage;
        deque<ofImage> introImages;
        ofTrueTypeFont myFont;
        long long int nowMillis, lastStateChangeMillis, startMillis;
        int currentFadeValue;
        ofFbo fboTitle, fboCanvas;
        void loadIntroImages(string filenamePrefix);
};
