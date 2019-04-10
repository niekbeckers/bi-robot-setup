#include "ofAppDisplay.h"



//--------------------------------------------------------------
void ofAppDisplay::setup()
{
	
	ofSetVerticalSync(false); // switch vsync off
	ofSetFrameRate(120);

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
	//clrCursor = ofColor(198, 192, 173); // egg white
	clrCursor = ofColor(255, 255, 255);
	cursor.setColor(clrCursor);

	//clrTarget = ofColor(225, 31, 31);
	//clrTarget = ofColor(153, 0, 0);
	//clrTarget = ofColor(203,75,22); // solarized orange
	//clrTarget = ofColor(220,50,47); // solarized red
	clrTarget = ofColor(227, 106, 36); // my orange
	clrTarget = ofColor(0, 0, 0);
	target.setColor(clrTarget);

	clrWSBoundary = ofColor(198, 192, 173); // egg white
	clrText = clrCursor;

	ofBackground(clrBackground); // background color
	ofSetWindowTitle("Display");

	int h = ofGetScreenHeight();
	int w = ofGetScreenWidth();

	dots_per_m = sqrt((double)(h * h) + (double)(w * w)) / scrDiagonal * 100.0 / 2.54; //dots per meter

	// font
	ofTrueTypeFont::setGlobalDpi(72);
	verdana20.load("verdana.ttf", 20, true, true);
	verdana20.setLineHeight(30.0f);
	verdana20.setLetterSpacing(1.035);

	verdana30.load("verdana.ttf", 30, true, true);
	verdana30.setLineHeight(40.0f);
	verdana30.setLetterSpacing(1.035);

	// setup display type
	displayType = DisplayType::PURSUIT_2D;
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
		

		// countdown for task time
		if (_drawTrialTimeCountdown && experimentApp->experimentIsLoaded()) {
			ofPushMatrix();
			// translate back to top-left corner
			ofTranslate(-ofGetWidth() / 2, ofGetHeight() / 2);
			float trialDuration = experimentApp->getCurrentTrial().trialDuration;
			float trialTime = (float)mainApp->getTrialTime();
			if (trialDuration > 0.0) {
				float r = 1.0 - ((trialDuration - trialTime) / trialDuration);
				ofSetLineWidth(6.0);
				ofSetColor(255, 255, 255);
				ofDrawLine(0.0, 0.0, r*ofGetWidth(), 0.0);
			}
			ofPopMatrix();
		}

		switch (displayType) {
		case DisplayType::PURSUIT_2D :
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
			cursor.draw();
			
			ofPopMatrix();
			break;

		case DisplayType::PURSUIT_1D:
			ofPushMatrix();
			
			ofNoFill();

			// draw target (at 0,0)
			ofSetColor(clrTarget);
			ofSetLineWidth(2.0);
			ofDrawLine(target.getPosition()[0], ofGetHeight() / 2.0, target.getPosition()[0], -ofGetHeight() / 2.0);

			// draw cursor (error)
			ofSetLineWidth(8.0);
			ofSetColor(clrCursor);
			cursor.radius = 160.0f;
			cursor.setPosition(ofPoint(cursor.getPosition()[0], 0.0)); // note the negation!
			cursor.update();
			cursor.draw();

			ofPopMatrix();
			break;

		case DisplayType::COMPENSATORY_1D:
			ofPushMatrix();

			ofNoFill();

			// draw target (at 0,0)
			ofSetColor(clrTarget);
			ofSetLineWidth(2.0);
			ofDrawLine(0.0, ofGetHeight() / 2.0, 0.0, -ofGetHeight() / 2.0);

			// draw cursor (error)
			ofSetLineWidth(8.0);
			ofSetColor(clrCursor);
			
			cursor.setPosition(-ofPoint(target.getPosition()[0] - cursor.getPosition()[0], 0.0)); // note the negation!
			cursor.update();
			cursor.draw();
			ofPopMatrix();
			break;

		case DisplayType::COMPENSATORY_2D:
			ofPushMatrix();

			// set cursor position (error)
			cursor.setPosition(-(target.getPosition() - cursor.getPosition())); // note the negation!
			

			// draw target at (0,0)
			target.setPosition(ofPoint(0.0, 0.0));
			target.update();
			target.draw();

			// draw cursor
			cursor.update();
			cursor.draw();

			ofPopMatrix();
			break;

		case DisplayType::PURSUIT_ROLL:

			ofFill();

			// horizon
			ofPushMatrix();
			ofRotateRad(-0.005*cursor.getPosition()[0]);
			ofSetColor(0, 197, 255); // "sky"
			ofDrawRectangle(-ofGetWidth(), 0.0, 2.0*ofGetWidth(), -2.0*ofGetHeight());
			ofSetColor(88, 110, 117); // "ground color"
			ofDrawRectangle(-ofGetWidth(), 0.0, 2.0*ofGetWidth(), 2.0*ofGetHeight());

			// target
			ofPushMatrix();
			ofRotateRad(0.005*target.getPosition()[0]);
			ofSetColor(0, 0, 0);
			ofSetLineWidth(3.0);
			ofDrawLine(-ofGetWidth(), 0.0, ofGetWidth(), 0.0);
			ofPopMatrix();

			ofPopMatrix();


			// attitude indicator
			ofPushMatrix();
			ofSetColor(255, 255, 255); // white
			ofDrawRectangle(-12.0, -4.0, 24.0, 8.0); // dot in the middle
			// left part
			ofDrawRectangle(-180.0, -4.0, 140.0, 8.0);
			ofDrawRectangle(-44.0, -4.0, 8.0, 30.0);
			// right part
			ofDrawRectangle(180.0, -4.0, -140.0, 8.0);
			ofDrawRectangle(44.0, -4.0, -8.0, 30.0);

			ofPopMatrix();
			break;

		case DisplayType::COMPENSATORY_ROLL:

			ofFill();

			// horizon
			ofPushMatrix();
			ofRotateRad(-0.005*(target.getPosition()[0]+cursor.getPosition()[0]));
			ofSetColor(0, 197, 255); // "sky"
			ofDrawRectangle(-ofGetWidth(), 0.0, 2.0*ofGetWidth(), -2.0*ofGetHeight());
			ofSetColor(88, 110, 117); // "ground color"
			ofDrawRectangle(-ofGetWidth(), 0.0, 2.0*ofGetWidth(), 2.0*ofGetHeight());

			// target
			ofPushMatrix();
			ofSetColor(0, 0, 0);
			ofSetLineWidth(3.0);
			ofDrawLine(-ofGetWidth(), 0.0, ofGetWidth(), 0.0);
			ofPopMatrix();
			ofPopMatrix();


			// attitude indicator
			ofPushMatrix();
			ofSetColor(255, 255, 255); // white
			ofDrawRectangle(-12.0, -4.0, 24.0, 8.0); // dot in the middle
													 // left part
			ofDrawRectangle(-180.0, -4.0, 140.0, 8.0);
			ofDrawRectangle(-44.0, -4.0, 8.0, 30.0);
			// right part
			ofDrawRectangle(180.0, -4.0, -140.0, 8.0);
			ofDrawRectangle(44.0, -4.0, -8.0, 30.0);

			ofPopMatrix();
			break;
		}

			
	}
	
	// draw message
	if (_showMessageNorth) {
		ofPushMatrix();
			ofSetColor(clrText);
			ofRectangle bounds = verdana30.getStringBoundingBox(_messageNorth, 0, 0);
			ofTranslate(0, -0.435*ofGetHeight(), 0);
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

	if (_showMessageNW) {
		ofPushMatrix();
		ofSetColor(clrText);
		ofRectangle bounds = verdana30.getStringBoundingBox(_messageNW, 0, 0);
		ofTranslate(-ofGetScreenWidth()/2.0+40.0, -ofGetScreenHeight()/2.0+40.0, 0);
		verdana30.drawString(_messageNW, 0.0, bounds.height/2.0);
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
void ofAppDisplay::showMessageNorthWest(bool show, const string &msg)
{
	_messageNW = msg;
	_showMessageNW = show;
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
	case DisplayType::PURSUIT_2D:

		// setup cursor and target
		cursor.setColor(clrCursor);
		//cursor.setFillMode(OF_OUTLINE);
		cursor.radius = 20.0f;
		cursor.setShape(ParticleShape::PARTICLESHAPE_CROSSHAIR);
		cursor.reset();

		//target.setMode(PARENTPARTICLE_MODE_CLOUD);
		//target.setMode(PARENTPARTICLE_MODE_NORMAL);
		target.setColor(clrTarget);
		target.setFillMode(OF_FILLED);
		target.radius = 16.0f;
		target.reset();
		break;

	case DisplayType::PURSUIT_1D:

		// setup cursor and target
		cursor.setColor(clrCursor);
		cursor.setFillMode(OF_OUTLINE);
		cursor.radius = 24.0f;
		cursor.setShape(ParticleShape::PARTICLESHAPE_LINE);
		cursor.reset();

		//target.setMode(PARENTPARTICLE_MODE_CLOUD);
		//target.setMode(PARENTPARTICLE_MODE_NORMAL);
		target.setColor(clrTarget);
		target.setFillMode(OF_FILLED);
		target.radius = 16.0f;
		target.reset();
		break;

	case DisplayType::COMPENSATORY_1D:
		// setup cursor and target
		cursor.setColor(clrCursor);
		cursor.setFillMode(OF_FILLED);
		cursor.radius = 160.0f;
		cursor.setShape(ParticleShape::PARTICLESHAPE_LINE);
		cursor.reset();

		//target.setMode(PARENTPARTICLE_MODE_CLOUD);
		//target.setMode(PARENTPARTICLE_MODE_NORMAL);
		target.setColor(clrTarget);
		target.setFillMode(OF_OUTLINE);
		target.radius = 15.0f;
		target.reset();
		break;

	case DisplayType::COMPENSATORY_2D:

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
		target.radius = 20.0f;
		target.reset();
		break;
	};

}