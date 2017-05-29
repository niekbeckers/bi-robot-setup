#include "ofAppExperiment.h"

using namespace std;

//--------------------------------------------------------------
void ofAppExperiment::setup()
{
	setupTCADS();	// setup TwinCAT ADS
	setExperimentState(ExperimentState::IDLE);
}

//--------------------------------------------------------------
void ofAppExperiment::update()
{
	double time = ofGetElapsedTimef();

	bool nowTrialRunning = (bool)mainApp->AdsData[8];

	// check whether the system is in error
	if (mainApp->systemIsInError()) {
		setExperimentState(ExperimentState::IDLE);
	}

	//
	// experiment state machine
	//
	switch(_expState) {
	case ExperimentState::IDLE:
		// do nothing
		break;

	case ExperimentState::EXPERIMENTSTART:
		_currentBlockNumber = 0;
		_currentTrialNumber = 0;
		_experimentRunning = true;

		setExperimentState(ExperimentState::NEWBLOCK);
		break;

	case ExperimentState::EXPERIMENTSTOP:
		_experimentRunning = false;
		display1->drawTask = true;
		display2->drawTask = true;

		setExperimentState(ExperimentState::EXPERIMENTDONE);
		break;

	case ExperimentState::EXPERIMENTPAUSE:
		break;

	case ExperimentState::EXPERIMENTCONTINUE:
		break;

	case ExperimentState::NEWBLOCK:
		_currentBlock = _blocks[_currentBlockNumber];
		_currentTrialNumber = 0;
		mainApp->lblBlockNumber = _currentBlockNumber + 1;
		mainApp->lblTrialNumber.setMax(_currentBlock.trials.size());

		// start first trial
		setExperimentState(ExperimentState::NEWTRIAL);
		break;

	case ExperimentState::NEWTRIAL:

		if (_experimentPaused) { break; }

		_currentTrial = _currentBlock.trials[_currentTrialNumber];
		mainApp->lblTrialNumber = _currentTrialNumber + 1;

		// send trial data to ADS
		setTrialDataADS();

		// check if the robot is at home position
		//if (!mainApp->systemIsInState(399)) {
		if (false) {
			mainApp->requestStateChange(_currentBlock.homingType);
			setExperimentState(ExperimentState::HOMINGBEFORE);
		}
		else {
			setExperimentState(ExperimentState::HOMINGBEFOREDONE);
		}
		break;

	case ExperimentState::HOMINGBEFORE:
		// when robot is in state 399 (at home), start countdown
		if (mainApp->systemIsInState(399)) {
			setExperimentState(ExperimentState::HOMINGBEFOREDONE);
		}

		break;
	case ExperimentState::HOMINGBEFOREDONE:
		_getReadyStartTime = time;
		display1->showMessage(true, "GET READY");
		display2->showMessage(true, "GET READY");
		setExperimentState(ExperimentState::GETREADY);
		break;
		
	case ExperimentState::GETREADY:
		if ((time - _getReadyStartTime) <= _getReadyDuration) {
			//
		}
		else {
			display1->showMessage(false, "");
			display2->showMessage(false, "");
			setExperimentState(ExperimentState::GETREADYDONE);
		}
		break;

	case ExperimentState::GETREADYDONE:

		// homing is done, so make sure the robot is in run mode!
		//mainApp->requestStateChange(4);

		if (_cdDuration < 0.0) {
			// if countdown is negative (i.e. no countdown needed), return
			setExperimentState(ExperimentState::COUNTDOWNDONE);
		}
		else {
			// start countdown
			_cdStartTime = ofGetElapsedTimef();
			display1->drawTask = true;
			display2->drawTask = true;
			setExperimentState(ExperimentState::COUNTDOWN);
		}
		break;

	case ExperimentState::COUNTDOWN:
		if ((time - _cdStartTime) <= _cdDuration) {
			double cdTimeRemaining = _cdDuration - (time - _cdStartTime);
			display1->showMessage(true, "COUNTDOWN");
			display2->showMessage(true, "COUNTDOWN");
			display1->showCountDown(true, cdTimeRemaining, _cdDuration);
			display2->showCountDown(true, cdTimeRemaining, _cdDuration);
		}
		else {
			setExperimentState(ExperimentState::COUNTDOWNDONE);
		}
		break;

	case ExperimentState::COUNTDOWNDONE:
		// countdown done, start trial
		display1->showMessage(false);
		display2->showMessage(false);
		display1->showCountDown(false);
		display2->showCountDown(false);
		display1->drawTask = true;
		display2->drawTask = true;

		requestStartTrialADS();

		display1->drawTask = true;
		setExperimentState(ExperimentState::TRIALRUNNING);
		break;

	case ExperimentState::TRIALRUNNING:
		// check if trial is running (data from simulink)
		if (prevTrialRunning && !nowTrialRunning) {
			setExperimentState(ExperimentState::TRIALDONE);
		}
		break;

	case ExperimentState::TRIALDONE:
		// set display to black
		display1->drawTask = false;
		display2->drawTask = false;

		// call for trial after homing
		//if (!mainApp->systemIsInState(399)) {
		if (false) {
			mainApp->requestStateChange(_currentBlock.homingType);
			setExperimentState(ExperimentState::HOMINGAFTER);
		}
		else {
			setExperimentState(ExperimentState::HOMINGAFTERDONE);
		}
		break;

	case ExperimentState::HOMINGAFTER:
		// check whether system is 'at home' (399)
		if (mainApp->systemIsInState(399)) setExperimentState(ExperimentState::HOMINGAFTERDONE);
		break;

	case ExperimentState::HOMINGAFTERDONE:
		setExperimentState(ExperimentState::CHECKNEXTSTEP);
		break;

	case ExperimentState::CHECKNEXTSTEP:

		if (_currentTrialNumber < _currentBlock.trials.size() - 1) {
			// new trial in block, go to trial break
			_breakStartTime = ofGetElapsedTimef();
			setExperimentState(ExperimentState::TRIALBREAK);
		}
		else if (_currentTrialNumber == _currentBlock.trials.size() - 1) {
			// end of block, determine if we proceed to the next block, or experiment is done
			if (_currentBlockNumber < _blocks.size() - 1) {
				// new block! First, block break
				_breakStartTime = ofGetElapsedTimef();
				setExperimentState(ExperimentState::BLOCKBREAK);
			}
			else {
				// all blocks are done, experiment is done
				setExperimentState(ExperimentState::EXPERIMENTSTOP);
			}
		}
		break;

	case ExperimentState::TRIALBREAK:
		if ((time - _breakStartTime) <= _currentTrial.breakDuration) {
			// trial break is running, show feedback on display
			double timeRemaining = _currentTrial.breakDuration - (time - _breakStartTime);
			string msg = "BREAK\n" + secToMin(timeRemaining) + " remaining";
			display1->showMessage(true, msg);
			display2->showMessage(true, msg);
		}
		else {
			display1->showMessage(false);
			display2->showMessage(false);
			setExperimentState(ExperimentState::TRIALBREAKDONE);
		}
		break;

	case ExperimentState::TRIALBREAKDONE:
		// trial break done, on to the next trial!
		_currentTrialNumber++;
		setExperimentState(ExperimentState::NEWTRIAL);
		break;

	case ExperimentState::BLOCKBREAK:
		if ((time - _breakStartTime) <= _currentBlock.breakDuration) {
			// block break is running, show feedback on display
			double timeRemaining = _currentBlock.breakDuration - (time - _breakStartTime);
			string msg = "BREAK\n" + secToMin(timeRemaining) + " remaining";
			display1->showMessage(true, msg);
			display2->showMessage(true, msg);
		}
		else {
			display1->showMessage(false);
			display2->showMessage(false);
			setExperimentState(ExperimentState::BLOCKBREAKDONE);
		}
		break;

	case ExperimentState::BLOCKBREAKDONE:
		// block break done, on to the next block!
		_currentBlockNumber++;
		setExperimentState(ExperimentState::NEWBLOCK);
		break;

	case ExperimentState::EXPERIMENTDONE:
		break;
	}

	prevTrialRunning = nowTrialRunning;
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

	char szVar6[] = { "Object1 (ModelBaseBROS).ModelParameters.ExpTrialRandom_Value" };
	_lHdlVar_Write_TrialRandom = _tcClient->getVariableHandle(szVar6, sizeof(szVar6));
}

//--------------------------------------------------------------
string ofAppExperiment::secToMin(double seconds)
{
	int minutes = seconds / 60;
	int remseconds = (int)seconds % 60;

	return ofToString(minutes) + ":" + ofToString(remseconds, 2, '0');
}

//--------------------------------------------------------------
void ofAppExperiment::startExperiment()
{
	if (_experimentLoaded && !_experimentRunning) {
		display1->drawTask = false;
		display2->drawTask = false;
		setExperimentState(ExperimentState::EXPERIMENTSTART);
	}
}

//--------------------------------------------------------------
void ofAppExperiment::stopExperiment()
{
	if (_experimentRunning) setExperimentState(ExperimentState::EXPERIMENTSTOP);
}

//--------------------------------------------------------------
void ofAppExperiment::pauseExperiment()
{
	_experimentPaused = true;
}

//--------------------------------------------------------------
void ofAppExperiment::resumeExperiment()
{
	_experimentPaused = false;
}

//--------------------------------------------------------------
void ofAppExperiment::setTrialDataADS()
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

	// trial randomization (phase set)
	int var5 = _currentTrial.trialRandomization;
	_tcClient->write(_lHdlVar_Write_TrialRandom, &var5, sizeof(var5));
}

//--------------------------------------------------------------
void ofAppExperiment::setExperimentState(ExperimentState newState)
{ 
	_expState = newState; 
	experimentStateLabel = StringExperimentStateLabel(newState); 
	mainApp->lblExpState = experimentStateLabel;
}

//--------------------------------------------------------------
void ofAppExperiment::requestStartTrialADS()
{
	// signal start trial to simulink, then await for trial done, then start break
	double var = 1.0;
	_tcClient->write(_lHdlVar_Write_StartTrial, &var, sizeof(var));
	var = 0.0;
	_tcClient->write(_lHdlVar_Write_StartTrial, &var, sizeof(var));
}

//--------------------------------------------------------------
void ofAppExperiment::loadExperimentXML()
{
	setExperimentState(ExperimentState::IDLE);
	//Open the Open File Dialog
	ofFileDialogResult openFileResult = ofSystemLoadDialog("Select an experiment XML file (.xml)",false, ofFilePath().getCurrentExeDir());
	ofLogVerbose(ofFilePath().getCurrentExePath());
	//Check if the user opened a file
	if (openFileResult.bSuccess) {
		ofLogVerbose("User opened file " + openFileResult.fileName);

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
	_numTrials = 0;

	mainApp->lblTrialNumber = _currentTrialNumber + 1;
	mainApp->lblBlockNumber = _currentBlockNumber + 1;
	mainApp->lblExpLoaded = openFileResult.fileName;

	if (XML.load(openFileResult.getPath())) {
		ofLogVerbose("Loaded: " + openFileResult.getPath());
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
					if (XML.getValue<double>("breakDuration")) trial.breakDuration = XML.getValue<double>("breakDuration");
					if (XML.getValue<double>("trialDuration")) trial.trialDuration = XML.getValue<double>("trialDuration");
					if (XML.getValue<int>("trialRandomization")) trial.trialRandomization = XML.getValue<int>("trialRandomization");

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

		_experimentLoaded = true;

		mainApp->lblTrialNumber.setMax(_blocks[0].trials.size());
		mainApp->lblBlockNumber.setMax(_blocks.size());
	}

}

