#include "ofAppExperiment.h"

//--------------------------------------------------------------
ofAppExperiment::ofAppExperiment()
{
	//
}

//--------------------------------------------------------------
void ofAppExperiment::start()
{
	// reset counters (0 based)
	_currentBlockNumber = 0;
	_currentTrialNumber = 0;

	// start trial
}

//--------------------------------------------------------------
void ofAppExperiment::stop()
{

}

//--------------------------------------------------------------
void ofAppExperiment::pause()
{

}

//--------------------------------------------------------------
void ofAppExperiment::resume()
{

}

//--------------------------------------------------------------
void ofAppExperiment::startTrail()
{
	// get (copy?) current block and current trial number
	_currentBlock = _blocks.at(_currentBlockNumber);
	_currentTrial = _currentBlock.trials.at(_currentTrialNumber);

	// request homing - once homing is done, the function 'eventAtHome()' is called by ofAppMain instance
	if (!mainApp->systemIsInState(399)) mainApp->requestStateChange(302);
}

//--------------------------------------------------------------
void ofAppExperiment::eventTrialDone()
{
	// called by ofAppMain (event from ADS)
	cout << "Trial done" << '\n';
	// start break counter
}

//--------------------------------------------------------------
void ofAppExperiment::eventCountDownDone()
{
	// called by ofAppMain (event from ADS)
	cout << "Countdown done" << '\n';

	// start break counter
}

//--------------------------------------------------------------
void ofAppExperiment::eventPauseDone()
{
	// called by ofAppMain (event from ADS)
	cout << "Pause done" << '\n';

	// start break counter
}

//--------------------------------------------------------------
void ofAppExperiment::eventAtHome()
{
	if (_experimentStarted) {
		startTrail(); // go to startTrial state machine
	}
}


//--------------------------------------------------------------
void ofAppExperiment::loadExperimentXML()
{
	_correctExpLoaded = false;

	//Open the Open File Dialog
	ofFileDialogResult openFileResult = ofSystemLoadDialog("Select an experiment XML file (.xml)");

	
	//Check if the user opened a file
	if (openFileResult.bSuccess) {
		
		ofLogVerbose("User opened file" + openFileResult.fileName);

		//We have a file, check it and process it
		processOpenFileSelection(openFileResult);
	}
	else {
		ofLogVerbose("User hit cancel");
	}
	
}

//--------------------------------------------------------------
void ofAppExperiment::processOpenFileSelection(ofFileDialogResult openFileResult)
{
	_currentBlockNumber = 0;
	_currentTrialNumber = 0;
	_numBlocks = 0;
	_numTrials = 0;

	if (XML.load(openFileResult.getPath() + openFileResult.getName())) {
		ofLogVerbose("Loaded: " + openFileResult.getPath() + openFileResult.getName());
	}

	// experiment settings (attributes)
	if (XML.exists("experiment")) {
		//XML.getAttribute();
	}
	else {
		// default experiment settings
	}

	int trialNumber = 0;
	int blockNumber = 0;

	// load blocks and trials
	if (XML.exists("block")) {
		XML.setTo("block[0]"); // set to first block

		do {
			blockData block;

			// read block data
			block.blockNumber = ++blockNumber;
			if (XML.getValue<double>("breakDuration")) block.breakDuration = XML.getValue<double>("breakDuration");
			if (XML.getValue<int>("homingType")) block.homingType = XML.getValue<int>("homingType");

			// set our "current" trial to the first one
			if (XML.getName() == "block" && XML.setTo("trial[0]"))
			{
				vector<trialData> trials;
				// read each trial
				do {
					// read and store trial data
					trialData trial;

					trial.trialNumber = ++trialNumber;
					if (XML.getValue<int>("condition")) trial.condition = XML.getValue<int>("condition");
					if (XML.getValue<bool>("connected")) trial.connected = XML.getValue<bool>("connected");
					if (XML.getValue<double>("connectionStiffness")) trial.connectionStiffness = XML.getValue<double>("connectionStiffness");
					if (XML.getValue<double>("countDownDuration")) trial.countDownDuration = XML.getValue<double>("countDownDuration");
					if (XML.getValue<double>("pauseDuration")) trial.pauseDuration = XML.getValue<double>("pauseDuration");
					if (XML.getValue<double>("trialDuration")) trial.trialDuration = XML.getValue<double>("trialDuration");

					trials.push_back(trial); // add trial to (temporary) trials list
				} 
				while (XML.setToSibling()); // go the next trial		

				std::reverse(trials.begin(), trials.end()); // since we pushed all trials back, call reverse

				block.trials = trials;	// add trials to block struct

				block.numTrials = trials.size();
				_numTrials += block.numTrials;

				_blocks.push_back(block); // add trials to vTrialsBlocks list

				XML.setToParent(); // go back up to the block level
			}

		} 
		while (XML.setToSibling()); // go to the next block

		std::reverse(_blocks.begin(), _blocks.end()); // again, reverse block vector (now in correct order)

		_numBlocks = _blocks.size();

		_correctExpLoaded = true; // assume the loaded experiment is correct.
	}

}

