#pragma once

#include "ofMain.h"
#include "ofxGui.h"


#include "tcAdsClient.h"

class ofAppMain : public ofBaseApp{
	private:
		tcAdsClient* _tcClient;
		double _AdsData[8];
		unsigned long _lHdlVar_Read_Data;

	public:

		ofxPanel gui;
		ofxButton button;

		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		void exit();

		

};
