#include "ofMain.h"
#include "ofAppMain.h"
#include "ofDisplayApp.h"

//========================================================================
int main( ){

	ofGLFWWindowSettings settings;
	settings.width = 600;
	settings.height = 600;
	settings.setPosition(ofVec2f(0, 50));
	settings.resizable = false;
	settings.title = "Controls";
	shared_ptr<ofAppBaseWindow> mainWindow = ofCreateWindow(settings);

	settings.width = 300;
	settings.height = 300;
	settings.setPosition(ofVec2f(600, 50));
	//settings.resizable = false;
	shared_ptr<ofAppBaseWindow> displayWindow = ofCreateWindow(settings);

	shared_ptr<ofAppMain> mainApp(new ofAppMain);
	shared_ptr<ofDisplayApp> displayApp(new ofDisplayApp);
	//mainApp->gui = guiApp;

	
	ofRunApp(mainWindow, mainApp);
	ofRunApp(displayWindow, displayApp);
	ofRunMainLoop();

	//ofSetupOpenGL(1024,768,OF_WINDOW);			// <-------- setup the GL context



	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	//ofRunApp(new ofAppMain());

}
