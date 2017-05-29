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

enum SystemState {
	FAULT = -1,
	RESET = 0,
	INIT = 1,
	REQCALIBRATION = 2,
	DOCALIBRATION = 201,
	CALIBRATIONDONE = 299,
	REQHOMINGMANUAL = 301,
	REQHOMINGAUTO = 302,
	ATHOME = 399,
	RUN = 4
};


void __stdcall onEventCallbackTCADS(AmsAddr*, AdsNotificationHeader*, ULONG);

class ofAppExperiment;

class ofAppMain : public ofBaseApp{
	

	private:

		//
		// variables
		//

		tcAdsClient *_tcClientCont, *_tcClientEvent;

		unsigned long _lHdlVar_Read_Data, _lHdlVar_Read_SystemState, _lHdlVar_Read_OpsEnabled, _lHdlVar_Read_SystemError, _lHdlVar_Write_CalibrateForceSensor,
			_lHdlNot_Read_OpsEnabled, _lHdlNot_Read_SystemState, _lHdlNot_Read_SystemError;


		float _timeRefreshCheck = 1.0f; // 1 second refresh
		float _timeCheck;

		displayData _display1Data, _display2Data;

		SystemState _systemState[2] = { SystemState::FAULT, SystemState::FAULT };

		// GUI system
		ofxPanel _guiSystem;
		ofxButton _btnReqState_Reset, _btnReqState_Init, _btnReqState_Calibrate, _btnReqState_HomingAuto, _btnReqState_HomingManual, _btnCalibrateForceSensor,
			_btnReqState_Run, _btnEnableDrive, _btnDisableDrive, _btnQuit;
		ofxToggle _btnToggleRecordData;
		ofxGuiGroup _grpReqState, _grpDriveControl;
		ofxLabel _lblEtherCAT;
		ofParameter<string>  _lblFRM, _lblSysState, _lblOpsEnabled, _lblSysError;
		ofParameterGroup _ofGrpSys;

		// GUI experiment
		ofxPanel _guiExperiment;
		ofxButton _btnExpLoad, _btnExpStart, _btnExpStop;
		ofxToggle _btnExpPauseResume;
		ofxGuiGroup _grpExpControl;
		ofParameterGroup _grpExpState;
		
		//
		// custom
		//
		void setupTCADS();
		void setupGUI();
		void buttonPressed(const void * sender);
		void recordDataTogglePressed(bool & value);
		void pauseExperimentTogglePressed(bool & value);
		

	public:
		//
		// variables
		//
		double AdsData[9] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };

		// pointers to other classes
		shared_ptr<ofAppExperiment> experimentApp = 0;
		shared_ptr<ofAppDisplay> display1 = 0;
		shared_ptr<ofAppDisplay> display2 = 0;

		// gui
		ofParameter<string> lblExpState, lblExpLoaded;
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
		bool systemIsInError() { return _systemState[0] == -1 || _systemState[1] == -1; };
		void handleCallback(AmsAddr*, AdsNotificationHeader*);
};
