#include "ofAppDisplay.h"

//--------------------------------------------------------------
void ofAppDisplay::setup()
{
	ofSetVerticalSync(false); // switch vsync off
	ofEnableAntiAliasing(); // enable anti-aliasing

	// define colors
	clrBackground = ofColor(18, 40, 47);
	clrCursor = ofColor(20, 138, 255);
	//clrTarget = ofColor(225, 31, 31);
	clrTarget = ofColor(153, 0, 0);
	clrWSBoundary = ofColor(198, 192, 173);
	clrText = ofColor::darkCyan;


	ofBackground(clrBackground); // background color
	ofSetWindowTitle("Display");

	int h = ofGetScreenHeight();
	int w = ofGetScreenWidth();

	dots_per_m = sqrt((double)(h * h) + (double)(w * w)) / scrDiagonal * 100.0 / 2.54; //dots per meter


	// font
	ofTrueTypeFont::setGlobalDpi(72);
	verdana50.load("verdana.ttf", 50, true, true);
	verdana50.setLineHeight(50.0f);
	verdana50.setLetterSpacing(1.035);
	_text.load("verdana.ttf", 50);

	verdana30.load("verdana.ttf", 30, true, true);
	verdana30.setLineHeight(30.0f);
	verdana30.setLetterSpacing(1.035);

	// setup cursor and target
	cursor.setColor(clrCursor);
	cursor.setFillMode(OF_FILLED);
	cursor.radius = 12.0f;

	target.setColor(clrTarget);
	target.setFillMode(OF_OUTLINE);
	target.radius = 15.0f;
}

//--------------------------------------------------------------
void ofAppDisplay::update()
{
	// update target and cursor
	target.setPosition(ofPoint(-(*pData).posTargetX*dots_per_m, (*pData).posTargetY*dots_per_m));
	target.update();
	cursor.setPosition(ofPoint(-((*pData).posCursorX - x0)*dots_per_m, ((*pData).posCursorY - y0)*dots_per_m));
	cursor.update();
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
		ofSetColor(clrWSBoundary);
		ofSetCircleResolution(80);
		ofDrawEllipse(0.0, 0.0, 2.0*(*pData).wsSemiMajor*dots_per_m, 2.0*(*pData).wsSemiMinor*dots_per_m);

		// draw target
		//ofSetColor(clrTarget);
		//ofSetLineWidth(6);
		//ofSetCircleResolution(60);
		//ofDrawCircle(-(*pData).posTargetX*dots_per_m, (*pData).posTargetY*dots_per_m, 15.0);
		target.draw();

		// draw cursor
		//ofFill();
		//ofSetColor(clrCursor);
		//ofDrawCircle(-((*pData).posCursorX - x0)*dots_per_m, ((*pData).posCursorY - y0)*dots_per_m, 12.0);
		cursor.draw();

		ofPopMatrix();
	}

	// draw message
	if (_showMessage) {
		ofPushMatrix();
		ofSetColor(clrText);
		ofRectangle bounds = verdana50.getStringBoundingBox(_message, 0, 0);
		//ofTranslate(-bounds.getCenter()[0], -( 0.4*ofGetScreenHeight() + bounds.getCenter()[1]));

		unsigned int flags = 0;
		flags |= ofxTextAlign::HORIZONTAL_ALIGN_CENTER;
		flags |= ofxTextAlign::VERTICAL_ALIGN_MIDDLE;
		ofTranslate(0.0, -0.4*ofGetScreenHeight());
		_text.draw(_message, 0.0, 0.0, flags);
		//verdana50.drawString(_message, 0.0, 0.0);
		ofPopMatrix();
	}

	// draw countdown
	if (_showCountDown) {
		// countdown bar
		ofPushMatrix();
		ofSetColor(clrText);
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