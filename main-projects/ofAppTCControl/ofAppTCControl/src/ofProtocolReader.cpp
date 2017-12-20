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
	// 
}



//--------------------------------------------------------------
void ofAppExperiment::processOpenFileSelection(ofFileDialogResult openFileResult)
{
	// clear block vector
	_blocks.clear();

	if (XML.load(openFileResult.getPath())) {
		ofLogVerbose() << "(" << typeid(this).name() << ") " << "processOpenFileSelection " << "Loaded: " << openFileResult.getPath();
		// log to file as well
		ofLogVerbose() << "(" << typeid(this).name() << ") " << "processOpenFileSelection " << "Experiment protocol XML file loaded: " << openFileResult.getPath();
	}

	// experiment settings (attributes)
	if (XML.exists("countDownDuration")) _cdDuration = XML.getValue<double>("countDownDuration");

	if (XML.exists("trialFeedback")) {
		_trialFeedbackType = static_cast<TrialFeedback>(XML.getValue<int>("trialFeedback"));
		// depending on trial feedback, check if a performance metric is given in the XML
		switch (_trialFeedbackType) {
		case TrialFeedback::RMSE:
			if (XML.exists("trialPerformanceThreshold")) _trialPerformanceThreshold = XML.getValue<double>("trialPerformanceThreshold");
			break;
		case TrialFeedback::MT:
			if (XML.exists("trialMTRangeLower")) _trialMovementTimeRangeSec[0] = XML.getValue<double>("trialMTRangeLower");
			if (XML.exists("trialMTRangeUpper")) _trialMovementTimeRangeSec[1] = XML.getValue<double>("trialMTRangeUpper");
			break;
		}
	}
	else { _trialFeedbackType = TrialFeedback::NONE; } // trialFeedback is either 0 or not present

													   // virtual partner settings
	if (XML.exists("doVirtualPartner")) { _vpDoVirtualPartner = XML.getValue<bool>("doVirtualPartner"); }
	else { _vpDoVirtualPartner = false; }

	// check which BROS need to be active
	_activeBROSIDs.clear();
	if (XML.exists("activeBROSID") && XML.setTo("activeBROSID")) {
		if (XML.setToChild(0)) {
			do {
				_activeBROSIDs.push_back(XML.getIntValue());
			} while (XML.setToSibling());
			XML.setToParent(); // go back to brosX
		}
		XML.setToParent();
	}
	else {
		// user did not include BROSID in the XML file. Use default  (BROS1 & BROS2)
		_activeBROSIDs.push_back(1); // BROS 1
		_activeBROSIDs.push_back(2); // BROS 2
	}
	ofLogNotice() << "(" << typeid(this).name() << ") " << "ActiveBROSID: " << ofToString(XML.getIntValue());

	int trialNumber = 0;
	int blockNumber = 0;

	// load blocks and trials
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
						for (auto id : _activeBROSIDs) {
							if (XML.exists("id" + ofToString(id))) { trial.connectedTo.push_back(static_cast<ConnectedToTypes>(XML.getValue<int>("id" + ofToString(id)))); }
							else { trial.connectedTo.push_back(ConnectedToTypes::HUMANPARTNER); }
						}
						XML.setToParent();
					}
					else {
						for (auto id : _activeBROSIDs) trial.connectedTo.push_back(ConnectedToTypes::HUMANPARTNER); // DEFAULT
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

				_numTrials += block.numTrials;

				_blocks.push_back(block); // add trials to vTrialsBlocks list

				XML.setToParent(); // go back up to the block level
			}
		} while (XML.setToSibling()); // go to the next block

		_experimentLoaded = true;
	}
}
