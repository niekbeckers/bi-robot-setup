#include "ofMain.h"
#include "ofAppMain.h"
#include "ofAppDisplay.h"
#include "ofAppExperiment.h"
#include "ofAppGLFWWindow.h"
#include "GLFW/glfw3.h"

//========================================================================
int main( ){

	// set console position
	HWND consoleWindow = GetConsoleWindow();
	SetWindowPos(consoleWindow, 0, 900, 50, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	// check number of displays/windows
	int monitorCount = 1;
	//ofWindowMode wmode = ofWindowMode::OF_FULLSCREEN;
	ofWindowMode wmode = ofWindowMode::OF_WINDOW;
	if (glfwInit()) {
		glfwGetMonitors(&monitorCount);
		wmode = (monitorCount > 1) ? ofWindowMode::OF_FULLSCREEN : ofWindowMode::OF_WINDOW;
	}

	//
	// mainWindow
	//
	ofGLFWWindowSettings settings0;
	settings0.setSize(980, 720);
	settings0.setPosition(ofVec2f(50, 50));
	settings0.title = "Controls";
	settings0.windowMode = OF_WINDOW;
	shared_ptr<ofAppBaseWindow> mainWindow = ofCreateWindow(settings0);

	ofGLFWWindowSettings settings1;
	settings1.setPosition(ofVec2f(50, 50));
	settings1.visible = false;
	settings1.title = "Experiment";
	settings1.windowMode = OF_WINDOW;
	shared_ptr<ofAppBaseWindow> experimentWindow = ofCreateWindow(settings1);

	//
	// display1Window
	//
	ofGLFWWindowSettings settings2;
	settings2.windowMode = wmode;
	shared_ptr<ofAppBaseWindow> display1Window = ofCreateWindow(settings2);
	
	//
	// display2Window
	//
	ofGLFWWindowSettings settings3;
	settings3.setPosition(ofVec2f(-2560, 0));
	settings3.windowMode = wmode;
	shared_ptr<ofAppBaseWindow> display2Window = ofCreateWindow(settings3);

	shared_ptr<ofAppDisplay> display1App(new ofAppDisplay);
	shared_ptr<ofAppDisplay> display2App(new ofAppDisplay);
	shared_ptr<ofAppExperiment> experimentApp(new ofAppExperiment);
	shared_ptr<ofAppMain> mainApp(new ofAppMain);
	
	// give pointers to classes
	mainApp->display1 = display1App;
	mainApp->display2 = display2App;
	mainApp->experimentApp = experimentApp;

	experimentApp->mainApp = mainApp;
	experimentApp->display1 = display1App;
	experimentApp->display2 = display2App;

	// run of apps
	ofRunApp(mainWindow, mainApp);
	ofRunApp(display1Window, display1App);
	ofRunApp(display2Window, display2App);
	ofRunApp(experimentWindow, experimentApp);

	ofRunMainLoop();

}
