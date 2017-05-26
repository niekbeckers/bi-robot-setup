#include "ofAppExperiment.h"

//--------------------------------------------------------------
void ofAppExperiment::setup()
{
	setupTCADS();	// setup TwinCAT ADS
}



//--------------------------------------------------------------
void ofAppExperiment::update()
{
	double time = ofGetElapsedTimef();

	// countdown
	if (_cdRunning) {
		if ((time - _cdStartTime) <= _cdDuration) {
			// countdown running
			double cdTimeRemaining = _cdDuration - (time - _cdStartTime);
			//display1->setMessage(ofToString(cdTimeRemaining, 1) + " seconds");
			//display2->setMessage(ofToString(cdTimeRemaining, 1) + " seconds");
		}
		else {
			countDownDone();
		}
	}

	// break
	if (_trialBreakRunning) {
		if ((time - _trialBreakTime) <= _currentTrial.pauseDuration) {
			// break is running
		}
		else {
			// break is done
			trialBreakDone();
		}
	}
}

//--------------------------------------------------------------
void ofAppExperiment::exit()
{
	_tcClient->disconnect();	// clean up TC ADS client
}

//--------------------------------------------------------------
void ofAppExperiment::setupTCADS()
{
	_tcClient = new tcAdsClient(adsPort);

	char szVar0[] = { "Object1 (ModelBaseBROS).ModelParameters.ExpStartTrial_Value" };
	_lHdlVar_Write_StartTrial = _tcClient->getVariableHandle(szVar0, sizeof(szVar0));

	char szVar1[] = { "Object1 (ModelBaseBROS).ModelParameters.ExpCondition_Value" };
	_lHdlVar_Write_Condition = _tcClient->getVariableHandle(szVar1, sizeof(szVar1));

	char szVar2[] = { "Object1 (ModelBaseBROS).ModelParameters.ExpConnectionStiffness_Value" };
	_lHdlVar_Write_ConnectionStiffness = _tcClient->getVariableHandle(szVar2, sizeof(szVar2));

	char szVar3[] = { "Object1 (ModelBaseBROS).ModelParameters.ExpConnected_Value" };
	_lHdlVar_Write_Connected = _tcClient->getVariableHandle(szVar3, sizeof(szVar3));

	char szVar4[] = { "Object1 (ModelBaseBROS).ModelParameters.ExpTrialDuration_Value" };
	_lHdlVar_Write_TrialDuration = _tcClient->getVariableHandle(szVar4, sizeof(szVar4));

	char szVar5[] = { "Object1 (ModelBaseBROS).ModelParameters.ExpTrialNumber_Value" };
	_lHdlVar_Write_TrialNumber = _tcClient->getVariableHandle(szVar5, sizeof(szVar5));
}

//--------------------------------------------------------------
void ofAppExperiment::start()
{
	// reset counters (0 based)
	_currentBlockNumber = 0;
	_currentTrialNumber = 0;

	// start trial
	newTrial();
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
void ofAppExperiment::newTrial()
{
	// get (copy?) current block and current trial number
	_currentBlock = _blocks.at(_currentBlockNumber);
	_currentTrial = _currentBlock.trials.at(_currentTrialNumber);

	// prepare trial (send data to model through ADS)
	prepareTrial();

	// make sure we are at home 
	// if not, request homing - once homing is done, the function 'eventAtHome()' is called by ofAppMain instance
	/*if (!mainApp->systemIsInState(399)) {
		mainApp->requestStateChange(_currentBlock.homingType);
	}
	else {
		startCountDown();
	}*/

}

void ofAppExperiment::prepareTrial()
{
	// write trial data to ADS/Simulink model

	// trial number
	int var0 = _currentTrial.trialNumber;
	_tcClient->write(_lHdlVar_Write_TrialNumber, &var0, sizeof(var0));

	// connected
	bool var1 = _currentTrial.connected;
	_tcClient->write(_lHdlVar_Write_Connected, &var1, sizeof(var1));

	// connectionStiffness
	double var2 = _currentTrial.connectionStiffness;
	_tcClient->write(_lHdlVar_Write_ConnectionStiffness, &var2, sizeof(var2));

	// condition
	int var3 = _currentTrial.condition;
	_tcClient->write(_lHdlVar_Write_Condition, &var3, sizeof(var3));
	
	// trialDuration
	if (_currentTrial.trialDuration < 0.0) {
		double var4 = _currentTrial.trialDuration;
		_tcClient->write(_lHdlVar_Write_TrialDuration, &var4, sizeof(var4));
	}
	

}

//--------------------------------------------------------------
void ofAppExperiment::startCountDown()
{
	// if countdown is negative (i.e. no countdown needed), return
	if (_cdDuration < 0.0) return;

	_cdStartTime = ofGetElapsedTimef();
	_cdRunning = true;

	// set message
	//display1->showMessage(true);
	//display2->showMessage(true);
	//display1->setMessage(ofToString(_cdDuration,1) + " seconds");
	//display2->setMessage(ofToString(_cdDuration, 1) + " seconds");
}

//--------------------------------------------------------------
void ofAppExperiment::startTrial()
{
	// signal start trial to simulink, then await for trial done, then start break
	double var = 1.0;
	_tcClient->write(_lHdlVar_Write_StartTrial, &var, sizeof(var));
	var = 0.0;
	_tcClient->write(_lHdlVar_Write_StartTrial, &var, sizeof(var));
}



//--------------------------------------------------------------
void ofAppExperiment::countDownDone()
{
	// called by ofAppMain (event from ADS)
	cout << "Countdown done" << '\n';

	// countdown done
	_cdRunning = false;

	// switch message off
	//display1->showMessage(false);
	//display2->showMessage(false);

	// start trial
	startTrial();
}

//--------------------------------------------------------------
void ofAppExperiment::trialBreakDone()
{
	// trial break is done
	if (_currentTrialNumber < _currentBlock.trials.size() - 1) {
		_currentTrialNumber++;
		
		newTrial();	// start new trial

	}
	else if (_currentTrialNumber == _currentBlock.trials.size() - 1) {
		if (_currentBlockNumber < _blocks.size() - 1) {
			// do pause, at end of pause, start new trial
			_currentBlockNumber++;
			_currentTrialNumber = 0;

			blockDone();
		}
		else {
			// all blocks done, experiment done
			experimentDone();
		}
	}
	// check if new trial, or block is done, 
}

//--------------------------------------------------------------
void ofAppExperiment::blockDone()
{
	// called by ofAppMain (event from ADS)
	cout << "Pause done" << '\n';

	// start break counter
}

//--------------------------------------------------------------
void ofAppExperiment::experimentDone()
{
	// experiment done
}

//--------------------------------------------------------------
void ofAppExperiment::eventTrialDone()
{
	// called by ofAppMain (event from ADS - ofAppMain)
	cout << "Trial done" << '\n';

	// start break
	_trialBreakRunning = true;
	_trialBreakTime = ofGetElapsedTimef();
}

//--------------------------------------------------------------
void ofAppExperiment::eventAtHome()
{
	if (_experimentStarted) {
		startCountDown();
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
		if (XML.getValue<double>("countDownDuration")) _cdDuration = XML.getValue<double>("countDownDuration");
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

