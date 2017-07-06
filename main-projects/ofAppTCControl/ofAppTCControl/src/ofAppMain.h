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

static std::string StringSystemStateLabel(const SystemState value) {
	static std::map<SystemState, std::string> strings;
	if (strings.size() == 0) {
#define INSERT_ELEMENT(p) strings[p] = #p
		INSERT_ELEMENT(FAULT);
		INSERT_ELEMENT(RESET);
		INSERT_ELEMENT(INIT);
		INSERT_ELEMENT(REQCALIBRATION);
		INSERT_ELEMENT(DOCALIBRATION);
		INSERT_ELEMENT(CALIBRATIONDONE);
		INSERT_ELEMENT(REQHOMINGMANUAL);
		INSERT_ELEMENT(REQHOMINGAUTO);
		INSERT_ELEMENT(ATHOME);
		INSERT_ELEMENT(RUN);
#undef INSERT_ELEMENT
	}

	return strings[value];
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
			_lHdlNot_Read_OpsEnabled, _lHdlNot_Read_SystemState, _lHdlNot_Read_SystemError, _lHdlVar_Connected, _lHdlVar_ConnectionStiffness, _lHdlVar_ConnectionDamping,
			_lHdlVar_RecordData;


		float _timeRefreshCheck = 1.0f; // 1 second refresh
		float _timeCheck;

		displayData _display1Data, _display2Data;
		SystemState _systemState[2] = { SystemState::FAULT, SystemState::FAULT };

		// GUI system
		ofxPanel _guiSystem;
		ofxButton _btnReqState_Reset, _btnReqState_Init, _btnReqState_Calibrate, _btnReqState_HomingAuto, _btnReqState_HomingManual, _btnCalibrateForceSensor,
			_btnReqState_Run, _btnEnableDrive, _btnDisableDrive;
		ofxToggle _btnToggleRecordData;
		ofxGuiGroup _grpReqState, _grpDriveControl;
		ofxLabel _lblEtherCAT;
		ofParameter<string>  _lblFRM, _lblOpsEnabled, _lblSysError, _lblSysState[2];
		ofParameterGroup _ofGrpSys;

		// GUI experiment
		ofxPanel _guiExperiment;
		ofxButton _btnExpLoad, _btnExpStart, _btnExpStop;
		ofxToggle _btnExpPauseResume, _btnDebugMode;
		ofxGuiGroup _grpExpControl;
		ofParameterGroup _grpExpState;

		ofxPanel _guiAdmittance;
		ofxToggle _btnSetConnected;
		ofxGuiGroup _grpConnectionControl;
		ofxButton _btnConnSetStiffness, _btnConnSetDamping;
		ofxLabel _lblConnStiffness, _lblConnDamping;
		
		//
		// custom
		//
		void setupTCADS();
		void setupGUI();
		void initGUI();
		void buttonPressed(const void * sender);
		void recordDataTogglePressed(bool & value);
		void pauseExperimentTogglePressed(bool & value);
		void experimentDebugModeTogglePressed(bool & value);
		void calibrateForceSensors();
		

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
		void exit();

		

		// custom
		void requestStateChange(int reqState);
		void requestDriveEnableDisable(bool enable);
		bool systemIsInState(int state) { return (_systemState[0] == state && _systemState[1] == state); }
		bool systemIsInState(SystemState state) { return systemIsInState((int)state); }
		bool systemIsInError() { return _systemState[0] == SystemState::FAULT || _systemState[1] == SystemState::FAULT; };
		void setConnectionStiffness(double Kp);
		void setConnectionDamping(double Kd);
		void setConnectionEnabled(bool & value);
		void handleCallback(AmsAddr*, AdsNotificationHeader*);
};