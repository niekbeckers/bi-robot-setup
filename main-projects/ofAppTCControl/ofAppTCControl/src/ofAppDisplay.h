#pragma once

#include <cmath>

#include "ofMain.h"
#include "ofAppMain.h"

class ofAppDisplay : public ofBaseApp
{
	private:
		double dots_per_m;
		double scrDiagonal = 27.0;
	public:

		shared_ptr<ofAppMain> mainApp;

		void setup();
		void update();
		void draw();
};

