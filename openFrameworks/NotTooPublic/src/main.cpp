#include "ofMain.h"
#include "Bling.h"
#include "Secret.h"
#include "Wish.h"
#include "ofAppGlutWindow.h"

//========================================================================
int main(int argc, char* argv[]){

    ofAppGlutWindow window;
	// can be OF_WINDOW or OF_FULLSCREEN
	ofSetupOpenGL(&window, 900,700, OF_FULLSCREEN);

    if((argc > 1) && !string(argv[1]).compare("-b"))
        ofRunApp( new Bling());
    else if((argc > 1) && !string(argv[1]).compare("-s"))
        ofRunApp( new Secret());
    else
        ofRunApp( new Wish());
}
