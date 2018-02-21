#pragma once

// ofx classes, addons
#include "ofMain.h"
#include "ofUtils.h"
// custom classes
#include "tcAdsClient.h"
#include "myCommon.h"
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
		unsigned long _lHdlVar_Write_VPUsePresetParams;

		// MATLAB Thread
		// the matlabthread is started upon construction. Don't worry, the thread does not consume any (hardly) CPU until it's actually called.
		MatlabThread _matlabThread;

		vector<int> _activeBROSIDs;

		bool _runningModelFit = false;
		bool _validVirtualPartnerFit = false;

		//
		// functions
		//
		void sendVirtualPartnerDataToTwinCAT(matlabOutput output, int id);
		void onVPOptimizationDone(matlabOutput output);

	public:
		//
		// variables
		//
		bool initialized;
		matlabOutput latestMatlabOutput;

		//
		// functions
		//
		VirtualPartner();
		~VirtualPartner();
		void update();
		void initialize(vector<int> vID);
		void runVPOptimization(matlabInput input);
		void setExecuteVP(int id, bool execute);
		void setUsePresetParamsVP(int id, bool setUse);

		inline bool validVirtualPartnerFit() { return _validVirtualPartnerFit; };
		inline bool modelFitIsRunning() { return _runningModelFit; }
};

