#include "ofAppVirtualPartner.h"

using namespace std;
 
//--------------------------------------------------------------
void ofAppVirtualPartner::initialize(vector<int> v)
{
	// set up tcAdsClient for data reading
	_tcClient = new tcAdsClient(adsPort);

#if INCLUDEMATLABFUNCTIONS
	_matlabThread.initialize();
#endif

	using namespace std::placeholders;

	_activeBROSIDs = v;

	// get ADS handles
	string var0, var1, var2;

	for (int i = 0; i < _activeBROSIDs.size(); i++) {
		int id = _activeBROSIDs[i];
		var0 = "Object1 (ModelBROS).ModelParameters.VPUseVirtualPartner_BROS" + ofToString(id);
		char *szVar0 = strdup(var0.c_str());
		_lHdlVar_Write_ExecuteVirtualPartner[i] = _tcClient->getVariableHandle(szVar0, sizeof(szVar0));

		var1 = "Object1 (ModelBROS).ModelParameters.VPModelParams_BROS" + ofToString(id);
		char *szVar1 = strdup(var1.c_str());
		_lHdlVar_Write_VPModelParams[i] = _tcClient->getVariableHandle(szVar1, sizeof(szVar1));

		var2 = "Object1 (ModelBROS).ModelParameters.VPModelParams_Changed_BROS" + ofToString(id);
		char *szVar2 = strdup(var2.c_str());
		_lHdlVar_Write_VPModelParamsChanged[i] = _tcClient->getVariableHandle(szVar2, sizeof(szVar2));
	}

	// register the callback function in the matlab thread. 
	_matlabThread.registerCBFunction(std::bind(&ofAppVirtualPartner::onVPOptimizationDone, this, _1));
}

//--------------------------------------------------------------
void ofAppVirtualPartner::runVPOptimization()
{
	if (!_matlabThread.matlabThreadInitialized) {
		ofLogError("MATLAB Runtime nog initialized. Did you add the header, is the DLL in the path?");
		return;
	}

	// Make sure that the datalogger is paused, such that the last datafile is closed. 
	// do we need to wait for a little bit?
	mainApp->stopDataRecorder();

	// set _runningVPOptimization flag
	_runningModelFit = true;

	//
	// run MATLAB script
	//

	// Create input struct
	matlabInput input;
	input.trialID = experimentApp->getCurrentTrialNumber();
	//input.doOptimization = _currentTrial.fitVPModel;

	// call for analysis. Once the MATLAB script is ready, it will call the registered callback function.
	_matlabThread.analyze(input);

	// once the optimization is done, the callback function onVPOptimizationDone will be called, in which we can set everything (in the state machine, ADS, etc).
}

//--------------------------------------------------------------
void ofAppVirtualPartner::onVPOptimizationDone(matlabOutput output)
{
	// do soemthing with the output data
	ofLogVerbose("ofAppExperiment::onVPOptimizationDone", "callback funtion called");

	/*
	// error check
	_useVPinTrial[brosID - 1] = true;
	if (output.error > 0) {
	_useVPinTrial[brosID - 1] = false;
	ofLogError("ofAppExperiment::onVPOptimizationDone", "Error occurred in the optimization (MATLAB). BROS: "+ ofToString(brosID-1) +" Error code: " + ofToString(output.error));
	}
	else {
	// set virtual partner settings here (send over ADS)
	setVirtualPartnerADS(output);
	}

	// perform the optimization for BROS 2
	if (brosID < 2) {
	matlabInput input;
	input.trialID = _currentTrialNumber;
	input.brosID = 2;
	input.doOptimization = _currentTrial.fitVPModel;

	// call for analysis. Once the MATLAB script is ready, it will call the registered callback function.
	_matlabThread.analyze(input);
	}
	else {
	_runningVPOptimization = false;

	// Unpause the data logger
	mainApp->startDataRecorder();
	}
	*/

}

//--------------------------------------------------------------
void ofAppVirtualPartner::sendToTwinCatADS(matlabOutput output)
{
	for (int i = 0; i < _activeBROSIDs.size(); i++) {

		// write executeVirtualPartner
		_tcClient->write(_lHdlVar_Write_ExecuteVirtualPartner[i], &output.executeVirtualPartner[i], sizeof(output.executeVirtualPartner[i]));

		// write model parameters
		_tcClient->write(_lHdlVar_Write_VPModelParams[i], &output.x[i], sizeof(output.x[i]));

		double d = 1.0;
		// write pulse to model params changed (to trigger DP)
		_tcClient->write(_lHdlVar_Write_VPModelParamsChanged[i], &d, sizeof(d));
		d = 0.0;
		_tcClient->write(_lHdlVar_Write_VPModelParamsChanged[i], &d, sizeof(d));
	}
}