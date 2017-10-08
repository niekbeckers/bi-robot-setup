#include "ofAppVirtualPartner.h"

using namespace std;

VirtualPartner::VirtualPartner()
{

}

VirtualPartner::~VirtualPartner()
{

}
//--------------------------------------------------------------
void VirtualPartner::initialize(vector<int> vID)
{
	// set up tcAdsClient for data reading
	_tcClient = new tcAdsClient(adsPort);

#if INCLUDEMATLABFUNCTIONS
	_matlabThread.initialize();
#endif

	// clear handle vectors
	_lHdlVar_Write_ExecuteVirtualPartner.clear();
	_lHdlVar_Write_VPModelParams.clear();
	_lHdlVar_Write_VPModelParamsChanged.clear();

	using namespace std::placeholders;

	_activeBROSIDs = vID;

	// get ADS handles
	for (int i = 0; i < _activeBROSIDs.size(); i++) {
		int id = _activeBROSIDs[i];
		string var0 = "Object1 (ModelBROS).ModelParameters.VPUseVirtualPartner_BROS" + ofToString(id);
		char *szVar0 = strdup(var0.c_str());
		_lHdlVar_Write_ExecuteVirtualPartner.push_back(_tcClient->getVariableHandle(szVar0, sizeof(szVar0)));

		string var1 = "Object1 (ModelBROS).ModelParameters.VPModelParams_BROS" + ofToString(id);
		char *szVar1 = strdup(var1.c_str());
		_lHdlVar_Write_VPModelParams.push_back(_tcClient->getVariableHandle(szVar1, sizeof(szVar1)));

		string var2 = "Object1 (ModelBROS).ModelParameters.VPModelParams_Changed_BROS" + ofToString(id);
		char *szVar2 = strdup(var2.c_str());
		_lHdlVar_Write_VPModelParamsChanged.push_back(_tcClient->getVariableHandle(szVar2, sizeof(szVar2)));
	}

	// register the callback function in the matlab thread. 
	_matlabThread.registerCBFunction(std::bind(&VirtualPartner::onVPOptimizationDone, this, _1));
}

//--------------------------------------------------------------
void VirtualPartner::runVPOptimization(matlabInput input)
{
	if (!_matlabThread.initialized) {
		ofLogError("MATLAB Runtime nog initialized. Did you add the header, is the DLL in the path?");
		return;
	}

	// set _runningVPOptimization flag
	_runningModelFit = true;

	//
	// run MATLAB script
	//

	// Create input struct
	//matlabInput input;
	//input.trialID = experimentApp->getCurrentTrialNumber();
	//input.doFitForBROSIDs = experimentApp->getCurrentTrial().fitVPBROSIDs;

	// call for analysis. Once the MATLAB script is ready, it will call the registered callback function.
	_matlabThread.analyze(input);

	// once the optimization is done, the callback function onVPOptimizationDone will be called, in which we can set everything (in the state machine, ADS, etc).
}

//--------------------------------------------------------------
void VirtualPartner::onVPOptimizationDone(matlabOutput output)
{
	// do soemthing with the output data
	ofLogVerbose("ofAppExperiment::onVPOptimizationDone", "callback function called");

	// error check
	bool noErrors = true;
	for (int i = 0; i < output.error.size(); i++) {
		if (output.error[i] > 0) {
			noErrors = false;
			ofLogError("ofAppVirtualPartner", "Error occurred in model fit for BROS" + ofToString(_activeBROSIDs[i]));
		}
	}

	if (noErrors) {
		sendToTwinCatADS(output);
	}
	else {
		// errors occurred, don't update the model parameters? Or run the virtual partner?
		bool b = false;
		for (auto l : _lHdlVar_Write_ExecuteVirtualPartner) {
			_tcClient->write(l, &b, sizeof(b));
		}
	}
	
	_runningModelFit = false;
}

//--------------------------------------------------------------
void VirtualPartner::sendToTwinCatADS(matlabOutput output)
{
	double d;
	for (int i = 0; i < _activeBROSIDs.size(); i++) {

		// write executeVirtualPartner
		_tcClient->write(_lHdlVar_Write_ExecuteVirtualPartner[i], &output.executeVirtualPartner[i], sizeof(output.executeVirtualPartner[i]));

		// write model parameters
		double* mp = &output.x[i][0];
		_tcClient->write(_lHdlVar_Write_VPModelParams[i], &mp, sizeof(mp));

		d = 1.0;
		// write pulse to model params changed (to trigger DP)
		_tcClient->write(_lHdlVar_Write_VPModelParamsChanged[i], &d, sizeof(d));
		d = 0.0;
		_tcClient->write(_lHdlVar_Write_VPModelParamsChanged[i], &d, sizeof(d));
	}
}