#include "ofDisplayApp.h"



void ofDisplayApp::setup()
{
	ofBackground(0.50, 0.50, 0.50);
	ofSetWindowTitle("Display");
}


void ofDisplayApp::update()
{

}

void ofDisplayApp::draw()
{
	ofFill();
	ofDrawCircle(0.0, 10.0, 5.0);
}