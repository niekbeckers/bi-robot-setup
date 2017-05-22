#include "ofMain.h"
#include "ofAppMain.h"
#include "ofAppDisplay.h"

//========================================================================
int main( ){

	ofGLFWWindowSettings settings;
	settings.width = 600;
	settings.height = 600;
	settings.setPosition(ofVec2f(0, 50));
	settings.resizable = false;
	settings.title = "Controls";
	shared_ptr<ofAppBaseWindow> mainWindow = ofCreateWindow(settings);

	settings.width = 1024;
	settings.height = 768;
	settings.monitor = 2;
	//settings.setPosition(ofVec2f(600, 50));
	settings.resizable = true;
	//settings.multiMonitorFullScreen = true;
	settings.title = "Display";
	settings.windowMode = OF_FULLSCREEN;
	shared_ptr<ofAppBaseWindow> displayWindow = ofCreateWindow(settings);

	shared_ptr<ofAppMain> mainApp(new ofAppMain);
	shared_ptr<ofAppDisplay> displayApp(new ofAppDisplay);

	displayApp->mainApp = mainApp;

	
	ofRunApp(mainWindow, mainApp);
	ofRunApp(displayWindow, displayApp);
	ofRunMainLoop();

}
