#include "ofAppDisplay.h"



//--------------------------------------------------------------
void ofAppDisplay::setup()
{
	
	ofSetVerticalSync(false); // switch vsync off
	//ofSetFrameRate(60);

	ofEnableAntiAliasing(); // enable anti-aliasing

	// define colors
	//clrBackground = ofColor(38, 139, 210);// solarized blue
	//clrBackground = ofColor(42, 161, 152); // solarized cyan
	clrBackground = ofColor(88, 110, 117); // solarized base01
	//clrBackground = ofColor(7, 54, 66); // solarized base02
	//clrBackground = ofColor(238, 232, 213); // solarized base3

	//clrCursor = ofColor(20, 138, 255);
	//clrCursor = ofColor(42, 161, 152);
	//clrCursor = ofColor(38, 139, 210); // solarized blue
	clrCursor = ofColor(198, 192, 173); // egg white

	//clrTarget = ofColor(225, 31, 31);
	//clrTarget = ofColor(153, 0, 0);
	//clrTarget = ofColor(203,75,22); // solarized orange
	//clrTarget = ofColor(220,50,47); // solarized red
	clrTarget = ofColor(227, 106, 36); // my orange

	clrWSBoundary = ofColor(198, 192, 173); // egg white
	clrText = ofColor(198, 192, 173);

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
	//_text.load("verdana.ttf", 50);

	verdana30.load("verdana.ttf", 30, true, true);
	verdana30.setLineHeight(30.0f);
	verdana30.setLetterSpacing(1.035);

	// setup display type
	displayType = DisplayType::PURSUIT;
	setDisplayType(displayType);

	// cursor shape
	//setCursorShape(ParticleShape::PARTICLESHAPE_CIRCLE);
	

	//virtualpartner.setColor(ofColor::forestGreen);
	//virtualpartner.setFillMode(OF_FILLED);
	//virtualpartner.radius = 12.0f;
}

//--------------------------------------------------------------
void ofAppDisplay::update()
{
	// update target and cursor
	target.setPosition(ofPoint(-(*pData).posTargetX*dots_per_m, (*pData).posTargetY*dots_per_m));
	target.update();
	cursor.setPosition(ofPoint(-((*pData).posCursorX - x0)*dots_per_m, ((*pData).posCursorY - y0)*dots_per_m));
	cursor.update();

	//if (drawVirtualPartner) {
	//	// update virtual partner
	//	virtualpartner.setPosition(ofPoint(-(*pData).posVPX*dots_per_m, (*pData).posVPY*dots_per_m));
	//	virtualpartner.update();
	//}
}

//--------------------------------------------------------------
void ofAppDisplay::draw()
{

	// translate origin to middle of the screen
	//ofTranslate(ofGetScreenWidth() / 2, ofGetScreenHeight() / 2);
	ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2);
	
	if (drawTask) {
		
		ofPoint p = ofPoint(100, 0);
		ofPoint p2 = ofPoint(-100, 0);
		float s = 24.0;

		switch (displayType) {
		case DisplayType::PURSUIT :
			ofPushMatrix();
			// draw workspace boundary
			ofNoFill();
			ofSetLineWidth(4);
			ofSetColor(clrWSBoundary);
			ofSetCircleResolution(240);
			ofDrawEllipse(0.0, 0.0, 2.0*(*pData).wsSemiMajor*dots_per_m, 2.0*(*pData).wsSemiMinor*dots_per_m);

			// draw target
			target.draw();

			// draw cursor
			cursor.update();
			cursor.draw();

			/*
			// test 1

			// new target
			ofSetLineWidth(6);
			//ofPushMatrix();
			//ofTranslate(_pos);
			ofSetColor(220, 50, 47);
			ofSetColor(211,54,130);  // solarized magenta
			ofSetColor(108,113,196); // solarized violet		
			ofSetColor(203,75,22); // solarized brred (orange)
			ofSetColor(4, 49, 255);
			ofSetColor(133, 153, 0);   // solarized green
			ofSetColor(42, 161, 152); // solarized cyan
			ofSetColor(181,137,0); // solarized yellow
			ofSetColor(227, 106, 36); // my orange
			//ofDrawLine(p2 + ofPoint(0.0, -s+12.0), p2 + ofPoint(0.0, s-12.0));
			//ofDrawLine(p2 + ofPoint( -s + 12.0,0.0), p2 + ofPoint(s - 12.0,0.0));
			ofFill();
			ofDrawCircle(p + ofPoint(5.0, 0.0), s-8.0);

			// cursor
			ofNoFill();
			//ofSetColor(38, 139, 210);  // solarized blue
			//ofSetColor(211, 54, 130);  // solarized magenta
			//ofSetColor(133, 153, 0);   // solarized green
			//ofSetColor(203, 75, 22); // solarized brred (orange)
			//ofSetColor(108, 113, 196); // solarized violet
			ofSetColor(ofColor(198, 192, 173));
			//ofSetLineWidth(1.0);
			
			//ofDrawLine(p + ofPoint(0.0, -s), p + ofPoint(0.0, s));
			ofSetLineWidth(6.0);
			ofSetCircleResolution(200);
			ofDrawCircle(p, s);
			ofDrawLine(p + ofPoint(0.0, -s), p + ofPoint(0.0, -s-0.75*s));
			ofDrawLine(p + ofPoint(0.0, s), p + ofPoint(0.0, s + 0.75*s));
			ofDrawLine(p + ofPoint(-s, 0.0), p + ofPoint(-s - 0.75*s, 0.0));
			ofDrawLine(p + ofPoint(s, 0.0), p + ofPoint(s + 0.75*s, 0.0));
			ofFill();
			ofDrawCircle(p, 4);

			*/


			

			ofPopMatrix();
			break;

		case DisplayType::PURSUIT_1D:
			ofPushMatrix();
			// draw workspace boundary
			//ofNoFill();
			//ofSetLineWidth(2);
			//ofSetColor(clrWSBoundary);
			//ofSetCircleResolution(80);
			//ofDrawEllipse(0.0, 0.0, 2.0*(*pData).wsSemiMajor*dots_per_m, 2.0*(*pData).wsSemiMinor*dots_per_m);
			//ofDrawLine(ofGetWidth() / 2.0, 0.0, -ofGetWidth() / 2.0, 0.0);
			
			// draw target
			target.draw();

			// draw cursor
			//cursor.update();
			cursor.draw();

			ofPopMatrix();
			break;

		case DisplayType::COMPENSATORY:
			ofPushMatrix();

			
			ofNoFill();

			// draw target (at 0,0)
			ofSetColor(clrTarget);
			//ofSetLineWidth(4.0);
			//int cursorheight = 80;
			//ofDrawLine(0.0, cursorheight / 2, 0.0, 120.0);
			//ofDrawLine(0.0, -cursorheight / 2, 0.0, -120.0);
			ofSetLineWidth(2.0);
			ofDrawLine(0.0, ofGetHeight() / 2.0, 0.0, -ofGetHeight() / 2.0);

			// draw cursor (error)
			ofSetLineWidth(4.0);
			ofSetColor(clrCursor);
			cursor.setPosition(ofPoint(target.getPosition()[0] - cursor.getPosition()[0], 0.0));
			cursor.update();
			cursor.draw();
			ofPopMatrix();
			break;
		}

			
	}
	
	// draw message
	if (_showMessageNorth) {
		ofPushMatrix();
			ofSetColor(clrText);
			ofRectangle bounds = verdana30.getStringBoundingBox(_messageNorth, 0, 0);
			ofTranslate(0, -0.425*ofGetHeight(), 0);
			verdana30.drawString(_messageNorth, -bounds.width / 2, bounds.height / 2);
		ofPopMatrix();
	}

	if (_showMessageCenter) {
		ofPushMatrix();
			ofSetColor(clrText);
			ofRectangle bounds = verdana30.getStringBoundingBox(_messageCenter, 0, 0);
			//ofTranslate(bounds.width / 2, bounds.height / 2, 0);
			verdana30.drawString(_messageCenter, -bounds.width / 2, bounds.height / 2);
		ofPopMatrix();
	}

	// draw countdown
	if (_showCountDown) {
		// countdown bar
		ofPushMatrix();

			_cdBarPosition = ofPoint(0.0 - _cdBarWidth / 2.0, -(0.325*ofGetHeight() + _cdBarHeight / 2.0));
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
void ofAppDisplay::showMessageNorth(bool show, const string &msg)
{
	_messageNorth = msg;
	_showMessageNorth = show;
}

//--------------------------------------------------------------
void ofAppDisplay::showMessageCenter(bool show, const string &msg)
{
	_messageCenter = msg;
	_showMessageCenter = show;
}

//--------------------------------------------------------------
void ofAppDisplay::showCountDown(bool show, double timeRemaining, double duration)
{
	_showCountDown = show;
	_cdTimeRemaining = timeRemaining;
	_cdDuration = duration;
}

void ofAppDisplay::setDisplayType(DisplayType dtype)
{
	displayType = dtype;

	switch (displayType) {
	case DisplayType::PURSUIT:

		// setup cursor and target
		cursor.setColor(clrCursor);
		//cursor.setFillMode(OF_OUTLINE);
		cursor.radius = 24.0f;
		cursor.setShape(ParticleShape::PARTICLESHAPE_CROSSHAIR);
		cursor.reset();

		//target.setMode(PARENTPARTICLE_MODE_CLOUD);
		//target.setMode(PARENTPARTICLE_MODE_NORMAL);
		target.setColor(clrTarget);
		target.setFillMode(OF_FILLED);
		target.radius = 16.0f;
		
		break;

	case DisplayType::PURSUIT_1D:

		// setup cursor and target
		cursor.setColor(clrCursor);
		cursor.setFillMode(OF_OUTLINE);
		cursor.radius = 24.0f;
		cursor.setShape(ParticleShape::PARTICLESHAPE_CROSSHAIR);
		cursor.reset();

		//target.setMode(PARENTPARTICLE_MODE_CLOUD);
		//target.setMode(PARENTPARTICLE_MODE_NORMAL);
		target.setColor(clrTarget);
		target.setFillMode(OF_FILLED);
		target.radius = 16.0f;
		target.reset();
		break;

	case DisplayType::COMPENSATORY:
		// setup cursor and target
		cursor.setColor(clrCursor);
		cursor.setFillMode(OF_FILLED);
		cursor.radius = 40.0f;
		cursor.setShape(ParticleShape::PARTICLESHAPE_LINE);
		cursor.reset();

		//target.setMode(PARENTPARTICLE_MODE_CLOUD);
		//target.setMode(PARENTPARTICLE_MODE_NORMAL);
		target.setColor(clrTarget);
		target.setFillMode(OF_OUTLINE);
		target.radius = 15.0f;
		target.reset();
		break;
	};
}