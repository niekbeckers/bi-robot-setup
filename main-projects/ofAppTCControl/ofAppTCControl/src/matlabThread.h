#pragma once

#include "ofMain.h"

#define INCLUDEMATLABFUNCTION 0

struct matlabOutput {
	double d[2] = {0.0,-1.0};
	int trialID = -1;
};

struct matlabInput {
	int trialID = -1;
};

class MatlabThread : public ofThread {
private:
	//
	// variables
	//
	ofThreadChannel<matlabInput> _toAnalyze;
	ofThreadChannel<matlabOutput> _analyzed;
	bool _newData;
	matlabOutput _output;

	//
	// functions
	//
	void threadedFunction();
	
public:

	//
	// functions
	//
	MatlabThread();
	~MatlabThread();
	void analyze(matlabInput input);
	void update();
};