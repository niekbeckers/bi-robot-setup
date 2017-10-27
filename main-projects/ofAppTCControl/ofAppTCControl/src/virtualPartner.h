#pragma once

// ofx classes, addons
#include "ofMain.h"
#include "ofUtils.h"
// custom classes
#include "tcAdsClient.h"
#include "myUtils.h"
#include "ofMatlabThread.h"

#if defined(mclmcrrt_h)
#define INCLUDEMATLABFUNCTIONS 1
#else
#define INCLUDEMATLABFUNCTIONS 0
#endif

class VirtualPartner
{
	private:

		// 
		// variables
		//
		tcAdsClient *_tcClient;
		vector<unsigned long> _lHdlVar_Write_ExecuteVirtualPartner, _lHdlVar_Write_VPModelParams, _lHdlVar_Write_VPModelParamsChanged;

		// MATLAB Thread
		// the matlabthread is started upon construction. Don't worry, the thread does not consume any (hardly) CPU until it's actually called.
		MatlabThread _matlabThread;
		MatlabStartupThread _matlabStartup;

		vector<int> _activeBROSIDs;

		bool _runningModelFit = false;
		bool _validVirtualPartnerFit = false;

	public:
		//
		// variables
		//

		//
		// functions
		//
		VirtualPartner();
		~VirtualPartner();
		void update();
		void initialize(vector<int> vID);
		void runVPOptimization(matlabInput input);
		void sendToTwinCatADS(matlabOutput output);
		void onVPOptimizationDone(matlabOutput output);

		inline bool modelFitIsRunning() { return _runningModelFit; }
};

