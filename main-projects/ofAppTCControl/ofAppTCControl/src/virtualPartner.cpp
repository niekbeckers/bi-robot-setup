#include "virtualPartner.h"

using namespace std;

//--------------------------------------------------------------
VirtualPartner::VirtualPartner()
{
	// register the callback function in the matlab thread. 
	using namespace std::placeholders;
	_matlabThread.registerCBFunction(std::bind(&VirtualPartner::onVPOptimizationDone, this, _1));

	/*	
	try {
		ofLogError() << "jemoeder";
		int i = system("\"C:\\Program Files\\PuTTY\\pscp.exe\" -r -agent -i C:\\Users\\Labuser\\keys\\niek.ppk C:\\Users\\Labuser\\Documents\\repositories\\bros_experiments\\experiments\\motor-learning\\exp1b-data\\exp2_learning_pilot1_type1\\*.mat niek@130.89.65.74:/home/niek/Documents/test/");
		ofLogError() << "en jevader";
		ofLogError() << "system " << i;
	
	}
	catch(std::exception e) {
		ofLogError() << e.what();
	}
	*/
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
		string var0 = "Object1 (ModelBROS).ModelParameters.VPUseVirtualPartner_BROS" + ofToString(id) + "_Value";
		char *szVar0 = strdup(var0.c_str());
		_lHdlVar_Write_ExecuteVirtualPartner.push_back(_tcClient->getVariableHandle(szVar0, strlen(szVar0)));

		//char szVar1[] = { "Object1 (ModelBROS).ModelParameters.VPModelParams_BROS1_Value" };
		string var1 = "Object1 (ModelBROS).ModelParameters.VPModelParams_BROS"+ ofToString(id) +"_Value";
		char *szVar1 = strdup(var1.c_str());
		_lHdlVar_Write_VPModelParams.push_back(_tcClient->getVariableHandle(szVar1, strlen(szVar1)));

		string var2 = "Object1 (ModelBROS).ModelParameters.VPModelParams_Changed_BROS" + ofToString(id) + "_Value";
		char *szVar2 = strdup(var2.c_str());
		_lHdlVar_Write_VPModelParamsChanged.push_back(_tcClient->getVariableHandle(szVar2, strlen(szVar2)));
	}


	ofLogVerbose() << "(" << typeid(this).name() << ") " << "initialized";

	// start up matlab stand-alone application for model fit
	//std::system(ofToString(matlabFunctionPath + "matlabVirtualPartner.exe &").c_str());
	//_matlabStartup.startThread();
}

//--------------------------------------------------------------
void VirtualPartner::runVPOptimization(matlabInput input)
{
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
	ofLogVerbose() << "(" << typeid(this).name() << ") " << "onVPOptimizationDone " << "callback function called";

	for (int i = 0; i < output.doFitForBROSIDs.size(); i++) {
		int id = output.doFitForBROSIDs[i];

		// error check
		bool noErrors = true;
		if (output.error[i] > 0) {
			noErrors = false;
			ofLogError() << "(" << typeid(this).name() << ") " << "onVPOptimizationDone " << "Errors in output for BROS" << id << ", code: " << output.error[i];
		}
		// check if the output struct is valid
		if (output.trialID == -1) { // this means that an empty matlabOutput struct is used
			noErrors = false;
			ofLogError() << "(" << typeid(this).name() << ") " << "onVPOptimizationDone " << "Empty output";
		}
		if (output.x.size() == 0) {
			noErrors = false;
			ofLogError() << "(" << typeid(this).name() << ") " << "onVPOptimizationDone " << "output.x.size() == 0";
		}

		// send data to ADS, if no errors occurred.
		if (noErrors) {
			sendToTwinCatADS(output, id);
			_validVirtualPartnerFit = true;
		}
		else {
			// errors occurred, don't update the model parameters? Or run the virtual partner?
			bool b = false;
			// find idx of id in activeBROSid
			ptrdiff_t idx = find(_activeBROSIDs.begin(), _activeBROSIDs.end(), id) - _activeBROSIDs.begin();
			if (idx >= _activeBROSIDs.size()) {
				ofLogError() << "(" << typeid(this).name() << ") " << "onVPOptimizationDone " << "Cannot find BROS id (" << id << ") in _activeBROSIDs. Skipping...";
				continue;
			}
			_tcClient->write(_lHdlVar_Write_ExecuteVirtualPartner[idx], &b, sizeof(b));

			ofLogError() << "(" << typeid(this).name() << ") " << "onVPOptimizationDone " << "Error occured in virtual partner fit for BROS" << id;
		}
	}

	_runningModelFit = false;
}

//--------------------------------------------------------------
void VirtualPartner::sendToTwinCatADS(matlabOutput output, int id)
{
	ofLogVerbose() << "(" << typeid(this).name() << ") " << "sendToTwinCatADS " << "Setting virtual partner data in TwinCAT";

	// find idx of id in activeBROSid
	ptrdiff_t idx_active = find(_activeBROSIDs.begin(), _activeBROSIDs.end(), id) - _activeBROSIDs.begin();
	if (idx_active >= _activeBROSIDs.size()) {
		ofLogError() << "(" << typeid(this).name() << ") " << "sendToTwinCatADS " << "Cannot find BROS id (" << id << ") in _activeBROSIDs. Skipping virtual partner parameter write";
		return;
	}
	// find idx of id in fitBROSIds
	ptrdiff_t idx_fit = find(output.doFitForBROSIDs.begin(), output.doFitForBROSIDs.end(), id) - output.doFitForBROSIDs.begin();
	if (idx_fit >= output.doFitForBROSIDs.size()) {
		ofLogError() << "(" << typeid(this).name() << ") " << "sendToTwinCatADS " << "Cannot find BROS id (" << id << ") in output.doFitForBROSIDs. Skipping virtual partner parameter write";
		return;
	}

	double d;
	try {

		// write executeVirtualPartner
		_tcClient->write(_lHdlVar_Write_ExecuteVirtualPartner[idx_active], &output.executeVirtualPartner[idx_fit], sizeof(output.executeVirtualPartner[idx_fit]));

		// write model parameters
		vector<double> x = output.x[idx_fit];

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

		_tcClient->write(_lHdlVar_Write_VPModelParams[idx_active], pData, 24);

		// delete array, cleanup
		delete[] pData;

		d = 1.0;
		// write pulse to model params changed (to trigger DP)
		_tcClient->write(_lHdlVar_Write_VPModelParamsChanged[idx_active], &d, sizeof(d));
		d = 0.0;
		_tcClient->write(_lHdlVar_Write_VPModelParamsChanged[idx_active], &d, sizeof(d));
	}
	catch (int e) {
		ofLogError() << "(" << typeid(this).name() << ") " << "sendToTwinCatADS" << "An exception occurred. Exception Nr: " << ofToString(e);
	}

	ofLogNotice() << "(" << typeid(this).name() << ") " << "sendToTwinCatADS" << "DONE - Setting virtual partner data in TwinCAT";

}