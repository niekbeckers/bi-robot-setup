#pragma once

#include "ofMain.h"

// For debugging purposes - check if any MATLAB SDK library is defined. If not, don't execute MATLAB code
#if defined(mclmcrrt_h)
	#define INCLUDEMATLABFUNCTIONS 1
#else
	#define INCLUDEMATLABFUNCTIONS 0
#endif

struct matlabOutput {
	double d[2] = {0.0,-1.0};
	int trialID = -1;				//
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
	bool _newOutput;
	matlabOutput _output;

	//
	// functions
	//
	void threadedFunction();
	
public:
	//
	// Initialized
	//
	bool _matlabThreadInitialized;


	//
	// functions
	//
	MatlabThread();
	~MatlabThread();
	void initialize();
	void analyze(matlabInput input);
	void update();
	bool newOutputData();
};