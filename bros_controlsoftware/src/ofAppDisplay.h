#pragma once
#define _USE_MATH_DEFINES
#include <cmath>

#include "ofMain.h"
#include "ofUtils.h"
#include "parentParticle.h"
#include "myCommon.h"
#include "ofAppMain.h"
#include "ofAppExperiment.h"

class ofAppMain;
class ofAppExperiment;

class ofAppDisplay : public ofBaseApp
{


	private:
		//
		// variables
		//
		const double scrDiagonal = 27.0;
		double dots_per_m;
		double x0 = 0.0;
		double y0 = 0*0.25;

		// colors
		ofColor clrBackground, clrCursor, clrTarget, clrWSBoundary, clrText;
		
		// message & font
		string _messageNorth = "";
		bool _showMessageNorth = false;
		string _messageCenter = "";
		bool _showMessageCenter = false;
		string _messageNW = "";
		bool _showMessageNW = false;

		ofTrueTypeFont verdana20;
		ofTrueTypeFont verdana30;

		//ofxTextAlignTTF _text;

		// countdown
		bool _showCountDown = false;
		double _cdDuration = 3.0;
		double _cdTimeRemaining = 1.0;
		double _cdBarWidth = 200.0;
		double _cdBarHeight = 40.0;
		ofPoint _cdBarPosition = { 0.0 - _cdBarWidth / 2.0, -(0.35*ofGetHeight() + _cdBarHeight / 2.0) };

		bool _drawTrialTimeCountdown = false;
	public:

		//
		// variables
		//
		displayData *pData;
		bool drawTask = true;
		bool drawVirtualPartner = false;

		parentParticle cursor;
		parentParticle target;
		//parentParticle virtualpartner;

		DisplayType displayType = DisplayType::PURSUIT_2D;

		shared_ptr<ofAppMain> mainApp;
		shared_ptr<ofAppExperiment> experimentApp;

		//
		// openFrameworks
		//
		void setup();
		void update();
		void draw();
		void windowResized(int w, int h);

		void showMessageNorth(bool show, const string &msg = "");
		void showMessageCenter(bool show, const string &msg = "");
		void showMessageNorthWest(bool show, const string & msg = "");

		void showCountDown(bool show, double timeRemaining = 0.0, double duration = 0.0);
		void setDisplayType(DisplayType dtype);
		//inline void setCursorShape(ParticleShape pshape) { cursor.setShape(pshape); };
		//inline void setTargetShape(ParticleShape pshape) { target.setShape(pshape); };
};
