#include "ofAppMain.h"

//--------------------------------------------------------------
void ofAppMain::setup(){

	// set up window
	ofBackground(0.50, 0.50, 0.50);
	ofSetWindowTitle("Control");
	ofSetFrameRate(240);

	// set up GUI
	setupGUI();

	// set up tcAdsClient for data reading
	_tcClientCont = new tcAdsClient(adsPort);

	// get variable handles for ADS
	char szVar0[] = { "Object1 (ModelBaseBROS).Output.DataToADS" };
	_lHdlVar_Read_Data = _tcClientCont->getVariableHandle(szVar0, sizeof(szVar0));

	// set up tcAdsClient for data reading
	_tcClientEvent = new tcAdsClient(adsPort);
	//char szVar[] = { "Object1 (ModelBaseBROS).BlockIO.VecCon_OpsEnabled" };
	char szVar1[] = { "Object1 (ModelBaseBROS).BlockIO.VecCon_SystemStates" };
	_lHdlVar_Read_SystemState = _tcClientEvent->getVariableHandle(szVar1, sizeof(szVar1));
	_lHdlNot_Read_DriveEnabled = _tcClientEvent->registerTCAdsDeviceNotification(_lHdlVar_Read_SystemState, (unsigned long)(this), onEventCallbackTCADS, 16);
}

//--------------------------------------------------------------
void ofAppMain::update(){

	// read continuous ADS data
	_tcClientCont->read(_lHdlVar_Read_Data, &_AdsData, sizeof(_AdsData));
}

//--------------------------------------------------------------
void ofAppMain::draw(){


	ofFill();
	double x = -_AdsData[0] * 2400.0 + ofGetScreenWidth() / 2.0;
	double y = (_AdsData[1] - 0.25) * 2400.0 + ofGetScreenHeight() / 2.0;

	ofSetColor(ofColor::yellow);
	ofDrawCircle(x, y, 10);
}

void ofAppMain::setupGUI()
{
	gui->addHeader("System states");
	gui->addFRM(0.5f); // add framerate monitor
	
	char buf[30];
	sprintf(buf, "System states:  [%d,%d]", 0, 0);
	guiLblSysState = gui->addLabel(buf);
	sprintf(buf, "System errors:  [%d,%d]", 0, 0);
	guiLblSysError = gui->addLabel(buf);
	sprintf(buf, "Operation Enabled:  [%d,%d]", 0, 0);
	guiLblOpsEnabled = gui->addLabel(buf);
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
}

void ofAppMain::onButtonEventReqState(ofxDatGuiButtonEvent e)
{
	// button event in GUI, most likely a request to the ADS (TwinCAT), hence set up client
	tcAdsClient* tcClient = new tcAdsClient(adsPort);
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
	double * data = (double *)pNotification->data;
	char buf[30];
	sprintf(buf, "System states:  [%d,%d]", (int)data[0], (int)data[1]);
	

	// print (to screen)) the value of the variable 
	cout << "Notification: " << pNotification->hNotification << " SampleSize: " << pNotification->cbSampleSize << '\n';
}