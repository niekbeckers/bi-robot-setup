#pragma once

#include <cmath>

#include "ofMain.h"
#include "ofAppMain.h"

class ofAppDisplay : public ofBaseApp
{
	private:
		const double scrDiagonal = 27.0;

		double dots_per_m;
		double x0 = 0.0;
		double y0 = 0.25;
		

	public:

		shared_ptr<ofAppMain> mainApp;

		void setup();
		void update();
		void draw();
};

