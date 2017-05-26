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
		double y0 = 0.25;
		
		string _message = "";
		bool _showMessage = false;

		ofTrueTypeFont verdana30;

	public:

		//
		// variables
		//
		displayData *pData;

		//
		// openFrameworks
		//
		void setup();
		void update();
		void draw();
		void windowResized(int w, int h);

		void showMessage(bool show);
		void setMessage(const string &msg);
};

