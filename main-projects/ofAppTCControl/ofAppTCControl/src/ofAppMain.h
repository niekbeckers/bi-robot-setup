#pragma once

// ofx classes, addons
#include "ofMain.h"
#include "ofUtils.h"
#include "ofxGui.h"


// custom classes
#include "ofAppDisplay.h"
#include "tcAdsClient.h"

void __stdcall onEventCallbackTCADS(AmsAddr*, AdsNotificationHeader*, ULONG);

class ofAppMain : public ofBaseApp{
	

	private:

		//
		// variables
		//

		tcAdsClient *_tcClientCont, *_tcClientEvent;

		unsigned long _lHdlVar_Read_Data, _lHdlVar_Read_SystemState, _lHdlVar_Read_OpsEnabled, _lHdlVar_Read_SystemError,
			_lHdlNot_Read_OpsEnabled, _lHdlNot_Read_SystemState, _lHdlNot_Read_SystemError;

		ofxPanel _guiSystem, _guiExperiment;
		ofxButton _btnReqState_Reset, _btnReqState_Init, _btnReqState_Calibrate, _btnReqState_HomingAuto, _btnReqState_HomingManual, _btnReqState_Run, _btnEnableDrive, _btnDisableDrive;
		ofxToggle _btnToggleRecordData;
		ofxGuiGroup _grpReqState, _grpDriveControl;
		ofxLabel _lblEtherCAT;
		ofParameter<string>  _lblFRM, _lblSysState, _lblOpsEnabled, _lblSysError;
		ofParameterGroup _ofGrpSys;

		float _timeRefreshCheck = 1.0f; // 1 second refresh
		float _timeCheck;
		
		//
		// custom
		//
		void setupTCADS();
		void setupGUI();

		void ButtonPressed(const void * sender);
		void RecordDataTogglePressed(bool & value);
		

	public:
		//
		// variables
		//
		const unsigned long adsPort = 350;
		double AdsData[8];

		shared_ptr<ofAppDisplay> display1 = 0;
		shared_ptr<ofAppDisplay> display2 = 0;

		//
		// functions
		//

		// openFrameworks
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

		// custom

		void RequestStateChange(double reqState);
		void RequestDriveEnableDisable(bool enable);
		void HandleCallback(AmsAddr*, AdsNotificationHeader*);
};
