#include "ofMain.h"
#include "ofAppMain.h"
#include "ofAppDisplay.h"

//========================================================================
int main( ){

	ofGLFWWindowSettings settings;
	settings.width = 600;
	settings.height = 600;
	settings.setPosition(ofVec2f(0, 50));
	settings.resizable = true;
	settings.title = "Controls";
	settings.windowMode = OF_WINDOW;
	shared_ptr<ofAppBaseWindow> mainWindow = ofCreateWindow(settings);

	settings.width = 1024;
	settings.height = 768;
	settings.monitor = 2;
	//settings.setPosition(ofVec2f(600, 50));
	settings.resizable = true;
	//settings.multiMonitorFullScreen = true;
	settings.title = "Display";
	settings.windowMode = OF_FULLSCREEN;
	shared_ptr<ofAppBaseWindow> display1Window = ofCreateWindow(settings);

	shared_ptr<ofAppMain> mainApp(new ofAppMain);
	shared_ptr<ofAppDisplay> display1App(new ofAppDisplay);

	mainApp->display1 = display1App;


	
	ofRunApp(mainWindow, mainApp);
	ofRunApp(display1Window, display1App);
	ofRunMainLoop();

}
