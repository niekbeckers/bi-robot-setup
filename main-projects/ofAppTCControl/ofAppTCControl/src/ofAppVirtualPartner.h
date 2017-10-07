#pragma once

// ofx classes, addons
#include "ofMain.h"
#include "ofUtils.h"


// custom classes
#include "ofAppMain.h"
#include "ofAppExperiment.h"
#include "tcAdsClient.h"
#include "myUtils.h"
#include "ofMatlabThread.h"

#if defined(mclmcrrt_h)
#define INCLUDEMATLABFUNCTIONS 1
#else
#define INCLUDEMATLABFUNCTIONS 0
#endif

class ofAppMain;
class ofAppExperiment;

class ofAppVirtualPartner
{
	private:

		// niek is een schaap
		// variables
		//
		tcAdsClient *_tcClient;
		unsigned long _lHdlVar_Write_ExecuteVirtualPartner[2], _lHdlVar_Write_VPModelParams[2], _lHdlVar_Write_VPModelParamsChanged[2];

		// MATLAB Thread
		// the matlabthread is started upon construction. Don't worry, the thread does not consume any (hardly) CPU until it's actually called.
		MatlabThread _matlabThread;

		vector<int> _activeBROSIDs;

		bool _runningModelFit = false;

	public:
		//
		// variables
		//
		shared_ptr<ofAppMain> mainApp;
		shared_ptr<ofAppExperiment> experimentApp;

		//
		// functions
		//
		void initialize(vector<int> v);
		void runVPOptimization();
		void sendToTwinCatADS(matlabOutput output);

		void onVPOptimizationDone(matlabOutput output);

		inline bool modelFitIsRunning() { return _runningModelFit; }
};