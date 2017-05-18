#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	myClient = new tcClient(350);

	char szVar[] = { "Object1 (ModelBaseBROS).Output.DataToADS" };
	
	lHdlVar = myClient->getVariableHande(szVar, sizeof(szVar));
	myClient->Read(lHdlVar, &Data, sizeof(Data));

	for (int nIndex = 0; nIndex < 8; nIndex++)
		cout << "Data[" << nIndex << "]: " << Data[nIndex] << '\n';

	ofBackground(0.50, 0.50, 0.50);
	ofSetWindowTitle("graphics example");
	ofSetFrameRate(400); // if vertical sync is off, we can go a bit fast... this caps the framerate at 60fps.
}

//--------------------------------------------------------------
void ofApp::update(){

	myClient->Read(lHdlVar, &Data, sizeof(Data));

	std::stringstream strm;
	strm << "fps: " << ofGetFrameRate();
	ofSetWindowTitle(strm.str());
}

//--------------------------------------------------------------
void ofApp::draw(){
	ofFill();
	double x = -Data[0] * 2400.0 + ofGetScreenWidth() / 2.0;
	double y = (Data[1] - 0.25) * 2400.0 + ofGetScreenHeight() / 2.0;

	ofSetColor(ofColor::yellow);
	ofDrawCircle(x, y, 10);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

//--------------------------------------------------------------
void ofApp::exit() {
	myClient->Disconnect();
}
