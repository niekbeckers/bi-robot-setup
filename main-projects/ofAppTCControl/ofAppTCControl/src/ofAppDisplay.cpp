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
	double xT = -mainApp->AdsData[5] * dots_per_m;
	double yT = mainApp->AdsData[6] * dots_per_m;

	ofSetColor(ofColor::darkSeaGreen);
	ofDrawCircle(xT, yT, 15.0);


	// draw cursor
	ofFill();
	double xC = -(mainApp->AdsData[0] - x0) * dots_per_m;
	double yC =  (mainApp->AdsData[1] - y0) * dots_per_m;

	ofSetColor(ofColor::darkCyan);
	ofDrawCircle(xC, yC, 10.0);

	ofPopMatrix();

}