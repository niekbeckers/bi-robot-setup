#pragma once
#include "ofBaseApp.h"
using namespace std;

class experimentHandler : public ofBaseApp
{
	private:

		//
		// variables
		//
		int trialCounter, blockCounter;

	public:
		experimentHandler();

		// 
		// functions
		//
		void LoadExperimentXML(const string &filename);
		void eventTrialDone();
};

