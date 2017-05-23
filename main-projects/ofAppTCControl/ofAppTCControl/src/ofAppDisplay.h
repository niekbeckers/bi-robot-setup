#pragma once

#include <cmath>

#include "ofMain.h"
#include "ofUtils.h"
//#include "ofAppMain.h"

class ofAppDisplay : public ofBaseApp
{
	private:
		//
		// variables
		//
		const double scrDiagonal = 27.0;
		double dots_per_m;
		double x0 = 0.0;
		double y0 = 0.25;
		

	public:

		//
		// variables
		//
		//shared_ptr<ofAppMain> mainApp;
		double posCursorX = 0.0, posCursorY = 0.0;
		double posTargetX = 0.0, posTargetY = 0.0;

		//
		// openFrameworks
		//
		void setup();
		void update();
		void draw();
};

