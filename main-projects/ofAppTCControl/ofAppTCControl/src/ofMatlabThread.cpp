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
	// call optimization by writing XML settings file
	ofXml xml = input2xml(input);
	
	// write settings file 
	copySettingsAndData(xml, input);

	// now wait for the executable to finish. The exe will write it's output to a XML file with the trial ID in the filename
	// check here if it takes longer than X seconds (error happened?)
	string outputFilename = matlabResultsFilePath_TC + "results_vpmodelfit_trial" + ofToString(input.trialID) + ".xml";
	bool foundFile = false;
	double startTime = ofGetElapsedTimef();
	ofFile file(outputFilename);
	
	while (!foundFile && (ofGetElapsedTimef()-startTime < 240.0)) { // returns false if read is not okay (i.e. file corrupted, not existing, etc).
		if (file.exists()) { 
			foundFile = true; 
		}
		else {
			if (input.fitOnHeRoC) {
				// attempt pscp results file from the HeRoC computer.
				string cmd = "pscp -r -agent -i " + strSSHKey + " -pw " + pwKeyHeRoC +  " " + userHeRoC + "@" + ipAddressHeRoC + ":" + matlabResultsFilePath_HeRoC + "results_vpmodelfit_trial" + ofToString(input.trialID) + ".xml" + " " + matlabResultsFilePath_TC;
				system(cmd.c_str());
			}

			sleep(200); // sleep thread for a little bit
		}

		
	}

	if (foundFile) {
		ofLogNotice() << "(" << typeid(this).name() << ") " << "callMatlabOptimization " << "Found file (fitResults_trial" << input.trialID << ") - parsing results";
		// load xml file
		ofXml xml;
		xml.load(outputFilename);

		// parse xml file
		output = xml2output(xml);
		
		xml.clear();
		ofFile::removeFile(outputFilename);
	}
	else {
		ofLogError() << "(" << typeid(this).name() << ") " << "callMatlabOptimization " << "Could not find file (fitResults_trial" << input.trialID << ")";
		matlabOutput tmp;
		output = tmp;
	}
}

void MatlabThread::copySettingsAndData(ofXml xml, matlabInput input)
{
    // save xml file to local direcory. If the matlabVirtualPartner script is running on the local machine, it will trigger a fit.
    string xmlfilename = ofToString(matlabSettingsFilePath_TC + "settings_vpmodelfit_trial" + ofToString(input.trialID) + ".xml");
    xml.save(xmlfilename);
    
    // if fit on HeRoC, copy mat files (data files) and settings file to HeRoC computer
	if (input.fitOnHeRoC) {
        // select last 5 data files.
        // list files (*.mat)
        ofDirectory dir(matlabDataFilePath_TC);
        dir.allowExt("mat");
        int nFiles = dir.listDir();
        
        // populate the vector<string>
        vector<string> vFilenames;
        for (int i = 0; i < dir.size(); i++) {
            vFilenames.push_back(dir.getPath(i));
        }
        
        // sort files on date modified
        CompareDateModified myComparator;
        std::sort (vFilenames.begin(), vFilenames.end(), myComparator);
        
        // select last 5 trials
        int nrSelFiles = 4;
        if (vFilenames.size() < 4) { nrSelFiles = vFilenames.size(); }
        vector<string> vMatFilenames;
        for ( int i = 0; i < nrSelFiles; i++ ) {
            vMatFilenames.push_back(vFilenames.back());
            vFilenames.pop_back();
        }

		// first, make sure to remove all remaining mat files in the tmpDataDir on the HEROC computer"
		string cmd = ofToString("plink -ssh -i " + strSSHKey + " -pw " + pwKeyHeRoC + " " + userHeRoC + "@" + ipAddressHeRoC + " rm -f " + matlabDataFilePath_HeRoC + "tmpDirDataModelFit/*.mat");
		system(cmd.c_str());
		
        
        // then, secure copy mat files to HeRoC
        try {
			ofLogNotice() << "(" << typeid(this).name() << ") " << "Sending data files (.mat) to HeRoC";

            for (int i = 0; i < vMatFilenames.size(); i++) {
                string cmd = ofToString("pscp -r -agent -i " + strSSHKey + " -pw " + pwKeyHeRoC +  " " + vMatFilenames[i] + " " + userHeRoC + "@" + ipAddressHeRoC + ":" + matlabDataFilePath_HeRoC);
				system(cmd.c_str());
                ofLogVerbose() << "(" << typeid(this).name() << ") " << "system command output: " << i;
            }
            
            // copy XML file to HeRoC (this will trigger the model fit)
			string cmd = ofToString("pscp -r -agent -i " + strSSHKey + " -pw " + pwKeyHeRoC +  " " + xmlfilename + " " + userHeRoC + "@" + ipAddressHeRoC + ":" + matlabSettingsFilePath_HeRoC);
            int i = system(cmd.c_str());
			ofLogNotice() << "(" << typeid(this).name() << ") " << "Sending XML settings file to HeRoC to trigger model fit" << endl << i;
        }
        catch(std::exception e) {
            ofLogError()  << "(" << typeid(this).name() << ") " << e.what();
        }
	}
}

//--------------------------------------------------------------
void MatlabThread::registerCBFunction(std::function<void(matlabOutput)> callback) 
{ 
	_cbFunction = callback; 
	ofLogVerbose() << "(" << typeid(this).name() << ") " << "registerCBFunction " << "callback function registered";
}

//--------------------------------------------------------------
ofXml MatlabThread::input2xml(matlabInput input)
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

	_counterMatlabInputFile++;

	return xml;
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
		xml.setToParent();
	}

	// read GOF, print it
	if (xml.exists("gof")) {
		xml.setTo("gof");

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
				ofLogNotice() << "GOF." << xml.getName() << ": " << ofToString(tmp);
			} while (xml.setToSibling());
			xml.setToParent();
		}
		xml.setToParent();
	}

	return output;
}

