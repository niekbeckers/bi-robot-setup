/*
 * ImgAnalysisThread.cpp
 *
 *  Created on: Oct 8, 2014
 *      Author: arturo
 */

#include "ofMatlabThread.h"

 //--------------------------------------------------------------
MatlabThread::MatlabThread():
	_newOutput(true),
	initialized(false)
{
	// start the thread as soon as the
	// class is created, it won't use any CPU
	// until we send a new frame to be analyzed
	startThread();
}

//--
void MatlabThread::initialize() {
#if INCLUDEMATLABFUNCTIONS
	// Initialize the MATLAB Compiler Runtime global state
	if (!mclInitializeApplication(NULL, 0)) {
		ofLogError("Could not initialize the application properly.");
	}

	// Initialize the Vigenere library
	if (!libtestInitialize()) {
		ofLogError("Could not initialize the library properly.");
	}
	else {
		// everything is initialized
		initialized = true;
	}
#endif

	_startTimeParpoolCheck = ofGetElapsedTimef();
}

//--------------------------------------------------------------
MatlabThread::~MatlabThread() {
	// when the class is destroyed
	// close both channels and wait for
	// the thread to finish
	_toAnalyze.close();
	_analyzed.close();
	waitForThread(true);

#if INCLUDEMATLABFUNCTIONS
	// Shut down the library and the application global state.
	libtestTerminate();
	mclTerminateApplication();
#endif
}

//--------------------------------------------------------------
void MatlabThread::analyze(matlabInput input) {
	// send the frame to the thread for analyzing this makes a copy but we can't avoid it anyway if
	// we want to update the grabber while analyzing previous frames
	_toAnalyze.send(input);
}

//--------------------------------------------------------------
void MatlabThread::update(){
	// check if there's a new analyzed frame and upload it to the texture. we use a while loop to drop any
	// extra frame in case the main thread is slower than the analysis
	// tryReceive doesn't reallocate or make any copies
	_newOutput = false;
	while(_analyzed.tryReceive(_output)){
		_newOutput = true;
	}

	if(_newOutput){
        // do stuff with the output
		//ofLogVerbose("Message from MATLAB thread: trialID = " + ofToString(_output.trialID) + " x=", ofToString(_output.x));

		// do callback function (check if it is assigned)
		if (_cbFunction) {
			_cbFunction(_output);
		}
	}

	/*
	if (ofGetElapsedTimef() - _startTimeParpoolCheck > _checkMatlabParpoolPeriod) {
		_startTimeParpoolCheck = ofGetElapsedTimef();
	}
	*/

}

//--------------------------------------------------------------
bool MatlabThread::newOutputData() {
	return _newOutput;
}

//--------------------------------------------------------------
void MatlabThread::threadedFunction(){
    // wait until there's a new frame this blocks the thread, so it doesn't use
    // the CPU at all, until a frame arrives. also receive doesn't allocate or make any copies

	matlabInput input;
    while(_toAnalyze.receive(input)){

		if (!initialized) return; // matlab not initialized

		matlabOutput output;

		// perform optimization per BROS
		callMatlabOptimization(input, output);

#if __cplusplus>=201103
        _analyzed.send(std::move(data));
#else
        _analyzed.send(output);
#endif
	}
}

//--------------------------------------------------------------
void MatlabThread::callMatlabOptimization(matlabInput input, matlabOutput &output)
{
	input2xml(input);

	// call matlab executable
	//std::system();

	//now wait for the executable to finish. The exe will write it's output to a XML file with the trial ID in the filename
	// check here if it takes longer than X seconds (error happened?)
	ofXml xml;
	string outputFilename = matlabFunctionPath + "fitResults_trial" + ofToString(input.trialID) + ".xml";
	while (!xml.load(outputFilename)) { // returns false if read is not okay (i.e. file corrupted, not existing, etc).
		sleep(200); // sleep thread for a little bit
	}

	// parse xml file
	xml2output(xml);
}

//--------------------------------------------------------------
void MatlabThread::input2xml(matlabInput input)
{
	// write matlab input to XML, which is read by the MATLAB application
	ofXml xml;

	xml.addChild("VP");
	xml.setTo("VP");

	xml.addValue("trialID", input.trialID);

	xml.addChild("doFitForBROSID");
	xml.setTo("doFitForBROSID");
	for (auto id : input.doFitForBROSIDs) { xml.addValue("brosID", id); }
	xml.setToParent();
	//_XMLWrite.addValue("useX0", input.useX0[0]); (etc)

	// save settings to XML file (one for the matlab script/exe, the other for our own administration/data logging
	xml.save(matlabFunctionPath + "vpFitSettings.xml");
	xml.save(matlabFunctionPath + "vpFitSettings_trial" + ofToString(input.trialID) + ".xml");
}

//--------------------------------------------------------------
matlabOutput MatlabThread::xml2output(ofXml xml)
{
	matlabOutput output;

	if (xml.exists("trialID")) output.trialID = xml.getValue<int>("trialID");
	// execute virtual partner
	if (xml.exists("executeVirtualPartner")) {
		xml.setTo("executeVirtualPartner");

		int i = 0;
		while (xml.exists("brosID[" + ofToString(i) + "]")) {
			output.executeVirtualPartner.push_back(xml.getValue<bool>("brosID[" + ofToString(i) + "]"));
			i++;
		}
		xml.setToParent();
	}
	// errors
	if (xml.exists("errors")) {
		xml.setTo("errors");

		int i = 0;
		while (xml.exists("brosID[" + ofToString(i) + "]")) {
			output.error.push_back(xml.getValue<bool>("brosID[" + ofToString(i) + "]"));
			i++;
		}
		xml.setToParent();
	}
	// model parameters
	if (xml.exists("modelparameters")) {
		xml.setTo("modelparameters");

		int i = 0;
		while (xml.exists("bros" + ofToString(i))) {
			xml.setTo("bros" + ofToString(i));
			int j = 0;
			vector<double> tmp;
			while (xml.exists("x" + ofToString(j))) {
				tmp.push_back(xml.getValue<double>("x" + ofToString(j)));
				j++;
			}
			output.x.push_back(tmp);
			xml.setToParent();
			i++;
		}
		xml.setToParent();
	}


	return output;
}

