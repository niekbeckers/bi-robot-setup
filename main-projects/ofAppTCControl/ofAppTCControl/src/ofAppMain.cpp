#include "ofAppMain.h"

//--------------------------------------------------------------
void ofAppMain::setup(){

	// set up window
	ofBackground(ofColor::blueSteel);
	ofSetWindowTitle("Control");

	// set up GUI
	setupGUI();

	// setup tcAdsClient
	setupTCADS();
	_timeCheck = ofGetElapsedTimef();
}

//--------------------------------------------------------------
void ofAppMain::update(){
	// read continuous ADS data
	_tcClientCont->read(_lHdlVar_Read_Data, &AdsData, sizeof(AdsData));

	display1->posCursorX = AdsData[0];
	display1->posCursorY = AdsData[1];
	display1->posTargetX = AdsData[5];
	display1->posTargetX = AdsData[6];



	// periodic check
	if (ofGetElapsedTimef() - _timeCheck > _timeRefreshCheck) {
		_timeCheck = ofGetElapsedTimef();

		if (_tcClientCont->nErr == 0) {\
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
	// draw gui
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

	// set up tcAdsClient for data reading
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

void ofAppMain::setupGUI()
{
	// setup GUIs
	_guiSystem.setup("System Control");
	_guiSystem.setPosition(10.0, 10.0);
	_guiExperiment.setup("Experiment");
	_guiExperiment.setPosition(250.0, 10.0);

	_guiSystem.setDefaultHeight(30);

	//
	// GUI System
	//
	
	_guiSystem.add(_lblEtherCAT.setup("EtherCAT/ADS", ""));
	_guiSystem.add(_lblFRM.set("Frame rate", ""));

	_ofGrpSys.setName("System state");
	_ofGrpSys.add(_lblSysState.set("System State", "[,]"));
	_ofGrpSys.add(_lblSysError.set("System Error", "[,]"));
	_ofGrpSys.add(_lblOpsEnabled.set("Drives Enabled", "[,]"));
	_guiSystem.add(_ofGrpSys);
	
	// request state
	_grpReqState.setup("Requested state");
	_grpReqState.setName("Request state");
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
	
	//
	// GUI EXPERIMENT
	//
	_guiExperiment.setDefaultHeight(30);
	_guiExperiment.add(_btnToggleRecordData.setup("Record data", false));

	//
	// add listeners
	//

	// buttons
	_btnReqState_Reset.addListener(this, &ofAppMain::ButtonPressed);
	_btnReqState_Init.addListener(this, &ofAppMain::ButtonPressed);
	_btnReqState_Calibrate.addListener(this, &ofAppMain::ButtonPressed);
	_btnReqState_HomingAuto.addListener(this, &ofAppMain::ButtonPressed);
	_btnReqState_HomingManual.addListener(this, &ofAppMain::ButtonPressed);
	_btnReqState_Run.addListener(this, &ofAppMain::ButtonPressed);
	_btnEnableDrive.addListener(this, &ofAppMain::ButtonPressed);
	_btnDisableDrive.addListener(this, &ofAppMain::ButtonPressed);

	// toggle
	_btnToggleRecordData.addListener(this, &ofAppMain::RecordDataTogglePressed);
}

//--------------------------------------------------------------
void ofAppMain::RequestStateChange(double reqState)
{
	// button event in GUI, most likely a request to the ADS (TwinCAT), hence set up client
	tcAdsClient* tcClient = new tcAdsClient(adsPort);

	// Request State
	char szVar[] = { "Object1 (ModelBaseBROS).ModelParameters.Requestedstate_Value" };
	long lHdlVar = tcClient->getVariableHandle(szVar, sizeof(szVar));

	// write state request to variable handle
	tcClient->write(lHdlVar, &reqState, sizeof(reqState));

	// clean up
	tcClient->disconnect();
}

//--------------------------------------------------------------
void ofAppMain::RequestDriveEnableDisable(bool enable)
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
void ofAppMain::ButtonPressed(const void * sender)
{
	ofxButton * button = (ofxButton*)sender;
	string clickedBtn = button->getName();
	
	if (clickedBtn.compare(ofToString("Reset")) == 0) {
		RequestStateChange(0.0);
	} 
	else if (clickedBtn.compare(ofToString("Init")) == 0) {
		RequestStateChange(1.0);
	}
	else if (clickedBtn.compare(ofToString("Calibrate")) == 0) {
		RequestStateChange(2.0);
	}
	else if (clickedBtn.compare(ofToString("Homing - Auto")) == 0) {
		RequestStateChange(302.0);
	}
	else if (clickedBtn.compare(ofToString("Homing - Manual")) == 0) {
		RequestStateChange(301.0);
	}
	else if (clickedBtn.compare(ofToString("Run")) == 0) {
		RequestStateChange(4.0);
	}
	else if (clickedBtn.compare(ofToString("Enable drives")) == 0) {
		RequestDriveEnableDisable(true);
	}
	else if (clickedBtn.compare(ofToString("Disable drives")) == 0) {
		RequestDriveEnableDisable(false);
	}


}

//--------------------------------------------------------------
void ofAppMain::RecordDataTogglePressed(bool & value)
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
void ofAppMain::keyPressed(int key){

}

//--------------------------------------------------------------
void ofAppMain::keyReleased(int key){

}

//--------------------------------------------------------------
void ofAppMain::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofAppMain::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofAppMain::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofAppMain::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofAppMain::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofAppMain::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofAppMain::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofAppMain::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofAppMain::dragEvent(ofDragInfo dragInfo){

}

//--------------------------------------------------------------
void ofAppMain::exit() {

	// remove listeners
	_btnReqState_Reset.removeListener(this, &ofAppMain::ButtonPressed);
	_btnReqState_Init.removeListener(this, &ofAppMain::ButtonPressed);
	_btnReqState_Calibrate.removeListener(this, &ofAppMain::ButtonPressed);
	_btnReqState_HomingAuto.removeListener(this, &ofAppMain::ButtonPressed);
	_btnReqState_HomingManual.removeListener(this, &ofAppMain::ButtonPressed);
	_btnReqState_Run.removeListener(this, &ofAppMain::ButtonPressed);
	_btnEnableDrive.removeListener(this, &ofAppMain::ButtonPressed);
	_btnDisableDrive.removeListener(this, &ofAppMain::ButtonPressed);

	_btnToggleRecordData = false;
	_btnToggleRecordData.removeListener(this, &ofAppMain::RecordDataTogglePressed);

	// disconnect ADS clients
	_tcClientCont->disconnect();
	_tcClientEvent->disconnect();
}

//--------------------------------------------------------------
void ofAppMain::HandleCallback(AmsAddr* pAddr, AdsNotificationHeader* pNotification)
{
	char buf[20];

	if (pNotification->hNotification == _lHdlNot_Read_OpsEnabled) {
		bool * data = (bool *)pNotification->data;
		sprintf(buf, "[%s,%s]", data[0] ? "T" : "F", data[1] ? "T" : "F");
		cout << "Drive Enabled: " << buf << '\n';
		_lblOpsEnabled = ofToString(buf);
	} 
	else if (pNotification->hNotification == _lHdlNot_Read_SystemError)  {
		double * data = (double *)pNotification->data;
		sprintf(buf, "[%d, %d]", (int)data[0], (int)data[1]);
		cout << "System Error: " << buf << '\n';
		_lblSysError = ofToString(buf);
	}
	else if (pNotification->hNotification == _lHdlNot_Read_SystemState) {
		double * data = (double *)pNotification->data;
		sprintf(buf, "[%d, %d]", (int)data[0], (int)data[1]);
		cout << "System State: " << buf << '\n';
		_lblSysState = ofToString(buf);
	}
	
	// print (to screen)) the value of the variable 
	cout << "Notification: " << pNotification->hNotification << " SampleSize: " << pNotification->cbSampleSize << '\n';
}

//--------------------------------------------------------------
void __stdcall onEventCallbackTCADS(AmsAddr* pAddr, AdsNotificationHeader* pNotification, ULONG hUser)
{
	// cast hUser to class pointer
	ofAppMain* ptr = (ofAppMain*)(hUser);

	// call HandleCallback for access to ofAppMain class
	ptr->HandleCallback(pAddr, pNotification);
}