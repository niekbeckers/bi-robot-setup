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
	
	ofTranslate(ofGetScreenWidth() / 2, ofGetScreenHeight() / 2);

	// draw target
	ofNoFill();
	ofSetColor(ofColor::darkSeaGreen);
	ofDrawCircle(-posTargetX*dots_per_m, posTargetY*dots_per_m, 15.0);


	// draw cursor
	ofFill();
	ofSetColor(ofColor::darkCyan);
	ofDrawCircle(-(posCursorX-x0)*dots_per_m, (posCursorY-y0)*dots_per_m, 10.0);

	ofPopMatrix();

}