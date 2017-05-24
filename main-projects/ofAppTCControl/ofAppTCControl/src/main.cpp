#include "ofMain.h"
#include "ofAppMain.h"
#include "ofAppDisplay.h"

//========================================================================
int main( ){

	//
	// mainWindow
	//
	ofGLFWWindowSettings settings;
	settings.width = 600;
	settings.height = 600;
	settings.setPosition(ofVec2f(0, 50));
	settings.resizable = true;
	settings.title = "Controls";
	settings.windowMode = OF_WINDOW;
	shared_ptr<ofAppBaseWindow> mainWindow = ofCreateWindow(settings);

	//
	// display1Window
	//
	ofGLFWWindowSettings settings1;
	//settings1.monitor = 0;
	settings1.windowMode = OF_FULLSCREEN;
	shared_ptr<ofAppBaseWindow> display1Window = ofCreateWindow(settings1);

	//
	// display2Window
	//
	ofGLFWWindowSettings settings2;
	//settings2.monitor = 0;
	settings2.setPosition(ofVec2f(-2560, 0));
	settings2.windowMode = OF_FULLSCREEN;
	shared_ptr<ofAppBaseWindow> display2Window = ofCreateWindow(settings2);

	shared_ptr<ofAppMain> mainApp(new ofAppMain);
	shared_ptr<ofAppDisplay> display1App(new ofAppDisplay);
	shared_ptr<ofAppDisplay> display2App(new ofAppDisplay);

	mainApp->display1 = display1App;
	mainApp->display2 = display2App;

	ofRunApp(mainWindow, mainApp);
	ofRunApp(display1Window, display1App);
	ofRunApp(display2Window, display2App);
	ofRunMainLoop();

}
