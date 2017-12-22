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
	vector<int> doFitForBROSIDs;
	int trialID = -1;
	vector<vector<double>> x;
	vector<int> error;
	vector<bool> executeVirtualPartner;
};

struct matlabInput {
	int trialID = -1;
	vector<int> doFitForBROSIDs;
	int condition = 0; 
	vector<vector<double>> x0;
	vector<bool> useX0;
	bool fitOnHeRoC = false;
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
	int _counterMatlabInputFile = 0;

	//
	// functions
	//
	void threadedFunction();
	void callMatlabOptimization(matlabInput input, matlabOutput &output);
	ofXml input2xml(matlabInput input);
	matlabOutput xml2output(ofXml xml);
	void copySettingsAndData(ofXml xml);

	// callback function 
	std::function<void(matlabOutput)> _cbFunction;
	
public:
	//
	// Parameters
	//
	bool initialized;
	string matlabSettingsFilePath = "C:\\Users\\Labuser\\Documents\\repositories\\bros_experiments\\main-projects\\matlab-vp-modelfit\\settings\\";
	string matlabResultsFilePath  = "C:\\Users\\Labuser\\Documents\\repositories\\bros_experiments\\main-projects\\matlab-vp-modelfit\\results\\";


	//
	// functions
	//
	MatlabThread();
	~MatlabThread();
	void initialize();
	void analyze(matlabInput input);
	void update();
	bool newOutputData();
	void registerCBFunction(std::function<void(matlabOutput)> cb);
};

class MatlabStartupThread : public ofThread {

private:
	string _matlabExePath = "C:\\Users\\Labuser\\Documents\\repositories\\bros_experiments\\main-projects\\ofAppTCControl\\ofAppTCControl\\matlab\\matlabVirtualPartner.exe &";
	void threadedFunction() { std::system(_matlabExePath.c_str()); done = true; };
public:
	bool done = false;
	MatlabStartupThread() {};
	MatlabStartupThread(string p) { _matlabExePath = p; }
	~MatlabStartupThread() { waitForThread(true); }
};