#include "ofAppDisplay.h"

//--------------------------------------------------------------
void ofAppDisplay::setup()
{
	ofBackground(0.50, 0.50, 0.50);
	ofSetWindowTitle("Display");

	int scrHeight = ofGetScreenHeight();
	int scrWidth = ofGetScreenWidth();

	dots_per_m = sqrt((double)(scrHeight * scrHeight) + (double)(scrWidth * scrWidth)) / scrDiagonal * 100.0 / 2.54; //dots per meter
}

//--------------------------------------------------------------
void ofAppDisplay::update()
{

}

//--------------------------------------------------------------
void ofAppDisplay::draw()
{
	ofPushMatrix();
	
	// translate origin to middle of the screen
	ofTranslate(ofGetScreenWidth() / 2, ofGetScreenHeight() / 2);

	// draw workspace boundary
	ofNoFill();
	ofSetLineWidth(4);
	ofSetColor(ofColor::lightGray);
	ofSetCircleResolution(50);
	//ofDrawCircle(0.0, 0.0, (*pData).wsSemiMajor*dots_per_m);
	ofDrawEllipse(0.0, 0.0, 2.0*(*pData).wsSemiMajor*dots_per_m, 2.0*(*pData).wsSemiMinor*dots_per_m);

	// draw target
	ofSetColor(ofColor::darkRed);
	ofSetLineWidth(3);
	ofSetCircleResolution(30);
	ofDrawCircle(-(*pData).posTargetX*dots_per_m, (*pData).posTargetY*dots_per_m, 15.0);

	// draw cursor
	ofFill();
	ofSetColor(ofColor::darkCyan);
	ofDrawCircle(-((*pData).posCursorX - x0)*dots_per_m, ((*pData).posCursorY - y0)*dots_per_m, 12.0);

	ofPopMatrix();

}