#pragma once

// ofx classes, addons
#include "ofMain.h"
#include "ofUtils.h"
#include "ofxGui.h"


// custom classes
#include "ofAppDisplay.h"
#include "ofAppExperiment.h"
#include "tcAdsClient.h"
#include "myUtils.h"


void __stdcall onEventCallbackTCADS(AmsAddr*, AdsNotificationHeader*, ULONG);

class ofAppExperiment;

class ofAppMain : public ofBaseApp{
	

	private:

		//
		// variables
		//

		tcAdsClient *_tcClientCont, *_tcClientEvent;

		unsigned long _lHdlVar_Read_Data, _lHdlVar_Read_SystemState, _lHdlVar_Read_OpsEnabled, _lHdlVar_Read_SystemError,
			_lHdlNot_Read_OpsEnabled, _lHdlNot_Read_SystemState, _lHdlNot_Read_SystemError;


		float _timeRefreshCheck = 1.0f; // 1 second refresh
		float _timeCheck;

		displayData _display1Data, _display2Data;

		int _systemState[2] = { -1, -1 };

		// GUI system
		ofxPanel _guiSystem;
		ofxButton _btnReqState_Reset, _btnReqState_Init, _btnReqState_Calibrate, _btnReqState_HomingAuto, _btnReqState_HomingManual, _btnReqState_Run, 
			_btnEnableDrive, _btnDisableDrive, _btnQuit;
		ofxToggle _btnToggleRecordData;
		ofxGuiGroup _grpReqState, _grpDriveControl;
		ofxLabel _lblEtherCAT;
		ofParameter<string>  _lblFRM, _lblSysState, _lblOpsEnabled, _lblSysError;
		ofParameterGroup _ofGrpSys;

		// GUI experiment
		ofxPanel _guiExperiment;
		ofxButton _btnExpLoad, _btnExpStart, _btnExpPause, _btnExpResume, _btnExpStop;
		ofxGuiGroup _grpExpControl;
		ofxLabel _lblExpLoaded;
		ofParameterGroup _grpExpState;
		
		//
		// custom
		//
		void setupTCADS();
		void setupGUI();

		void buttonPressed(const void * sender);
		void recordDataTogglePressed(bool & value);
		

	public:
		//
		// variables
		//

		shared_ptr<ofAppExperiment> experimentApp;

		double AdsData[9] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };

		shared_ptr<ofAppDisplay> display1 = 0;
		shared_ptr<ofAppDisplay> display2 = 0;

		ofParameter<string> lblExpState;
		
		ofParameter<int> lblTrialNumber, lblBlockNumber;

		//
		// functions
		//

		// openFrameworks
		void setup();
		void update();
		void draw();
		void keyPressed(int key);
		void keyReleased(int key);
		void windowResized(int w, int h);
		void exit();

		// custom

		void requestStateChange(int reqState);
		void requestDriveEnableDisable(bool enable);
		bool systemIsInState(int state);
		void handleCallback(AmsAddr*, AdsNotificationHeader*);
};
