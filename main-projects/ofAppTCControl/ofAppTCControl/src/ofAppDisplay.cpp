#include "ofAppDisplay.h"

void ofAppDisplay::setup()
{
	ofBackground(0.50, 0.50, 0.50);
	ofSetWindowTitle("Display");

	int scrHeight = ofGetScreenHeight();
	int scrWidth = ofGetScreenWidth();

	dots_per_m = sqrt((double)(scrHeight * scrHeight) + (double)(scrWidth * scrWidth)) / scrDiagonal * 100.0 / 2.54; //dots per meter
}

void ofAppDisplay::update()
{

}

void ofAppDisplay::draw()
{
	ofFill();

	// draw target
	double xT = -mainApp->AdsData[5] * dots_per_m + ofGetScreenWidth() / 2;
	double yT = (double)ofGetScreenHeight() + mainApp->AdsData[6] * dots_per_m - (double)ofGetScreenHeight() / 2.0;

	ofSetColor(ofColor::darkSeaGreen);
	ofDrawCircle(xT, yT, 15.0);


	// draw cursor
	double xC = -(mainApp->AdsData[0] - x0) * dots_per_m + ofGetScreenWidth()/2;
	double yC = (double)ofGetScreenHeight() + (mainApp->AdsData[1]-y0) * dots_per_m - (double)ofGetScreenHeight() / 2.0;

	ofSetColor(ofColor::darkCyan);
	ofDrawCircle(xC, yC, 10.0);



}