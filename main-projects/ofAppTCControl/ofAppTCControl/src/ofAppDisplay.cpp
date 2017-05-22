#include "ofAppDisplay.h"

void ofAppDisplay::setup()
{
	ofBackground(0.50, 0.50, 0.50);
	ofSetWindowTitle("Display");
}

void ofAppDisplay::update()
{

}

void ofAppDisplay::draw()
{
	ofFill();
	ofDrawCircle(0.0, 10.0, 5.0);
}