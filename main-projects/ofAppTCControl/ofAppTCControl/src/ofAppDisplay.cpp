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

	// font
	ofTrueTypeFont::setGlobalDpi(72);
	verdana50.load("verdana.ttf", 50, true, true);
	verdana50.setLineHeight(50.0f);
	verdana50.setLetterSpacing(1.035);

	verdana30.load("verdana.ttf", 30, true, true);
	verdana30.setLineHeight(30.0f);
	verdana30.setLetterSpacing(1.035);
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

	if (drawTask) {
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
	}

	// draw message
	if (_showMessage) {
		ofPushMatrix();
		ofRectangle bounds = verdana50.getStringBoundingBox(_message, 0, 0);
		ofTranslate(-bounds.getCenter()[0], -( 0.4*ofGetScreenHeight() + bounds.getCenter()[1]));
		verdana50.drawString(_message, 0.0, 0.0);
		ofPopMatrix();
	}

	// draw countdown
	if (_showCountDown) {
		// countdown bar
		ofPushMatrix();
		ofNoFill();
		ofDrawRectangle(_cdBarPosition, _cdBarWidth, _cdBarHeight);
		ofFill();
		ofPoint p = _cdBarPosition + ofPoint(_cdBarWidth, _cdBarHeight);
		double w = _cdBarWidth*(_cdTimeRemaining / _cdDuration);
		ofDrawRectangle(p, -w, -_cdBarHeight);
		
		// text remaining time
		string msg = ofToString(ceil(_cdTimeRemaining)) + " s";
		ofRectangle bounds2 = verdana30.getStringBoundingBox(msg, 0, 0);
		verdana30.drawString(msg, p[0]+20.0, p[1] + bounds2.getCenter()[1]);
		ofPopMatrix();
	}

}

//--------------------------------------------------------------
void ofAppDisplay::windowResized(int w, int h) 
{
	dots_per_m = sqrt((double)(h * h) + (double)(w * w)) / scrDiagonal * 100.0 / 2.54; //dots per meter
}

//--------------------------------------------------------------
void ofAppDisplay::showMessage(bool show)
{
	_showMessage = show;
}

//--------------------------------------------------------------
void ofAppDisplay::showMessage(bool show, const string &msg)
{
	setMessage(msg);
	showMessage(show);
}

//--------------------------------------------------------------
void ofAppDisplay::setMessage(const string &msg)
{
	_message = msg;
}

void ofAppDisplay::showCountDown(bool show, double timeRemaining, double duration)
{
	_showCountDown = show;
	_cdTimeRemaining = timeRemaining;
	_cdDuration = duration;
}