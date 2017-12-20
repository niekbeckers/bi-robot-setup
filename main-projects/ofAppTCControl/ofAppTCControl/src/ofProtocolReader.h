#pragma once

#include "ofMain.h"
#include <functional>

#include "ofAppExperiment.h"

struct experimentSettings {
	TrialFeedback trialFeedbackType;
};

class ofProtocolReader : public ofThread {

private:
	vector<blockData> _blocks; // vector of vector<trials> to store all trials per block

	void threadedFunction();

public:
	ofXml XML;

	ofProtocolReader();
	~ofProtocolReader();
};