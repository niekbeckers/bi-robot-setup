#pragma once

#include "ofMain.h"
#include <functional>

// For debugging purposes - check if any MATLAB SDK library is defined. If not, don't execute MATLAB code
#if defined(mclmcrrt_h)
	#define INCLUDEMATLABFUNCTIONS 1
#else
	#define INCLUDEMATLABFUNCTIONS 0
#endif

struct matlabOutput {
	int trialID = -1;
	vector<double> x;
	int error[2] = { 0, 0 };
	bool executeVirtualPartner[2] = { false, false };
};

struct matlabInput {
	int trialID = -1;
	bool doOptimization[2] = { false, false };
	double x0[2] = { -1.0, -1.0 };
	bool useX0 = false;
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
	void callMatlabOptimization(matlabInput input, matlabOutput &output);

	// callback function, when 
	std::function<void(matlabOutput)> _cbFunction = NULL;
	
public:
	//
	// Initialized
	//
	bool matlabThreadInitialized;

	//
	// functions
	//
	MatlabThread();
	~MatlabThread();
	void initialize();
	void analyze(matlabInput input);
	void update();
	bool newOutputData();
	inline void registerCBFunction(std::function<void(matlabOutput)> callback) { _cbFunction = callback; };
};