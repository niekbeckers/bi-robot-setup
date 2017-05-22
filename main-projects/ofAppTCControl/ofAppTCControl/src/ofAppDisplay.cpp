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
	// set cursor
	double x = -mainApp->AdsData[0] * dots_per_m + ofGetScreenWidth()/2;
	double y = (double)ofGetScreenHeight() + (mainApp->AdsData[1]-0.25) * dots_per_m - (double)ofGetScreenHeight() / 2.0;

	ofFill();
	ofDrawCircle(x, y, 10.0);
}