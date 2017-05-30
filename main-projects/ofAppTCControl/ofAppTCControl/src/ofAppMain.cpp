#include "ofAppMain.h"

using namespace std;

//--------------------------------------------------------------
void ofAppMain::setup(){
	ofSetLogLevel(OF_LOG_VERBOSE);

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

	_lblSysState[0] = StringSystemStateLabel(_systemState[0]);
	_lblSysState[1] = StringSystemStateLabel(_systemState[1]);

}

//--------------------------------------------------------------
void ofAppMain::update(){

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
	char szVar0[] = { "Object1 (ModelBROS).Output.DataToADS" };
	_lHdlVar_Read_Data = _tcClientCont->getVariableHandle(szVar0, sizeof(szVar0));


	// set up tcAdsClient for events
	_tcClientEvent = new tcAdsClient(adsPort);

	// get variables
	char szVar1[] = { "Object1 (ModelBROS).BlockIO.VecCon_SystemStates" };
	_lHdlVar_Read_SystemState = _tcClientEvent->getVariableHandle(szVar1, sizeof(szVar1));
	_lHdlNot_Read_SystemState = _tcClientEvent->registerTCAdsDeviceNotification(_lHdlVar_Read_SystemState, (unsigned long)(this), onEventCallbackTCADS, 16);

	char szVar2[] = { "Object1 (ModelBROS).BlockIO.VecCon_OpsEnabled" };
	_lHdlVar_Read_OpsEnabled = _tcClientEvent->getVariableHandle(szVar2, sizeof(szVar2));
	_lHdlNot_Read_OpsEnabled = _tcClientEvent->registerTCAdsDeviceNotification(_lHdlVar_Read_OpsEnabled, (unsigned long)(this), onEventCallbackTCADS, 2);

	char szVar3[] = { "Object1 (ModelBROS).BlockIO.VecCon_Errors" };
	_lHdlVar_Read_SystemError = _tcClientEvent->getVariableHandle(szVar3, sizeof(szVar3));
	_lHdlNot_Read_SystemError = _tcClientEvent->registerTCAdsDeviceNotification(_lHdlVar_Read_SystemError, (unsigned long)(this), onEventCallbackTCADS, 16);

	char szVar4[] = { "Object1 (ModelBROS).ModelParameters.CalibrateForceSensors_Value" };
	_lHdlVar_Write_CalibrateForceSensor = _tcClientEvent->getVariableHandle(szVar4, sizeof(szVar4));
	
}
 
//--------------------------------------------------------------
void ofAppMain::setupGUI()
{
	// add listeners
	_btnCalibrateForceSensor.addListener(this, &ofAppMain::buttonPressed);
	_btnQuit.addListener(this, &ofAppMain::buttonPressed);
	_btnReqState_Reset.addListener(this, &ofAppMain::buttonPressed);
	_btnReqState_Init.addListener(this, &ofAppMain::buttonPressed);
	_btnReqState_Calibrate.addListener(this, &ofAppMain::buttonPressed);
	_btnReqState_HomingAuto.addListener(this, &ofAppMain::buttonPressed);
	_btnReqState_HomingManual.addListener(this, &ofAppMain::buttonPressed);
	_btnReqState_Run.addListener(this, &ofAppMain::buttonPressed);
	_btnEnableDrive.addListener(this, &ofAppMain::buttonPressed);
	_btnDisableDrive.addListener(this, &ofAppMain::buttonPressed);
	_btnExpRestart.addListener(this, &ofAppMain::buttonPressed);
	_btnExpLoad.addListener(this, &ofAppMain::buttonPressed);
	_btnExpStart.addListener(this, &ofAppMain::buttonPressed);
	_btnExpStop.addListener(this, &ofAppMain::buttonPressed);

	// toggle
	_btnToggleRecordData.addListener(this, &ofAppMain::recordDataTogglePressed);
	_btnExpPauseResume.addListener(this, &ofAppMain::pauseExperimentTogglePressed);

	
	// setup GUIs
	int width = 300;
	_guiSystem.setDefaultWidth(width);
	_guiExperiment.setDefaultWidth(width);

	_guiSystem.setup("System Control");
	_guiSystem.setPosition(10.0, 10.0);
	
	_guiExperiment.setup("Experiment");
	_guiExperiment.setPosition(width+40, 10.0);

	_guiSystem.setDefaultHeight(30.0);
	_guiExperiment.setDefaultHeight(30);
	
	// GUI system
	//_guiSystem.add(_btnQuit.setup("Quit"));
	_guiSystem.add(_lblEtherCAT.setup("EtherCAT/ADS", ""));
	_guiSystem.add(_lblFRM.set("Frame rate", ""));
	_guiSystem.add(_btnCalibrateForceSensor.setup("Calibrate force sensors"));

	_ofGrpSys.setName("System states");
	_ofGrpSys.add(_lblSysState[0].set("State 1", ""));
	_ofGrpSys.add(_lblSysState[1].set("State 2", ""));
	_ofGrpSys.add(_lblSysError.set("System Error", "[,]"));
	_ofGrpSys.add(_lblOpsEnabled.set("Drives Enabled", "[,]"));
	_guiSystem.add(_ofGrpSys);
	
	// request state
	_grpReqState.setup("Request state");
	_grpReqState.setName("State request");
	_grpReqState.add(_btnReqState_Reset.setup("Reset"));
	_grpReqState.add(_btnReqState_Init.setup("Init"));
	_grpReqState.add(_btnReqState_Calibrate.setup("Calibrate"));
	_grpReqState.add(_btnReqState_HomingAuto.setup("Homing - Auto"));
	_grpReqState.add(_btnReqState_HomingManual.setup("Homing - Manual"));
	_grpReqState.add(_btnReqState_Run.setup("Run"));
	_guiSystem.add(&_grpReqState);

	// drive controls
	_grpDriveControl.setup("Drive control");
	_grpDriveControl.setName("Drive control");
	_grpDriveControl.add(_btnEnableDrive.setup("Enable drives"));
	_grpDriveControl.add(_btnDisableDrive.setup("Disable drives"));
	_guiSystem.add(&_grpDriveControl);

	// GUI experiment
	_guiExperiment.add(_btnToggleRecordData.setup("Record data", false));
	_guiExperiment.add(_btnExpLoad.setup("Load"));
	_guiExperiment.add(lblExpLoaded.set("", "No protocol loaded"));
	_guiExperiment.add(lblExpState.set("ExpState", ""));

	_grpExpControl.setup("Experiment control");
	_grpExpControl.setName("Experiment control");
	_grpExpControl.add(_btnExpStart.setup("Start"));
	_grpExpControl.add(_btnExpStop.setup("Stop"));
	_grpExpControl.add(_btnExpPauseResume.setup("Pause", false));
	_guiExperiment.add(&_grpExpControl);

	_grpExpState.setName("Experiment state");
	_grpExpState.add(lblBlockNumber.set("Block number", 2, 0, 4));
	_grpExpState.add(lblTrialNumber.set("Trial number", 8, 0, 10));
	_guiExperiment.add(_grpExpState);
	_guiExperiment.add(_btnExpRestart.setup("Restart experiment"));

	_guiSystem.setWidthElements(width);
	_guiExperiment.setWidthElements(width);

	ofLogVerbose(ofToString(_guiExperiment.getWidth()) + "  " + ofToString(_btnReqState_Run.getWidth()));
}

//--------------------------------------------------------------
void ofAppMain::requestStateChange(int reqState)
{
	// button event in GUI, most likely a request to the ADS (TwinCAT), hence set up client
	tcAdsClient* tcClient = new tcAdsClient(adsPort);

	// Request State
	char szVar[] = { "Object1 (ModelBROS).ModelParameters.Requestedstate_Value" };
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
		char szVar1[] = { "Object1 (ModelBROS).ModelParameters.EnableDrives_Value" };
		lHdlVar = tcClient->getVariableHandle(szVar1, sizeof(szVar1));
	}
	else {
		// disable drives
		char szVar1[] = { "Object1 (ModelBROS).ModelParameters.DisableDrives_Value" };
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

	if (clickedBtn.compare(ofToString("Quit")) == 0) {
		ofExit();
	}
	else if (clickedBtn.compare(ofToString("Reset")) == 0) {
		requestStateChange(0);
	} 
	else if (clickedBtn.compare(ofToString("Init")) == 0) {
		requestStateChange(1);
	}
	else if (clickedBtn.compare(ofToString("Calibrate")) == 0) {
		requestStateChange(2);
	}
	else if (clickedBtn.compare(ofToString("Homing - Auto")) == 0) {
		requestStateChange(302);
	}
	else if (clickedBtn.compare(ofToString("Homing - Manual")) == 0) {
		requestStateChange(301);
	}
	else if (clickedBtn.compare(ofToString("Run")) == 0) {
		requestStateChange(4);
	}
	else if (clickedBtn.compare(ofToString("Enable drives")) == 0) {
		requestDriveEnableDisable(true);
	}
	else if (clickedBtn.compare(ofToString("Disable drives")) == 0) {
		requestDriveEnableDisable(false);
	}
	else if (clickedBtn.compare(ofToString("Load")) == 0) {
		experimentApp->loadExperimentXML(); // load experiment XML
	}
	else if (clickedBtn.compare(ofToString("Start")) == 0) {
		experimentApp->startExperiment();
	}
	else if (clickedBtn.compare(ofToString("Stop")) == 0) {
		experimentApp->stopExperiment();
	}
	else if (clickedBtn.compare(ofToString("Restart experiment")) == 0) {
		experimentApp->restartExperiment();
	}
	else if (clickedBtn.compare(ofToString("Calibrate force sensors")) == 0) {
		// only allow force sensor calibration when in the following experiment states
		if (experimentApp->experimentState() == ExperimentState::BLOCKBREAK || 
			experimentApp->experimentState() == ExperimentState::IDLE || 
			experimentApp->experimentState() == ExperimentState::EXPERIMENTDONE) {
	
			double var = 1.0;
			_tcClientEvent->write(_lHdlVar_Write_CalibrateForceSensor, &var, sizeof(var));
			var = 0.0;
			_tcClientEvent->write(_lHdlVar_Write_CalibrateForceSensor, &var, sizeof(var));

			ofLogVerbose("Calibrate force sensor requested");
		}
		else {
			ofLogError("Incorrect experiment state, force sensor calibration not allowed (only during IDLE, BLOCKBREAK, EXPERIMENTDONE)");
		}
		
	}
	else {
		ofLogError("Button " + clickedBtn + " unknown");
	}
}

//--------------------------------------------------------------
void ofAppMain::recordDataTogglePressed(bool & value)
{
	// button event in GUI, most likely a request to the ADS (TwinCAT), hence set up client
	tcAdsClient* tcClient = new tcAdsClient(adsPort);

	// Request State
	char szVar[] = { "Object1 (ModelBROS).ModelParameters.Recorddata1yes0no_Value" };
	long lHdlVar = tcClient->getVariableHandle(szVar, sizeof(szVar));

	// Write 1 to enable data recording, 0 to disable data recording
	double val = value ? 1.0 : 0.0;

	// write
	tcClient->write(lHdlVar, &val, sizeof(val));

	// clean up
	tcClient->disconnect();
}

//--------------------------------------------------------------
void ofAppMain::pauseExperimentTogglePressed(bool & value) 
{
	if (value) {
		_btnExpPauseResume.setName("Resume");
		experimentApp->pauseExperiment();
	}
	else {
		_btnExpPauseResume.setName("Pause");
		experimentApp->resumeExperiment();
	}
}

//--------------------------------------------------------------
bool ofAppMain::systemIsInState(int state)
{
	return (_systemState[0] == state && _systemState[1] == state);
}

//--------------------------------------------------------------
bool ofAppMain::systemIsInState(SystemState state)
{
	return (_systemState[0] == state && _systemState[1] == state);
}

//--------------------------------------------------------------
void ofAppMain::keyPressed(int key){
	//if (key == 'n') {
	//	ofLogVerbose(ofSystemTextBoxDialog("Input URL", ""));
	//}
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
	_btnCalibrateForceSensor.removeListener(this, &ofAppMain::buttonPressed);
	_btnReqState_Reset.removeListener(this, &ofAppMain::buttonPressed);
	_btnReqState_Init.removeListener(this, &ofAppMain::buttonPressed);
	_btnReqState_Calibrate.removeListener(this, &ofAppMain::buttonPressed);
	_btnReqState_HomingAuto.removeListener(this, &ofAppMain::buttonPressed);
	_btnReqState_HomingManual.removeListener(this, &ofAppMain::buttonPressed);
	_btnReqState_Run.removeListener(this, &ofAppMain::buttonPressed);
	_btnEnableDrive.removeListener(this, &ofAppMain::buttonPressed);
	_btnDisableDrive.removeListener(this, &ofAppMain::buttonPressed);
	_btnExpLoad.removeListener(this, &ofAppMain::buttonPressed);
	_btnExpStart.removeListener(this, &ofAppMain::buttonPressed);
	_btnExpStop.removeListener(this, &ofAppMain::buttonPressed);
	_btnExpRestart.removeListener(this, &ofAppMain::buttonPressed);
	_btnToggleRecordData = false;
	_btnToggleRecordData.removeListener(this, &ofAppMain::recordDataTogglePressed);
	_btnExpPauseResume.removeListener(this, &ofAppMain::pauseExperimentTogglePressed);

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
		_systemState[0] = static_cast<SystemState>((int)data[0]);
		_systemState[1] = static_cast<SystemState>((int)data[1]);

		sprintf(buf, "[%d, %d]", _systemState[0], _systemState[1]);
		ofLogVerbose("System State: " + ofToString(buf));
		_lblSysState[0] = StringSystemStateLabel(_systemState[0]);
		_lblSysState[1] = StringSystemStateLabel(_systemState[1]);
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