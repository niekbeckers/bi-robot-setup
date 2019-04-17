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
	
	string fn;

	//Check if the user opened a file
	if (openFileResult.bSuccess) {
		ofLogVerbose() << "(" << typeid(this).name() << ") " << "ofAppExperiment::loadExperimentXML ", "User opened file " + openFileResult.fileName;
		_settings.protocolname = openFileResult.fileName;
		fn = openFileResult.fileName;
		//We have a file, check it and process it
		processOpenFileSelection(openFileResult);
	}
	else {
		ofLogNotice() << "(" << typeid(this).name() << ") " << "loadExperimentXML " << "User hit cancel";
		fn = "No protocol loaded";
	}

	// callback function
	if (_cbFunction) {
		_cbFunction(openFileResult.bSuccess, fn, _settings, _blocks);
	}
	ofLogVerbose() << "(" << typeid(this).name() << ") threadedFunction done";
}


//--------------------------------------------------------------
void ofProtocolReader::registerCBFunction(std::function<void(bool, std::string, experimentSettings, vector<blockData>)> callback)
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
	auto exp = XML.getChild("experiment");
	if (exp) {
		// main node found

		// countdown duration (if specified)
		if (exp.getChild("countDownDuration")) _settings.cdDuration = exp.getChild("countDownDuration").getDoubleValue();

		// trial performance feedback
		if (exp.getChild("trialFeedback")) {
			_settings.trialFeedbackType = static_cast<TrialFeedback>(exp.getChild("trialFeedback").getIntValue());

			// depending on trial feedback, check if a performance metric is given in the XML
			switch (_settings.trialFeedbackType) {
			case TrialFeedback::RMSE:
				_settings.trialPerformanceThreshold = exp.getChild("trialPerformanceThreshold").getDoubleValue();
				break;
			case TrialFeedback::MT:
				_settings.trialMovementTimeRangeSec[0] = exp.getChild("trialMTRangeLower").getDoubleValue();
				_settings.trialMovementTimeRangeSec[1] = exp.getChild("trialMTRangeUpper").getDoubleValue();
				break;
			}
		}
		else { _settings.trialFeedbackType = TrialFeedback::NONE; } // trialFeedback is either 0 or not present

		//  display settings 
		if (exp.getChild("displayType")) _settings.displayType = static_cast<DisplayType>(exp.getChild("displayType").getIntValue());

		if (exp.getChild("cursorShape")) _settings.cursorShape = static_cast<ParticleShape>(exp.getChild("cursorShape").getIntValue());


		// check which BROS need to be active
		_settings.activeBROSIDs.clear();
		auto ab = exp.getChild("activeBROSID");
		if (ab) {
			auto ids = ab.getChildren();
			for (auto & id : ids) {
				_settings.activeBROSIDs.push_back(id.getIntValue());
			}
		}
		else {
			// user did not include BROSID in the XML file. Use default  (BROS1 & BROS2)
			_settings.activeBROSIDs.push_back(1); // BROS 1
			_settings.activeBROSIDs.push_back(2); // BROS 2
		}

		// target particle count
		if (exp.getChild("targetParticleCount")) _settings.targetParticleCount = exp.getChild("targetParticleCount").getIntValue();

		// target particle period (how long each particle lives)
		if (exp.getChild("targetParticlePeriod")) _settings.targetParticlePeriod = exp.getChild("targetParticlePeriod").getDoubleValue();

		// target particle child (small)
		if (exp.getChild("targetParticleSize")) _settings.targetParticleSize = exp.getChild("targetParticleSize").getDoubleValue();

		//
		// load blocks and trials
		//

		int trialNumber = 0;
		int blockNumber = 0;
		auto blcks = exp.getChildren("block");

		for (auto & blck : blcks) {
			blockData block;

			// read block data
			block.blockNumber = ++blockNumber;
			// general settings first
			if (blck.getChild("breakDuration")) block.breakDuration = blck.getChild("breakDuration").getDoubleValue();
			if (blck.getChild("homingType")) block.homingType = blck.getChild("homingType").getIntValue();

			// read trials
			auto trls = blck.getChildren("trial");
			for (auto & trl : trls) {
				// read and store trial data
				trialData trial;
				trial.trialNumber = ++trialNumber;

				// trial settings
				if (trl.getChild("condition")) trial.condition = trl.getChild("condition").getIntValue();
				if (trl.getChild("breakDuration")) trial.breakDuration = trl.getChild("breakDuration").getDoubleValue();
				if (trl.getChild("trialDuration")) trial.trialDuration = trl.getChild("trialDuration").getDoubleValue();
				if (trl.getChild("trialRandomization")) trial.trialRandomization = trl.getChild("trialRandomization").getDoubleValue();

				// connection settings
				if (trl.getChild("connected")) { trial.connected = trl.getChild("connected").getBoolValue(); }
				else { trial.connected = false; }
				if (trl.getChild("connectionStiffness")) trial.connectionStiffness = trl.getChild("connectionStiffness").getDoubleValue();
				if (trl.getChild("connectionDamping")) trial.connectionDamping = trl.getChild("connectionDamping").getDoubleValue();

				// check connection types
				trial.connectedTo.clear();
				for (auto id : _settings.activeBROSIDs) trial.connectedTo.push_back(ConnectedToTypes::HUMANPARTNER); // DEFAULT
				

				// get target position normal distribution
				auto tpv = trl.getChild("targetPosSD");
				if (tpv) {
					trial.targetPosSD.clear();
					auto ids = tpv.getChildren();
					for (auto & id : ids) {
						trial.targetPosSD.push_back(id.getDoubleValue());
					}
				}
				

				// get target velocity normal distribution
				auto tvv = trl.getChild("targetVelSD");
				if (tvv) {
					trial.targetVelSD.clear();
					auto ids = tvv.getChildren();
					for (auto & id : ids) {
						trial.targetVelSD.push_back(id.getDoubleValue());
					}
				}

																													 // check fit virtual partner
				//if (XML.exists("fitVirtualPartner")) {

				//	// read which partners we need to fit the virtual partner model to
				//	XML.setTo("fitVirtualPartner");
				//	if (XML.setToChild(0)) {
				//		do {
				//			trial.fitVPBROSIDs.push_back(XML.getIntValue());
				//		} while (XML.setToSibling());
				//		XML.setToParent(); // go back to brosX
				//	}
				//	if (trial.fitVPBROSIDs.size() > 0) {
				//		trial.fitVirtualPartner = true;
				//		ofLogVerbose() << "(" << typeid(this).name() << ") " << "FitVPBROSIDs: " << ofToString(trial.fitVPBROSIDs) << " for trial " << trial.trialNumber;
				//	}

				//	XML.setToParent();
				//}

				//// check if the virtual partner needs to run during this trial.
				//if (XML.exists("executeVirtualPartner")) {
				//	trial.executeVirtualPartner = XML.getBoolValue();
				//}

				block.trials.push_back(trial); // add trial to trials list
			}

			block.numTrials = block.trials.size();

			_settings.numTrials += block.numTrials;

			_blocks.push_back(block); // add trials to vTrialsBlocks list
		}

		//// virtual partner settings
		//if (XML.exists("doVirtualPartner")) { _settings.vpDoVirtualPartner = XML.getValue<bool>("doVirtualPartner"); }
		//else { _settings.vpDoVirtualPartner = false; }

		//// set fit on Heroc pc
		//if (XML.exists("vpFitOnHeRoC")) { _settings.vpFitOnHeRoC = XML.getValue<bool>("vpFitOnHeRoC"); }

		_experimentLoaded = true;	

		// print xml to console for debugging
		ofBuffer auxBuffer;
		string xmlText;
		auxBuffer.set(XML.toString("  "));
		xmlText.clear();
		auto i = 0;
		for (auto & line : auxBuffer.getReverseLines()) {
			xmlText = line + "\n" + xmlText;
		}

		// print protocol to terminal (debuig)
		cout << xmlText << endl;
	}
}
