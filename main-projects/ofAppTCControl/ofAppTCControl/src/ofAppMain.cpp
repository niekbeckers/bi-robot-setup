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
	_tcClientContRead = new tcAdsClient(adsPort);

	// get variable handles for ADS
	char szVar[] = { "Object1 (ModelBaseBROS).Output.DataToADS" };
	_lHdlVar_Read_Data = _tcClientContRead->getVariableHande(szVar, sizeof(szVar));
	//char szVar[] = { "Object1 (ModelBaseBROS).Output.DataToADS" };
	//_lHdlVar_Read_SystemState = _tcClient->getVariableHande(szVar, sizeof(szVar));

	//_tcClient->Read(_lHdlVar_Read_Data, &_AdsData, sizeof(_AdsData));
	//for (int nIndex = 0; nIndex < 8; nIndex++)
	//	cout << "Data[" << nIndex << "]: " << _AdsData[nIndex] << '\n';

}

//--------------------------------------------------------------
void ofAppMain::update(){

	// read continuous ADS data
	_tcClientContRead->Read(_lHdlVar_Read_Data, &_AdsData, sizeof(_AdsData));
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



	// Folder Request State buttons
	guiFldrReqSysState = gui->addFolder("Request System State", ofColor::aquamarine);
	guiFldrReqSysState->addBreak()->setHeight(10.0f);
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
	long lHdlVar_Write_ReqState = tcClient->getVariableHande(szVar, sizeof(szVar));
	double reqState;

	if (e.target->is("Init")) {
		reqState = 1.0;
		tcClient->Write(lHdlVar_Write_ReqState, &reqState, sizeof(reqState));
	}
	else if (e.target->is("Calibrate")) {
		reqState = 2.0;
		tcClient->Write(lHdlVar_Write_ReqState, &reqState, sizeof(reqState));
	}
	else if (e.target->is("Homing - Auto")) {
		reqState = 302.0;
		tcClient->Write(lHdlVar_Write_ReqState, &reqState, sizeof(reqState));
	}
	else if (e.target->is("Homing - Manual")) {
		reqState = 301.0;
		tcClient->Write(lHdlVar_Write_ReqState, &reqState, sizeof(reqState));
	}
	else if (e.target->is("Run")) {
		reqState = 4.0;
		tcClient->Write(lHdlVar_Write_ReqState, &reqState, sizeof(reqState));
	}

	// clean up
	tcClient->Disconnect();
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
	_tcClientContRead->Disconnect();
}
