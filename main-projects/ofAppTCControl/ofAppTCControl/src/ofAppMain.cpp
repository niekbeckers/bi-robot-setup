#include "ofAppMain.h"

//--------------------------------------------------------------
void ofAppMain::setup(){

	// set up window
	ofBackground(0.50, 0.50, 0.50);
	ofSetWindowTitle("Control");
	ofSetFrameRate(200);

	// set up GUI
	setupGUI();

	// setup tcAdsClient
	setupTCADS();
	
}

//--------------------------------------------------------------
void ofAppMain::update(){

	// read continuous ADS data
	_tcClientCont->read(_lHdlVar_Read_Data, &_AdsData, sizeof(_AdsData));

	// update gui
	updateGUI();
	
}

//--------------------------------------------------------------
void ofAppMain::draw(){


	ofFill();
	double x = -_AdsData[0] * 2400.0 + ofGetScreenWidth() / 2.0;
	double y = (_AdsData[1] - 0.25) * 2400.0 + ofGetScreenHeight() / 2.0;

	ofSetColor(ofColor::yellow);
	ofDrawCircle(x, y, 10);

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
	gui->addHeader("System Control");
	gui->addFRM(0.5f); // add framerate monitor

	// Add system state, error, drive enabled labels
	_sSystemState = "[,]"; _sSystemStateNew = _sSystemState;
	guiLblSysState = gui->addTextInput("System State", _sSystemState);
	guiLblSysState->setStripeColor(ofColor::red);

	_sSystemError = "[,]"; _sSystemErrorNew = _sSystemError;
	guiLblSysError = gui->addTextInput("System Error", _sSystemError);
	guiLblSysError->setStripeColor(ofColor::red);

	_sOpsEnabled = "[,]"; _sOpsEnabledNew = _sOpsEnabled;
	guiLblOpsEnabled = gui->addTextInput("Drive Enabled", _sOpsEnabled); 
	guiLblOpsEnabled->setStripeColor(ofColor::red);

	gui->addBreak()->setHeight(10.0f);

	// Folder Request State buttons
	guiFldrReqSysState = gui->addFolder("Request System State", ofColor::aquamarine);
	guiFldrReqSysState->addBreak()->setHeight(5.0f);
	guiFldrReqSysState->addButton("Reset");
	guiFldrReqSysState->addBreak()->setHeight(5.0f);
	guiFldrReqSysState->addButton("Init");
	guiFldrReqSysState->addBreak()->setHeight(5.0f);
	guiFldrReqSysState->addButton("Calibrate");
	guiFldrReqSysState->addBreak()->setHeight(5.0f);
	guiFldrReqSysState->addButton("Homing - Auto");
	guiFldrReqSysState->addBreak()->setHeight(5.0f);
	guiFldrReqSysState->addButton("Homing - Manual");
	guiFldrReqSysState->addBreak()->setHeight(5.0f);
	guiFldrReqSysState->addButton("Run");
	guiFldrReqSysState->expand();
	guiFldrReqSysState->onButtonEvent(this, &ofAppMain::onButtonEventReqState); // connect custom event for req system state

	gui->addBreak()->setHeight(10.0f);
	guiFldrMotorControl = gui->addFolder("Motor Control", ofColor::forestGreen);
	guiFldrMotorControl->addBreak()->setHeight(5.0f);
	guiFldrMotorControl->addButton("Enable Drive");
	guiFldrMotorControl->addBreak()->setHeight(5.0f);
	guiFldrMotorControl->addButton("Disable Drive");
	guiFldrMotorControl->expand();
	guiFldrMotorControl->onButtonEvent(this, &ofAppMain::onButtonEventReqState);

	gui->addFooter();
}

void ofAppMain::updateGUI()
{
	if (_sSystemError.compare(_sSystemErrorNew) != 0) {
		_sSystemError = _sSystemErrorNew;
		guiLblSysError->setText(_sSystemError);
	}
	if (_sSystemState.compare(_sSystemStateNew) != 0) {
		_sSystemState = _sSystemStateNew;
		guiLblSysState->setText(_sSystemState);
	}
	if (_sOpsEnabled.compare(_sOpsEnabledNew) != 0) {
		_sOpsEnabled = _sOpsEnabledNew;
		guiLblOpsEnabled->setText(_sOpsEnabled);
	}
}

void ofAppMain::onButtonEventReqState(ofxDatGuiButtonEvent e)
{
	// button event in GUI, most likely a request to the ADS (TwinCAT), hence set up client
	tcAdsClient* tcClient = new tcAdsClient(adsPort);

	// Request State
	char szVar[] = { "Object1 (ModelBaseBROS).ModelParameters.Requestedstate_Value" };
	long lHdlVar_Write_ReqState = tcClient->getVariableHandle(szVar, sizeof(szVar));

	double reqState;

	if (e.target->is("Reset")) {
		reqState = 0.0;
		tcClient->write(lHdlVar_Write_ReqState, &reqState, sizeof(reqState));
	}
	else if(e.target->is("Init")) {
		reqState = 1.0;
		tcClient->write(lHdlVar_Write_ReqState, &reqState, sizeof(reqState));
	}
	else if (e.target->is("Calibrate")) {
		reqState = 2.0;
		tcClient->write(lHdlVar_Write_ReqState, &reqState, sizeof(reqState));
	}
	else if (e.target->is("Homing - Auto")) {
		reqState = 302.0;
		tcClient->write(lHdlVar_Write_ReqState, &reqState, sizeof(reqState));
	}
	else if (e.target->is("Homing - Manual")) {
		reqState = 301.0;
		tcClient->write(lHdlVar_Write_ReqState, &reqState, sizeof(reqState));
	}
	else if (e.target->is("Run")) {
		reqState = 4.0;
		tcClient->write(lHdlVar_Write_ReqState, &reqState, sizeof(reqState));
	}

	// Motor Control
	double val;
	if (e.target->is("Enable Drive")) {
		char szVar1[] = { "Object1 (ModelBaseBROS).ModelParameters.EnableDrives_Value" };
		long lHdlVar_Write_EnableDrive = tcClient->getVariableHandle(szVar1, sizeof(szVar1));
		val = 1.0;
		tcClient->write(lHdlVar_Write_EnableDrive, &val, sizeof(val));
		val = 0.0;
		tcClient->write(lHdlVar_Write_EnableDrive, &val, sizeof(val));
	}
	else if (e.target->is("Disable Drive")) {
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
	_tcClientCont->disconnect();
	_tcClientEvent->disconnect();
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
		_sOpsEnabledNew = ofToString(buf);
	} 
	else if (pNotification->hNotification == _lHdlNot_Read_SystemError)  {
		double * data = (double *)pNotification->data;
		sprintf(buf, "[%d, %d]", (int)data[0], (int)data[1]);
		cout << "System Error: " << buf << '\n';
		_sSystemErrorNew = ofToString(buf);
	}
	else if (pNotification->hNotification == _lHdlNot_Read_SystemState) {
		double * data = (double *)pNotification->data;
		sprintf(buf, "[%d, %d]", (int)data[0], (int)data[1]);
		cout << "System State: " << buf << '\n';
		_sSystemStateNew = ofToString(buf);
	}
	
	// print (to screen)) the value of the variable 
	cout << "Notification: " << pNotification->hNotification << " SampleSize: " << pNotification->cbSampleSize << '\n';
}