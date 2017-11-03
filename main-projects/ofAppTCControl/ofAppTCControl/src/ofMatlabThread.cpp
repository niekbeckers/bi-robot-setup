#include "ofMatlabThread.h"

 //--------------------------------------------------------------
MatlabThread::MatlabThread():
	_newOutput(true),
	initialized(false),
	_counterMatlabInputFile(0)
{
	// start the thread as soon as the
	// class is created, it won't use any CPU
	// until we send a new frame to be analyzed
	startThread();
}

//--
void MatlabThread::initialize() {
	initialized = true;
}

//--------------------------------------------------------------
MatlabThread::~MatlabThread() {
	// when the class is destroyed
	// close both channels and wait for
	// the thread to finish
	_toAnalyze.close();
	_analyzed.close();
	waitForThread(true);
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

		// new uoutput ready: do callback function (check if it is assigned)
		if (_cbFunction) {
			_cbFunction(_output);
		}
	}
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

		//if (!initialized) return; // matlab not initialized

		matlabOutput output;

		// perform optimization per BROS
		callMatlabOptimization(input, output);
		_output = output;

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

	//now wait for the executable to finish. The exe will write it's output to a XML file with the trial ID in the filename
	// check here if it takes longer than X seconds (error happened?)
	string outputFilename = matlabResultsFilePath + "results_vpmodelfit_trial" + ofToString(input.trialID) + ".xml";
	bool foundFile = false;
	double startTime = ofGetElapsedTimef();
	ofFile file(outputFilename);
	
	while (!foundFile && (ofGetElapsedTimef()-startTime < 240.0)) { // returns false if read is not okay (i.e. file corrupted, not existing, etc).
		if (file.exists()) { foundFile = true; }
		sleep(250); // sleep thread for a little bit
	}

	if (foundFile) {
		ofLogVerbose() << "(" << typeid(this).name() << ") " << "callMatlabOptimization " << "Found file (fitResults_trial" << input.trialID << ") - parsing results";
		// load xml file
		ofXml xml;
		xml.load(outputFilename);

		// parse xml file
		output = xml2output(xml);
		
		xml.clear();
		ofFile::removeFile(outputFilename);
	}
	else {
		ofLogVerbose() << "(" << typeid(this).name() << ") " << "callMatlabOptimization " << "Could not find file (fitResults_trial" << input.trialID << ")";
		matlabOutput tmp;
		output = tmp;
	}
}

//--------------------------------------------------------------
void MatlabThread::registerCBFunction(std::function<void(matlabOutput)> callback) 
{ 
	_cbFunction = callback; 
	ofLogVerbose() << "(" << typeid(this).name() << ") " << "registerCBFunction " << "callback function registered";
}

//--------------------------------------------------------------
void MatlabThread::input2xml(matlabInput input)
{
	// write matlab input to XML, which is read by the MATLAB application
	ofXml xml;

	xml.addChild("VP");
	xml.setTo("VP");
	xml.addValue("trialID", input.trialID);
	xml.addValue("condition", input.condition);
	xml.addChild("doFitForBROSID");
	xml.setTo("doFitForBROSID");
	for (int i = 0; i < input.doFitForBROSIDs.size(); i++) {
		xml.addValue("id" + ofToString(i) , input.doFitForBROSIDs[i]);
	}
	xml.setToParent();
	//_XMLWrite.addValue("useX0", input.useX0[0]); (etc)

	// save settings to XML file (one for the matlab script/exe, the other for our own administration/data logging
	xml.save(matlabSettingsFilePath + "settings_vpmodelfit_trial" + ofToString(_counterMatlabInputFile) + ".xml");
	_counterMatlabInputFile++;
}

//--------------------------------------------------------------
matlabOutput MatlabThread::xml2output(ofXml xml)
{
	// read xml and store in matlabOutput struct

	matlabOutput output;
	ofLogNotice() << "(" << typeid(this).name() << ") " << "xml2output " << "Results from the model fit:";

	if (xml.exists("trialID")) output.trialID = xml.getValue<int>("trialID");
	ofLogNotice() << "Trial ID: " << output.trialID;

	// execute virtual partner
	if (xml.exists("executeVirtualPartner")) {
		xml.setTo("executeVirtualPartner");
		if (xml.setToChild(0)) {
			do {
				output.executeVirtualPartner.push_back(xml.getBoolValue());
			} while (xml.setToSibling());
			xml.setToParent(); // go back to brosX
		}
		xml.setToParent();
	}
	ofLogNotice() << "ExecuteVirtualPartner " << ofToString(output.executeVirtualPartner);

	// doFitForBROSID
	if (xml.exists("doFitForBROSID")) {
		xml.setTo("doFitForBROSID");
		if (xml.setToChild(0)) {
			do {
				output.doFitForBROSIDs.push_back(xml.getIntValue());
			} while (xml.setToSibling());
			xml.setToParent(); // go back to brosX
		}
		xml.setToParent();
	}
	ofLogNotice() << "DoFitForBROSID " << ofToString(output.doFitForBROSIDs);

	// errors
	if (xml.exists("error")) {
		xml.setTo("error");
		if (xml.setToChild(0)) {
			do {
				output.error.push_back(xml.getIntValue());
			} while (xml.setToSibling());
			xml.setToParent(); // go back to brosX
		}
		xml.setToParent();
	}
	ofLogNotice() << "Error " << ofToString(output.error);

	// model parameters
	if (xml.exists("modelparameters")) {
		xml.setTo("modelparameters");

		if (xml.setToChild(0)) { // set to first child (if it exists)
			vector<double> tmp;
			do {
				tmp.clear();
				if (xml.setToChild(0)) {
					do {
						tmp.push_back(xml.getFloatValue());
					} while (xml.setToSibling());
					xml.setToParent(); // go back to brosX
				}
				output.x.push_back(tmp);
				ofLogNotice() << "Modelparameters." << xml.getName() << ": " << ofToString(tmp);
			} while (xml.setToSibling());
			xml.setToParent();
		}
	}
	return output;
}

