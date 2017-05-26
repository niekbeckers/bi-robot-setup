#include "ofAppMain.h"

//--------------------------------------------------------------
void ofAppMain::setup(){


	ofSetLogLevel(OF_LOG_VERBOSE);

	ofLogError("ofAppMain::setup()");

	// set up window
	ofBackground(ofColor::blueSteel);
	ofSetWindowTitle("Control");

	// set up GUI
	setupGUI();

	// setup tcAdsClient
	setupTCADS();

	_timeCheck = ofGetElapsedTimef();

	// point struct pointers of displays to data structs
	display1->pData = &_display1Data;
	display2->pData = &_display2Data;
}

//--------------------------------------------------------------
void ofAppMain::update(){

	bool prevTrialRunning = (bool)AdsData[8];

	// read continuous ADS data
	_tcClientCont->read(_lHdlVar_Read_Data, &AdsData, sizeof(AdsData));
	 
	// set data in displayData structs
	// display 1
	_display1Data.posCursorX = AdsData[0];
	_display1Data.posCursorY = AdsData[1];
	_display1Data.posTargetX = AdsData[4];
	_display1Data.posTargetY = AdsData[5];

	// display 2
	_display2Data.posCursorX = AdsData[2];
	_display2Data.posCursorY = AdsData[3];
	_display2Data.posTargetX = AdsData[6];
	_display2Data.posTargetY = AdsData[7];

	// check if trial is done
	if (prevTrialRunning  && !(bool)AdsData[8]) {
//		experimentApp->eventTrialDone();
	}

	// periodic check
	if (ofGetElapsedTimef() - _timeCheck > _timeRefreshCheck) {
		_timeCheck = ofGetElapsedTimef();

		// Check TwinCAT/ADS
		if (_tcClientCont->nErr == 0) {
			_lblEtherCAT = "ON";
			_lblEtherCAT.setBackgroundColor(ofColor::darkGreen);
		}
		else {
			// error, probably no ADS running
			_lblEtherCAT = "OFF";
			_lblEtherCAT.setBackgroundColor(ofColor::red);
		}

		// Check trial running
		if ((int)AdsData[8] == 1) {
			_lblTrialRunning = "Trial running";
			_lblTrialRunning.setBackgroundColor(ofColor::darkGreen);
		}
		else {
			_lblTrialRunning = "No trial running";
			_lblTrialRunning.setBackgroundColor(_guiDefaultBackgroundColor);
		}

		// frame rate in GUI
		_lblFRM = ofToString((int)ofGetFrameRate()) + " fps";
	}	
}

//--------------------------------------------------------------
void ofAppMain::draw() {
	_guiSystem.draw(); 
	_guiExperiment.draw();
}

//--------------------------------------------------------------
void ofAppMain::setupTCADS()
{
	// set up tcAdsClient for data reading
	_tcClientCont = new tcAdsClient(adsPort);

	// get variable handles for ADS
	char szVar0[] = { "Object1 (ModelBaseBROS).Output.DataToADS" };
	_lHdlVar_Read_Data = _tcClientCont->getVariableHandle(szVar0, sizeof(szVar0));


	// set up tcAdsClient for events
	_tcClientEvent = new tcAdsClient(adsPort);

	// get variables
	char szVar1[] = { "Object1 (ModelBaseBROS).BlockIO.VecCon_SystemStates" };
	_lHdlVar_Read_SystemState = _tcClientEvent->getVariableHandle(szVar1, sizeof(szVar1));
	_lHdlNot_Read_SystemState = _tcClientEvent->registerTCAdsDeviceNotification(_lHdlVar_Read_SystemState, (unsigned long)(this), onEventCallbackTCADS, 16);

	char szVar2[] = { "Object1 (ModelBaseBROS).BlockIO.VecCon_OpsEnabled" };
	_lHdlVar_Read_OpsEnabled = _tcClientEvent->getVariableHandle(szVar2, sizeof(szVar2));
	_lHdlNot_Read_OpsEnabled = _tcClientEvent->registerTCAdsDeviceNotification(_lHdlVar_Read_OpsEnabled, (unsigned long)(this), onEventCallbackTCADS, 2);

	char szVar3[] = { "Object1 (ModelBaseBROS).BlockIO.VecCon_Errors" };
	_lHdlVar_Read_SystemError = _tcClientEvent->getVariableHandle(szVar3, sizeof(szVar3));
	_lHdlNot_Read_SystemError = _tcClientEvent->registerTCAdsDeviceNotification(_lHdlVar_Read_SystemError, (unsigned long)(this), onEventCallbackTCADS, 16);

	
}
 
//--------------------------------------------------------------
void ofAppMain::setupGUI()
{
	// add listeners
	_btnReqState_Reset.addListener(this, &ofAppMain::buttonPressed);
	_btnReqState_Init.addListener(this, &ofAppMain::buttonPressed);
	_btnReqState_Calibrate.addListener(this, &ofAppMain::buttonPressed);
	_btnReqState_HomingAuto.addListener(this, &ofAppMain::buttonPressed);
	_btnReqState_HomingManual.addListener(this, &ofAppMain::buttonPressed);
	_btnReqState_Run.addListener(this, &ofAppMain::buttonPressed);
	_btnEnableDrive.addListener(this, &ofAppMain::buttonPressed);
	_btnDisableDrive.addListener(this, &ofAppMain::buttonPressed);

	// toggle
	_btnToggleRecordData.addListener(this, &ofAppMain::recordDataTogglePressed);

	_btnExpLoad.addListener(this, &ofAppMain::buttonPressed);
	_btnExpStart.addListener(this, &ofAppMain::buttonPressed);
	_btnExpPause.addListener(this, &ofAppMain::buttonPressed);
	_btnExpResume.addListener(this, &ofAppMain::buttonPressed);
	_btnExpStop.addListener(this, &ofAppMain::buttonPressed);


	// setup GUIs
	_guiSystem.setup("System Control");
	_guiSystem.setPosition(10.0, 10.0);
	
	_guiExperiment.setup("Experiment");
	_guiExperiment.setPosition(300.0, 10.0);

	_guiSystem.setDefaultHeight(30.0);
	_guiExperiment.setDefaultHeight(30);

	// GUI system
	_guiSystem.add(_lblEtherCAT.setup("EtherCAT/ADS", ""));
	_guiSystem.add(_lblFRM.set("Frame rate", ""));
	_guiDefaultBackgroundColor = _lblEtherCAT.getBackgroundColor();

	_ofGrpSys.setName("System");
	_ofGrpSys.add(_lblSysState.set("System State", "[,]"));
	_ofGrpSys.add(_lblSysError.set("System Error", "[,]"));
	_ofGrpSys.add(_lblOpsEnabled.set("Drives Enabled", "[,]"));
	_guiSystem.add(_ofGrpSys);
	
	// request state
	_grpReqState.setup("Request state");
	//_grpReqState.setName("Request state");
	_grpReqState.add(_btnReqState_Reset.setup("Reset [0]"));
	_grpReqState.add(_btnReqState_Init.setup("Init [1]"));
	_grpReqState.add(_btnReqState_Calibrate.setup("Calibrate [299]"));
	_grpReqState.add(_btnReqState_HomingAuto.setup("Homing - Auto [399]"));
	_grpReqState.add(_btnReqState_HomingManual.setup("Homing - Manual [399]"));
	_grpReqState.add(_btnReqState_Run.setup("Run [4]"));
	_guiSystem.add(&_grpReqState);


	// drive controls
	_grpDriveControl.setup("Drive control");
	//_grpDriveControl.setName("Drive control");
	_grpDriveControl.add(_btnEnableDrive.setup("Enable drives"));
	_grpDriveControl.add(_btnDisableDrive.setup("Disable drives"));
	_guiSystem.add(&_grpDriveControl);

	_guiSystem.add(_btnToggleRecordData.setup("Record data", false));


	// GUI experiment
	_grpExpControl.setup("Experiment control");
	_grpExpControl.setName("Experiment control");
	_grpExpControl.add(_lblExpLoaded.setup("Experiment", ""));
	_grpExpControl.add(_lblTrialRunning.setup("Trial", ""));
	_grpExpControl.add(_btnExpLoad.setup("Load"));
	_grpExpControl.add(_btnExpStart.setup("Start"));
	_grpExpControl.add(_btnExpStop.setup("Stop"));
	_grpExpControl.add(_btnExpPause.setup("Pause"));
	_grpExpControl.add(_btnExpResume.setup("Resume"));
	_guiExperiment.add(&_grpExpControl);
}

//--------------------------------------------------------------
void ofAppMain::requestStateChange(int reqState)
{
	// button event in GUI, most likely a request to the ADS (TwinCAT), hence set up client
	tcAdsClient* tcClient = new tcAdsClient(adsPort);

	// Request State
	char szVar[] = { "Object1 (ModelBaseBROS).ModelParameters.Requestedstate_Value" };
	long lHdlVar = tcClient->getVariableHandle(szVar, sizeof(szVar));

	double state = (double)reqState; // cast to double because the simulink model/TMC object expects a double
	
	// write state request to variable handle
	tcClient->write(lHdlVar, &state, sizeof(state));

	// clean up
	tcClient->disconnect();
}

//--------------------------------------------------------------
void ofAppMain::requestDriveEnableDisable(bool enable)
{
	// button event in GUI, most likely a request to the ADS (TwinCAT), hence set up client
	tcAdsClient* tcClient = new tcAdsClient(adsPort);

	long lHdlVar; // variable handle

	if (enable) { 
		// enable drives
		char szVar1[] = { "Object1 (ModelBaseBROS).ModelParameters.EnableDrives_Value" };
		lHdlVar = tcClient->getVariableHandle(szVar1, sizeof(szVar1));
	}
	else {
		// disable drives
		char szVar1[] = { "Object1 (ModelBaseBROS).ModelParameters.DisableDrives_Value" };
		lHdlVar = tcClient->getVariableHandle(szVar1, sizeof(szVar1));
	}
	
	// write 1 and 0 to variable handle (pulse)
	double val = 1.0;
	tcClient->write(lHdlVar, &val, sizeof(val));
	val = 0.0;
	tcClient->write(lHdlVar, &val, sizeof(val));

	// clean up
	tcClient->disconnect();
}

//--------------------------------------------------------------
void ofAppMain::buttonPressed(const void * sender)
{
	ofxButton * button = (ofxButton*)sender;
	string clickedBtn = button->getName();

	if (clickedBtn.compare(ofToString("Reset [0]")) == 0) {
		requestStateChange(0);
	} 
	else if (clickedBtn.compare(ofToString("Init [1]")) == 0) {
		requestStateChange(1);
	}
	else if (clickedBtn.compare(ofToString("Calibrate [299]")) == 0) {
		requestStateChange(2);
	}
	else if (clickedBtn.compare(ofToString("Homing - Auto [399]")) == 0) {
		requestStateChange(302);
	}
	else if (clickedBtn.compare(ofToString("Homing - Manual [399]")) == 0) {
		requestStateChange(301);
	}
	else if (clickedBtn.compare(ofToString("Run [4]")) == 0) {
		requestStateChange(4);
	}
	else if (clickedBtn.compare(ofToString("Enable drives")) == 0) {
		requestDriveEnableDisable(true);
	}
	else if (clickedBtn.compare(ofToString("Disable drives")) == 0) {
		requestDriveEnableDisable(false);
	}
	else if (clickedBtn.compare(ofToString("Load")) == 0) {
		//experimentApp->loadExperimentXML(); // load experiment XML
	}
	else if (clickedBtn.compare(ofToString("Start")) == 0) {
		//experimentApp->start();
	}
	else if (clickedBtn.compare(ofToString("Stop")) == 0) {
		//experimentApp->stop();
	}
	else if (clickedBtn.compare(ofToString("Pause")) == 0) {
		//experimentApp->pause();
	}
	else if (clickedBtn.compare(ofToString("Resume")) == 0) {
		//experimentApp->resume();
	}
}

//--------------------------------------------------------------
void ofAppMain::recordDataTogglePressed(bool & value)
{
	// button event in GUI, most likely a request to the ADS (TwinCAT), hence set up client
	tcAdsClient* tcClient = new tcAdsClient(adsPort);

	// Request State
	char szVar[] = { "Object1 (ModelBaseBROS).ModelParameters.Recorddata1yes0no_Value" };
	long lHdlVar = tcClient->getVariableHandle(szVar, sizeof(szVar));

	// Write 1 to enable data recording, 0 to disable data recording
	double val = value ? 1.0 : 0.0;

	// write
	tcClient->write(lHdlVar, &val, sizeof(val));

	// clean up
	tcClient->disconnect();
}

//--------------------------------------------------------------
bool ofAppMain::systemIsInState(int state)
{
	return (_systemState[0] == state && _systemState[1] == state);
}

//--------------------------------------------------------------
void ofAppMain::keyPressed(int key){

}

//--------------------------------------------------------------
void ofAppMain::keyReleased(int key){

}
//--------------------------------------------------------------
void ofAppMain::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofAppMain::exit() {

	// remove listeners
	_btnReqState_Reset.removeListener(this, &ofAppMain::buttonPressed);
	_btnReqState_Init.removeListener(this, &ofAppMain::buttonPressed);
	_btnReqState_Calibrate.removeListener(this, &ofAppMain::buttonPressed);
	_btnReqState_HomingAuto.removeListener(this, &ofAppMain::buttonPressed);
	_btnReqState_HomingManual.removeListener(this, &ofAppMain::buttonPressed);
	_btnReqState_Run.removeListener(this, &ofAppMain::buttonPressed);
	_btnEnableDrive.removeListener(this, &ofAppMain::buttonPressed);
	_btnDisableDrive.removeListener(this, &ofAppMain::buttonPressed);

	// remove listeners
	_btnExpLoad.removeListener(this, &ofAppMain::buttonPressed);
	_btnExpStart.removeListener(this, &ofAppMain::buttonPressed);
	_btnExpPause.removeListener(this, &ofAppMain::buttonPressed);
	_btnExpResume.removeListener(this, &ofAppMain::buttonPressed);
	_btnExpStop.removeListener(this, &ofAppMain::buttonPressed);


	_btnToggleRecordData = false;
	_btnToggleRecordData.removeListener(this, &ofAppMain::recordDataTogglePressed);

	// disconnect ADS clients
	_tcClientCont->disconnect();
	_tcClientEvent->disconnect();
}

//--------------------------------------------------------------
void ofAppMain::handleCallback(AmsAddr* pAddr, AdsNotificationHeader* pNotification)
{
	char buf[20];

	if (pNotification->hNotification == _lHdlNot_Read_OpsEnabled) {
		bool * data = (bool *)pNotification->data;
		sprintf(buf, "[%s,%s]", data[0] ? "T" : "F", data[1] ? "T" : "F");
		ofLogVerbose("Drive Enabled: " + ofToString(buf));
		_lblOpsEnabled = ofToString(buf);
	} 
	else if (pNotification->hNotification == _lHdlNot_Read_SystemError)  {
		double * data = (double *)pNotification->data;
		sprintf(buf, "[%d, %d]", (int)data[0], (int)data[1]);
		ofLogVerbose("System Error: " + ofToString(buf));
		_lblSysError = ofToString(buf);
	}
	else if (pNotification->hNotification == _lHdlNot_Read_SystemState) {
		double * data = (double *)pNotification->data;
		_systemState[0] = (int)data[0];
		_systemState[1] = (int)data[1];

		sprintf(buf, "[%d, %d]", _systemState[0], _systemState[1]);
		ofLogVerbose("System State: " + ofToString(buf));
		_lblSysState = ofToString(buf);

		// in case the robots are "at home", signal this to the experiment app
//		if (systemIsInState(399)) experimentApp->eventAtHome();
	}
	
	// print (to screen)) the value of the variable 
	ofLogVerbose("Notification: " + ofToString(pNotification->hNotification) + " SampleSize: " + ofToString(pNotification->cbSampleSize));
}

//--------------------------------------------------------------
void __stdcall onEventCallbackTCADS(AmsAddr* pAddr, AdsNotificationHeader* pNotification, ULONG hUser)
{
	// cast hUser to class pointer
	ofAppMain* ptr = (ofAppMain*)(hUser);

	// call handleCallback for access to ofAppMain class
	ptr->handleCallback(pAddr, pNotification);
}