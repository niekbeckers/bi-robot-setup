#pragma once
#include "ofBaseApp.h"
using namespace std;

class ExperimentHandler : public ofBaseApp
{
	private:
		int trialCounter, blockCounter;

	public:
		ExperimentHandler();

		void LoadExperimentXML(string &filename);


};

