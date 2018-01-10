#include "ofProtocolReader.h"


//--------------------------------------------------------------
ofProtocolReader::ofProtocolReader() {

}

//--------------------------------------------------------------
ofProtocolReader::~ofProtocolReader() {
	// when the class is destroyed
	// close both channels and wait for
	// the thread to finish
	waitForThread(true);
}

//--------------------------------------------------------------
void ofProtocolReader::threadedFunction() {
	// this function is run when StartThread is called. Execute once.
	//Open the Open File Dialog
	ofFileDialogResult openFileResult = ofSystemLoadDialog("Select an experiment XML file (.xml)", false, ofFilePath().getCurrentExeDir());
	//ofLogVerbose() << "(" << typeid(this).name() << ") " << "loadExperimentXML " << ofFilePath().getCurrentExePath();
	
	_settings.protocolname = openFileResult.fileName;
	
	//Check if the user opened a file
	if (openFileResult.bSuccess) {
		ofLogVerbose() << "(" << typeid(this).name() << ") " << "ofAppExperiment::loadExperimentXML ", "User opened file " + openFileResult.fileName;

		//We have a file, check it and process it
		processOpenFileSelection(openFileResult);
	}
	else {
		ofLogNotice() << "(" << typeid(this).name() << ") " << "loadExperimentXML " << "User hit cancel";
	}

	// callback function
	if (_cbFunction) {
		_cbFunction(openFileResult.bSuccess, _settings, _blocks);
	}
	ofLogVerbose() << "(" << typeid(this).name() << ") threadedFunction done";
}


//--------------------------------------------------------------
void ofProtocolReader::registerCBFunction(std::function<void(bool, experimentSettings, vector<blockData>)> callback)
{
	_cbFunction = callback;
	ofLogVerbose() << "(" << typeid(this).name() << ") " << "registerCBFunction " << "callback function registered";
}


//--------------------------------------------------------------
void ofProtocolReader::processOpenFileSelection(ofFileDialogResult openFileResult)
{
	// clear block vector
	_blocks.clear();

	// reset struct
	_settings = {};

	// load XML
	if (XML.load(openFileResult.getPath())) {
		ofLogVerbose() << "(" << typeid(this).name() << ") " << "processOpenFileSelection " << "Experiment protocol XML file loaded: " << openFileResult.fileName;
	}

	// experiment settings (attributes)
	if (XML.exists("countDownDuration")) _settings.cdDuration = XML.getValue<double>("countDownDuration");

	if (XML.exists("trialFeedback")) {
		_settings.trialFeedbackType = static_cast<TrialFeedback>(XML.getValue<int>("trialFeedback"));
		// depending on trial feedback, check if a performance metric is given in the XML
		switch (_settings.trialFeedbackType) {
		case TrialFeedback::RMSE:
			if (XML.exists("trialPerformanceThreshold")) _settings.trialPerformanceThreshold = XML.getValue<double>("trialPerformanceThreshold");
			break;
		case TrialFeedback::MT:
			if (XML.exists("trialMTRangeLower")) _settings.trialMovementTimeRangeSec[0] = XML.getValue<double>("trialMTRangeLower");
			if (XML.exists("trialMTRangeUpper")) _settings.trialMovementTimeRangeSec[1] = XML.getValue<double>("trialMTRangeUpper");
			break;
		}
	}
	else { _settings.trialFeedbackType = TrialFeedback::NONE; } // trialFeedback is either 0 or not present

	// virtual partner settings
	if (XML.exists("doVirtualPartner")) { _settings.vpDoVirtualPartner = XML.getValue<bool>("doVirtualPartner"); }
	else { _settings.vpDoVirtualPartner = false; }

	if (XML.exists("vpFitOnHeRoC")) { _settings.vpFitOnHeRoC = XML.getValue<bool>("vpFitOnHeRoC"); }

	// check which BROS need to be active
	_settings.activeBROSIDs.clear();
	if (XML.exists("activeBROSID") && XML.setTo("activeBROSID")) {
		if (XML.setToChild(0)) {
			do {
				_settings.activeBROSIDs.push_back(XML.getIntValue());
			} while (XML.setToSibling());
			XML.setToParent(); // go back to brosX
		}
		XML.setToParent();
	}
	else {
		// user did not include BROSID in the XML file. Use default  (BROS1 & BROS2)
		_settings.activeBROSIDs.push_back(1); // BROS 1
		_settings.activeBROSIDs.push_back(2); // BROS 2
	}
	ofLogNotice() << "(" << typeid(this).name() << ") " << "ActiveBROSID: " << ofToString(_settings.activeBROSIDs);

	//
	// load blocks and trials
	//

	int trialNumber = 0;
	int blockNumber = 0;

	if (XML.exists("block")) {
		XML.setTo("block[0]"); // set to first block

		do {
			blockData block;

			// read block data
			block.blockNumber = ++blockNumber;
			if (XML.exists("breakDuration")) block.breakDuration = XML.getValue<double>("breakDuration");
			if (XML.exists("homingType")) block.homingType = XML.getValue<int>("homingType");

			// set our "current" trial to the first one
			if (XML.getName() == "block" && XML.setTo("trial[0]"))
			{
				// read each trial
				do {
					// read and store trial data
					trialData trial;
					trial.trialNumber = ++trialNumber;

					if (XML.exists("condition")) trial.condition = XML.getValue<int>("condition");
					if (XML.exists("breakDuration")) trial.breakDuration = XML.getValue<double>("breakDuration");
					if (XML.exists("trialDuration")) trial.trialDuration = XML.getValue<double>("trialDuration");
					if (XML.exists("trialRandomization")) trial.trialRandomization = XML.getValue<double>("trialRandomization");

					if (XML.exists("connected")) { trial.connected = XML.getValue<bool>("connected"); }
					else { trial.connected = false; }
					if (XML.exists("connectionStiffness")) trial.connectionStiffness = XML.getValue<double>("connectionStiffness");
					if (XML.exists("connectionDamping")) trial.connectionDamping = XML.getValue<double>("connectionDamping");

					// check connection types
					trial.connectedTo.clear();
					if (XML.exists("connectedTo")) {
						XML.setTo("connectedTo");
						// loop through active BROS IDs
						for (auto id : _settings.activeBROSIDs) {
							if (XML.exists("id" + ofToString(id))) { trial.connectedTo.push_back(static_cast<ConnectedToTypes>(XML.getValue<int>("id" + ofToString(id)))); }
							else { trial.connectedTo.push_back(ConnectedToTypes::HUMANPARTNER); }
						}
						XML.setToParent();
					}
					else {
						for (auto id : _settings.activeBROSIDs) trial.connectedTo.push_back(ConnectedToTypes::HUMANPARTNER); // DEFAULT
					}

					// check fit virtual partner
					if (XML.exists("fitVirtualPartner")) {

						// read which partners we need to fit the virtual partner model to
						XML.setTo("fitVirtualPartner");
						if (XML.setToChild(0)) {
							do {
								trial.fitVPBROSIDs.push_back(XML.getIntValue());
							} while (XML.setToSibling());
							XML.setToParent(); // go back to brosX
						}
						if (trial.fitVPBROSIDs.size() > 0) {
							trial.fitVirtualPartner = true;
							ofLogVerbose() << "(" << typeid(this).name() << ") " << "FitVPBROSIDs: " << ofToString(trial.fitVPBROSIDs) << " for trial " << trial.trialNumber;
						}

						XML.setToParent();
					}

					block.trials.push_back(trial); // add trial to trials list

				} while (XML.setToSibling()); // go the next trial		

				block.numTrials = block.trials.size();

				_settings.numTrials += block.numTrials;

				_blocks.push_back(block); // add trials to vTrialsBlocks list

				XML.setToParent(); // go back up to the block level
			}
		} while (XML.setToSibling()); // go to the next block

		_experimentLoaded = true;
	}
}
