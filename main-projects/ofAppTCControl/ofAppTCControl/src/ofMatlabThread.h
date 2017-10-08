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
	vector<vector<double>> x;
	vector<int> error;
	vector<bool> executeVirtualPartner;
};

struct matlabInput {
	int trialID = -1;
	vector<int> doFitForBROSIDs;
	vector<vector<double>> x0;
	vector<bool> useX0;
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
	double _startTimeParpoolCheck = 0.0, _checkMatlabParpoolPeriod = 30.0;

	//
	// functions
	//
	void threadedFunction();
	void callMatlabOptimization(matlabInput input, matlabOutput &output);

	// callback function 
	std::function<void(matlabOutput)> _cbFunction = NULL;
	
public:
	//
	// Initialized
	//
	bool initialized;

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