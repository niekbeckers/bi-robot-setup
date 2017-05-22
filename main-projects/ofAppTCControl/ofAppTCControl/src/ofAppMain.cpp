#include "ofAppMain.h"

//--------------------------------------------------------------
void ofAppMain::setup(){

	// set up window
	ofBackground(ofColor::azure);
	ofSetWindowTitle("Control");

	// set up GUI
	setupGUI();

	// setup tcAdsClient
	setupTCADS();
}

//--------------------------------------------------------------
void ofAppMain::update(){
	// read continuous ADS data
	_tcClientCont->read(_lHdlVar_Read_Data, &AdsData, sizeof(AdsData));

	// frame rate in GUI
	lblFRM = ofToString((int)ofGetFrameRate()) + " fps";
}

//--------------------------------------------------------------
void ofAppMain::draw() {
	// draw gui
	guiSystem.draw(); 
	guiExperiment.draw();
}

void ofAppMain::setupTCADS()
{
	// set up tcAdsClient for data reading
	_tcClientCont = new tcAdsClient(adsPort);

	// get variable handles for ADS
	char szVar0[] = { "Object1 (ModelBaseBROS).Output.DataToADS" };
	_lHdlVar_Read_Data = _tcClientCont->getVariableHandle(szVar0, sizeof(szVar0));

	// set up tcAdsClient for data reading
	_tcClientEvent = new tcAdsClient(adsPort);

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

	guiSystem.setup("System Control");
	guiSystem.setPosition(10.0, 10.0);
	guiExperiment.setup("Experiment");
	guiExperiment.setPosition(250.0, 10.0);
	guiSystem.setDefaultHeight(30);

	//
	// GUI System
	//
	guiSystem.add(lblFRM.set("Frame rate", ""));

	ofParameterGroup ofGrpSys;
	ofGrpSys.add(lblSysState.set("System State", "[,]"));
	ofGrpSys.add(lblSysError.set("System Error", "[,]"));
	ofGrpSys.add(lblOpsEnabled.set("Drives Enabled", "[,]"));
	guiSystem.add(ofGrpSys);
	
	// request state
	ofParameter<string> sep1;
	guiSystem.add(sep1.set("Request State"));
	guiSystem.add(btnReqState_Reset.setup("Reset"));
	guiSystem.add(btnReqState_Init.setup("Init"));
	guiSystem.add(btnReqState_Calibrate.setup("Calibrate"));
	guiSystem.add(btnReqState_HomingAuto.setup("Homing - Auto"));
	guiSystem.add(btnReqState_HomingManual.setup("Homing - Manual"));
	guiSystem.add(btnReqState_Run.setup("Run"));

	// drive controls
	ofParameter<string> sep2;
	guiSystem.add(sep2.set("Drive Control"));
	guiSystem.add(btnEnableDrive.setup("Enable drives"));
	guiSystem.add(btnDisableDrive.setup("Disable drives"));

	
	//
	// GUI EXPERIMENT
	//
	guiExperiment.setDefaultHeight(30);
	guiExperiment.add(btnToggleRecordData.setup("Record data", false));

	// add listeners

	// buttons
	btnReqState_Reset.addListener(this, &ofAppMain::ButtonPressed);
	btnReqState_Init.addListener(this, &ofAppMain::ButtonPressed);
	btnReqState_Calibrate.addListener(this, &ofAppMain::ButtonPressed);
	btnReqState_HomingAuto.addListener(this, &ofAppMain::ButtonPressed);
	btnReqState_HomingManual.addListener(this, &ofAppMain::ButtonPressed);
	btnReqState_Run.addListener(this, &ofAppMain::ButtonPressed);
	btnEnableDrive.addListener(this, &ofAppMain::ButtonPressed);
	btnDisableDrive.addListener(this, &ofAppMain::ButtonPressed);

	// toggle
	btnToggleRecordData.addListener(this, &ofAppMain::RecordDataTogglePressed);
}

void ofAppMain::ButtonPressed(const void * sender)
{
	// button event in GUI, most likely a request to the ADS (TwinCAT), hence set up client
	tcAdsClient* tcClient = new tcAdsClient(adsPort);

	// Request State
	char szVar[] = { "Object1 (ModelBaseBROS).ModelParameters.Requestedstate_Value" };
	long lHdlVar_Write_ReqState = tcClient->getVariableHandle(szVar, sizeof(szVar));

	double reqState, val;
	
	ofxButton * button = (ofxButton*)sender;
	string clickedBtn = button->getName();
	
	if (clickedBtn.compare(ofToString("Reset")) == 0) {
		reqState = 0.0;
		tcClient->write(lHdlVar_Write_ReqState, &reqState, sizeof(reqState));
	} 
	else if (clickedBtn.compare(ofToString("Init")) == 0) {
		reqState = 1.0;
		tcClient->write(lHdlVar_Write_ReqState, &reqState, sizeof(reqState));
	}
	else if (clickedBtn.compare(ofToString("Calibrate")) == 0) {
		reqState = 2.0;
		tcClient->write(lHdlVar_Write_ReqState, &reqState, sizeof(reqState));
	}
	else if (clickedBtn.compare(ofToString("Homing - Auto")) == 0) {
		reqState = 302.0;
		tcClient->write(lHdlVar_Write_ReqState, &reqState, sizeof(reqState));
	}
	else if (clickedBtn.compare(ofToString("Homing - Manual")) == 0) {
		reqState = 301.0;
		tcClient->write(lHdlVar_Write_ReqState, &reqState, sizeof(reqState));
	}
	else if (clickedBtn.compare(ofToString("Run")) == 0) {
		reqState = 4.0;
		tcClient->write(lHdlVar_Write_ReqState, &reqState, sizeof(reqState));
	}
	else if (clickedBtn.compare(ofToString("Enable Drives")) == 0) {
		char szVar1[] = { "Object1 (ModelBaseBROS).ModelParameters.EnableDrives_Value" };
		long lHdlVar_Write_EnableDrive = tcClient->getVariableHandle(szVar1, sizeof(szVar1));
		val = 1.0;
		tcClient->write(lHdlVar_Write_EnableDrive, &val, sizeof(val));
		val = 0.0;
		tcClient->write(lHdlVar_Write_EnableDrive, &val, sizeof(val));
	}
	else if (clickedBtn.compare(ofToString("Disable Drives")) == 0) {
		char szVar1[] = { "Object1 (ModelBaseBROS).ModelParameters.DisableDrives_Value" };
		long lHdlVar_Write_DisableDrive = tcClient->getVariableHandle(szVar1, sizeof(szVar1));
		val = 1.0;
		tcClient->write(lHdlVar_Write_DisableDrive, &val, sizeof(val));
		val = 0.0;
		tcClient->write(lHdlVar_Write_DisableDrive, &val, sizeof(val));
	}

	// clean up
	tcClient->disconnect();
}

//--------------------------------------------------------------
void ofAppMain::RecordDataTogglePressed(bool & value)
{
	// button event in GUI, most likely a request to the ADS (TwinCAT), hence set up client
	tcAdsClient* tcClient = new tcAdsClient(adsPort);

	// Request State
	char szVar[] = { "Object1 (ModelBaseBROS).ModelParameters.Recorddata1yes0no_Value" };
	long lHdlVar_Write_RecData = tcClient->getVariableHandle(szVar, sizeof(szVar));

	double val;

	if (value) {
		val = 1.0;
		tcClient->write(lHdlVar_Write_RecData, &val, sizeof(val));
	}
	else {
		val = 0.0;
		tcClient->write(lHdlVar_Write_RecData, &val, sizeof(val));
	}

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

	// disconnect ADS clients
	_tcClientCont->disconnect();
	_tcClientEvent->disconnect();

	// remove listeners
	btnReqState_Reset.removeListener(this, &ofAppMain::ButtonPressed);
	btnReqState_Init.removeListener(this, &ofAppMain::ButtonPressed);
	btnReqState_Calibrate.removeListener(this, &ofAppMain::ButtonPressed);
	btnReqState_HomingAuto.removeListener(this, &ofAppMain::ButtonPressed);
	btnReqState_HomingManual.removeListener(this, &ofAppMain::ButtonPressed);
	btnReqState_Run.removeListener(this, &ofAppMain::ButtonPressed);
	btnEnableDrive.removeListener(this, &ofAppMain::ButtonPressed);
	btnDisableDrive.removeListener(this, &ofAppMain::ButtonPressed);

	btnToggleRecordData = false;
	btnToggleRecordData.removeListener(this, &ofAppMain::RecordDataTogglePressed);
}


void __stdcall onEventCallbackTCADS(AmsAddr* pAddr, AdsNotificationHeader* pNotification, ULONG hUser)
{
	// cast hUser to class pointer
	ofAppMain* ptr = (ofAppMain*)(hUser);

	// call HandleCallback for access to ofAppMain class
	ptr->HandleCallback(pAddr, pNotification);
}

void ofAppMain::HandleCallback(AmsAddr* pAddr, AdsNotificationHeader* pNotification)
{
	char buf[30];

	if (pNotification->hNotification == _lHdlNot_Read_OpsEnabled) {
		bool * data = (bool *)pNotification->data;
		sprintf(buf, "[%s,%s]", data[0] ? "true" : "false", data[1] ? "true" : "false");
		cout << "Operation Enabled: " << buf << '\n';
		lblOpsEnabled = ofToString(buf);
	} 
	else if (pNotification->hNotification == _lHdlNot_Read_SystemError)  {
		double * data = (double *)pNotification->data;
		sprintf(buf, "[%d, %d]", (int)data[0], (int)data[1]);
		cout << "System Error: " << buf << '\n';
		lblSysError = ofToString(buf);
	}
	else if (pNotification->hNotification == _lHdlNot_Read_SystemState) {
		double * data = (double *)pNotification->data;
		sprintf(buf, "[%d, %d]", (int)data[0], (int)data[1]);
		cout << "System State: " << buf << '\n';
		lblSysState = ofToString(buf);
	}
	
	// print (to screen)) the value of the variable 
	cout << "Notification: " << pNotification->hNotification << " SampleSize: " << pNotification->cbSampleSize << '\n';
}