#pragma once
#include "ofBaseApp.h"
using namespace std;

class experimentHandler : public ofBaseApp
{
	private:
		int trialCounter, blockCounter;

	public:
		experimentHandler();

		void LoadExperimentXML(string &filename);


};

