#pragma once

#include <cmath>

#include "ofMain.h"
#include "ofUtils.h"

#include "myUtils.h"

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
		string _message = "";
		bool _showMessage = false;
		ofTrueTypeFont verdana50;
		ofTrueTypeFont verdana30;

		// countdown
		bool _showCountDown = false;
		double _cdDuration = 3.0;
		double _cdTimeRemaining = 1.0;
		double _cdBarWidth = 200.0;
		double _cdBarHeight = 40.0;
		ofPoint _cdBarPosition = { 0.0 - _cdBarWidth / 2.0, -(0.35*ofGetScreenHeight() + _cdBarHeight / 2.0) };

	public:

		//
		// variables
		//
		displayData *pData;
		bool drawTask = true;

		//
		// openFrameworks
		//
		void setup();
		void update();
		void draw();
		void windowResized(int w, int h);

		void showMessage(bool show);
		void showMessage(bool show, const string &msg);
		void setMessage(const string &msg);

		void showCountDown(bool show, double timeRemaining = 0.0, double duration = 0.0);
};

