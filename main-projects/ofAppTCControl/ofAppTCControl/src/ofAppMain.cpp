#include "ofAppMain.h"

//--------------------------------------------------------------
void ofAppMain::setup(){

	// set up window
	ofBackground(0.50, 0.50, 0.50);
	ofSetWindowTitle("Control");
	ofSetFrameRate(240);


	// set up GUI
	gui.setup();
	gui.add(button.setup("button"));

	// set up tcAdsClient for data reading
	_tcClient = new tcAdsClient(350);

	// get variable handles for ADS
	char szVar[] = { "Object1 (ModelBaseBROS).Output.DataToADS" };
	_lHdlVar_Read_Data = _tcClient->getVariableHande(szVar, sizeof(szVar));
	//_tcClient->Read(_lHdlVar_Read_Data, &_AdsData, sizeof(_AdsData));
	//for (int nIndex = 0; nIndex < 8; nIndex++)
	//	cout << "Data[" << nIndex << "]: " << _AdsData[nIndex] << '\n';

}

//--------------------------------------------------------------
void ofAppMain::update(){

	// read continuous ADS data
	_tcClient->Read(_lHdlVar_Read_Data, &_AdsData, sizeof(_AdsData));

	// show frame rate in window
	std::stringstream strm;
	strm << "fps: " << ofGetFrameRate();
	ofSetWindowTitle(strm.str());
}

//--------------------------------------------------------------
void ofAppMain::draw(){
	gui.draw();

	ofFill();
	double x = -_AdsData[0] * 2400.0 + ofGetScreenWidth() / 2.0;
	double y = (_AdsData[1] - 0.25) * 2400.0 + ofGetScreenHeight() / 2.0;

	ofSetColor(ofColor::yellow);
	ofDrawCircle(x, y, 10);
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
	_tcClient->Disconnect();
}
