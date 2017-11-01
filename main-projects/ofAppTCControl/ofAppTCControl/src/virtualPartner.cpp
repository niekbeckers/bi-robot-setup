#include "virtualPartner.h"

using namespace std;

//--------------------------------------------------------------
VirtualPartner::VirtualPartner()
{
	// register the callback function in the matlab thread. 
	using namespace std::placeholders;
	_matlabThread.registerCBFunction(std::bind(&VirtualPartner::onVPOptimizationDone, this, _1));
}

//--------------------------------------------------------------
VirtualPartner::~VirtualPartner()
{

}

//--------------------------------------------------------------
void VirtualPartner::update()
{
	_matlabThread.update();
}

//--------------------------------------------------------------
void VirtualPartner::initialize(vector<int> vID)
{
	// initialize matlab thread
	_matlabThread.initialize();

	// set up tcAdsClient for data reading
	_tcClient = new tcAdsClient(adsPort);

	// clear handle vectors
	_lHdlVar_Write_ExecuteVirtualPartner.clear();
	_lHdlVar_Write_VPModelParams.clear();
	_lHdlVar_Write_VPModelParamsChanged.clear();

	// copy active bros id vector
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

	ofLogVerbose("VirtualPartner::initialize", "initialized");

	// start up matlab stand-alone application for model fit
	//std::system(ofToString(matlabFunctionPath + "matlabVirtualPartner.exe &").c_str());
	//_matlabStartup.startThread();
}

//--------------------------------------------------------------
void VirtualPartner::runVPOptimization(matlabInput input)
{
	if (!_matlabThread.initialized) {
		ofLogError("MATLAB thread not initialized. Did you add the header, is the DLL in the path?");
		//return;
	}

	// set _runningVPOptimization flag
	_runningModelFit = true;
	_validVirtualPartnerFit = false;

	// call for analysis. Once the MATLAB script is ready, it will call the registered callback function.
	_matlabThread.analyze(input);

	// once the optimization is done, the callback function onVPOptimizationDone will be called, in which we can set everything (in the state machine, ADS, etc).
}

//--------------------------------------------------------------
void VirtualPartner::onVPOptimizationDone(matlabOutput output)
{
	// do soemthing with the output data
	ofLogVerbose("VirtualPartner::onVPOptimizationDone", "callback function called");

	// error check
	bool noErrors = true;
	for (int i = 0; i < output.error.size(); i++) {
		if (output.error[i] > 0) {
			noErrors = false;
			ofLogError("VirtualPartner::onVPOptimizationDone", "errors in output, code: "+ output.error[i]);
		}
	}
	// check if the output struct is valid
	if (output.trialID == -1) { // this means that an empty matlabOutput struct is used
		noErrors = false;
		ofLogError("VirtualPartner::onVPOptimizationDone", "empty output");
	}
	if (output.x.size() == 0) {
		noErrors = false;
		ofLogError("VirtualPartner::onVPOptimizationDone", "output.x.size() == 0");
	}

	// send data to ADS, if no errors occurred.
	if (noErrors) {
		sendToTwinCatADS(output);
		_validVirtualPartnerFit = true;
	}
	else {
		// errors occurred, don't update the model parameters? Or run the virtual partner?
		bool b = false;
		for (auto l : _lHdlVar_Write_ExecuteVirtualPartner) {
			_tcClient->write(l, &b, sizeof(b));
		}
		_validVirtualPartnerFit = false;
		ofLogError("VirtualPartner::onVPOptimizationDone","Error occured in virtual partner fit");
	}
	
	_runningModelFit = false;
}

//--------------------------------------------------------------
void VirtualPartner::sendToTwinCatADS(matlabOutput output)
{
	ofLogVerbose("VirtualPartner::sendToTwinCatADS", "Setting virtual partner data in TwinCAT");
	double d;
	for (int i = 0; i < _activeBROSIDs.size(); i++) {
		// write executeVirtualPartner
		_tcClient->write(_lHdlVar_Write_ExecuteVirtualPartner[i], &output.executeVirtualPartner[i], sizeof(output.executeVirtualPartner[i]));

		// write model parameters
		vector<double> x = output.x[i];
		ofLogVerbose("x " + ofToString(x));
		// vector values to byte array, such that we can send the model params to TC. Note: the byte array size cannot be set dynamically (runtime), so preset.
		// this means you'd have to adjust the byte array size in case you update the number of model parameters sent to TC.
		// The code below is based on the example found here: 
		// https://infosys.beckhoff.com/english.php?content=../content/1033/tcsample_webservice/html/webservice_sample_cpp.htm&id=
		
		BYTE *pData = new BYTE[24];
		int nIOffs = 0;
		int nISize = 24;
		for (int j = 0; j < x.size(); j++) {
			memcpy_s(&pData[nIOffs], nISize, &x[j], 8); // copy double to byte array
			nIOffs += 8;								// writing doubles, i.e. offset with 8 bytes
			nISize -= 8;								// decrease destination size
		}

		ofLogVerbose("pData BROS" + ofToString(i) + " " + ofToString(pData) + " sizeof " + ofToString(sizeof(pData)));

		_tcClient->write(_lHdlVar_Write_VPModelParams[i], pData, 24);

		// delete array
		delete[] pData;
		
		d = 1.0;
		// write pulse to model params changed (to trigger DP)
		_tcClient->write(_lHdlVar_Write_VPModelParamsChanged[i], &d, sizeof(d));
		d = 0.0;
		_tcClient->write(_lHdlVar_Write_VPModelParamsChanged[i], &d, sizeof(d));
	}
}