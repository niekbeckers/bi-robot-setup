#include "ofAppDisplay.h"

//--------------------------------------------------------------
void ofAppDisplay::setup()
{
	ofBackground(25,25,25);
	ofSetWindowTitle("Display");

	int h = ofGetScreenHeight();
	int w = ofGetScreenWidth();

	dots_per_m = sqrt((double)(h * h) + (double)(w * w)) / scrDiagonal * 100.0 / 2.54; //dots per meter

	ofEnableAntiAliasing(); // enable anti-aliasing
}

//--------------------------------------------------------------
void ofAppDisplay::update()
{

}

//--------------------------------------------------------------
void ofAppDisplay::draw()
{
	// translate origin to middle of the screen
	ofTranslate(ofGetScreenWidth() / 2, ofGetScreenHeight() / 2);

	ofPushMatrix();
	
		// draw workspace boundary
		ofNoFill();
		ofSetLineWidth(4);
		ofSetColor(ofColor::lightGray);
		ofSetCircleResolution(80);
		ofDrawEllipse(0.0, 0.0, 2.0*(*pData).wsSemiMajor*dots_per_m, 2.0*(*pData).wsSemiMinor*dots_per_m);

		// draw target
		ofSetColor(ofColor::darkRed);
		ofSetLineWidth(4);
		ofSetCircleResolution(30);
		ofDrawCircle(-(*pData).posTargetX*dots_per_m, (*pData).posTargetY*dots_per_m, 15.0);

		// draw cursor
		ofFill();
		ofSetColor(ofColor::darkCyan);
		ofDrawCircle(-((*pData).posCursorX - x0)*dots_per_m, ((*pData).posCursorY - y0)*dots_per_m, 12.0);

	ofPopMatrix();

	// draw text

}

//--------------------------------------------------------------
void ofAppDisplay::windowResized(int w, int h) {
	dots_per_m = sqrt((double)(h * h) + (double)(w * w)) / scrDiagonal * 100.0 / 2.54; //dots per meter
}

//--------------------------------------------------------------
void showText(const string &text)
{
	
}

//--------------------------------------------------------------
void showText(int x, int y, const string &text)
{

}