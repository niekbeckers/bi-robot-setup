#pragma once

// ofx classes, addons
#include "ofMain.h"
#include "ofUtils.h"
#include "ofxDatGui.h"

// custom classes
#include "tcAdsClient.h"

void __stdcall onEventCallbackTCADS(AmsAddr*, AdsNotificationHeader*, ULONG);

class ofAppMain : public ofBaseApp{
	

	private:
		tcAdsClient *_tcClientCont, *_tcClientEvent;

		double _AdsData[8];
		unsigned long _lHdlVar_Read_Data, _lHdlVar_Read_SystemState, _lHdlVar_Read_OpsEnabled, _lHdlVar_Read_SystemError,
			_lHdlNot_Read_OpsEnabled, _lHdlNot_Read_SystemState, _lHdlNot_Read_SystemError;
		const unsigned long adsPort = 350;
		string _sOpsEnabled, _sSystemState, _sSystemError, _sOpsEnabledNew, _sSystemStateNew, _sSystemErrorNew;

	public:
		void HandleCallback(AmsAddr*, AdsNotificationHeader*);
		void setup();
		void update();
		void draw();
		void setupTCADS();

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

		
		ofxDatGui* gui = new ofxDatGui(ofxDatGuiAnchor::TOP_LEFT);
		ofxDatGuiFolder *guiFldrReqSysState, *guiFldrMotorControl;
		ofxDatGuiTextInput *guiLblSysState, *guiLblOpsEnabled, *guiLblSysError;


		void setupGUI();
		void updateGUI();
		void onButtonEventReqState(ofxDatGuiButtonEvent e);
};
