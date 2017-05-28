#include "ofAppExperiment.h"

using namespace std;

//--------------------------------------------------------------
void ofAppExperiment::setup()
{
	setupTCADS();	// setup TwinCAT ADS
}

//--------------------------------------------------------------
void ofAppExperiment::update()
{
	double time = ofGetElapsedTimef();

	bool nowTrialRunning = (bool)mainApp->AdsData[8];

	// experiment state machine
	switch(_expState) {
	case ExperimentState::IDLE:
		// do nothing
		break;

	case ExperimentState::EXPERIMENTSTART:
		_currentBlockNumber = 0;
		_currentTrialNumber = 0;
		_expState = ExperimentState::NEWBLOCK;
		break;

	case ExperimentState::EXPERIMENTSTOP:
		break;

	case ExperimentState::EXPERIMENTPAUSE:
		break;

	case ExperimentState::EXPERIMENTCONTINUE:
		break;

	case ExperimentState::NEWBLOCK:
		_currentBlock = _blocks[_currentBlockNumber];
		_currentTrialNumber = 0;

		// start first trial
		_expState = ExperimentState::NEWTRIAL;
		break;

	case ExperimentState::NEWTRIAL:
		_currentTrial = _currentBlock.trials[_currentTrialNumber];

		// send trial data to ADS
		setTrialDataADS();

		if (!mainApp->systemIsInState(399)) {
			mainApp->requestStateChange(_currentBlock.homingType);
			_expState = ExperimentState::HOMINGBEFORE;
		}
		
		break;

	case ExperimentState::HOMINGBEFORE:
		// when robot is in state 399 (at home), start countdown
		if (mainApp->systemIsInState(399)) _expState = ExperimentState::HOMINGBEFOREDONE;
		break;
		
	case ExperimentState::HOMINGBEFOREDONE:
		if (_cdDuration < 0.0) {
			// if countdown is negative (i.e. no countdown needed), return
			_expState = ExperimentState::COUNTDOWNDONE;
		}
		else {
			// start countdown
			_cdStartTime = ofGetElapsedTimef();
			display1->drawTask = true;
			display2->drawTask = true;
			_expState = ExperimentState::COUNTDOWN;
		}
		break;

	case ExperimentState::COUNTDOWN:
		if ((time - _cdStartTime) <= _cdDuration) {
			double cdTimeRemaining = _cdDuration - (time - _cdStartTime);
			string msg = ofToString(cdTimeRemaining, 1) + " seconds";
			display1->showMessage(true);
			display2->showMessage(true);
			display1->setMessage(msg);
			display2->setMessage(msg);
		}
		else {
			_expState = ExperimentState::COUNTDOWNDONE;
		}
		break;

	case ExperimentState::COUNTDOWNDONE:
		// countdown done, start trial
		display1->showMessage(false);
		display2->showMessage(false);

		requestStartTrialADS();

		_expState = ExperimentState::TRIALRUNNING;
		break;

	case ExperimentState::TRIALRUNNING:
		// check if trial is running (data from simulink)
		if (prevTrialRunning && !nowTrialRunning) {
			_expState = ExperimentState::TRIALDONE;
		}
		break;

	case ExperimentState::TRIALDONE:
		// set display to black
		display1->drawTask = false;
		display2->drawTask = false;

		// call for trial after homing
		if (!mainApp->systemIsInState(399)) {
			mainApp->requestStateChange(_currentBlock.homingType);
			_expState = ExperimentState::HOMINGAFTER;
		}
		break;

	case ExperimentState::HOMINGAFTER:
		if (mainApp->systemIsInState(399)) _expState = ExperimentState::HOMINGAFTERDONE;
		break;

	case ExperimentState::HOMINGAFTERDONE:
		_expState = ExperimentState::CHECKNEXTSTEP;
		break;

	case ExperimentState::CHECKNEXTSTEP:

		if (_currentTrialNumber < _currentBlock.trials.size() - 1) {
			// new trial in block, go to trial break
			_breakStartTime = ofGetElapsedTimef();
			_expState = ExperimentState::TRIALBREAK;
		}
		else if (_currentTrialNumber == _currentBlock.trials.size() - 1) {
			// end of block, determine if we proceed to the next block, or experiment is done
			if (_currentBlockNumber < _blocks.size() - 1) {
				// new block! First, block break
				_breakStartTime = ofGetElapsedTimef();
				_expState = ExperimentState::BLOCKBREAK;
			}
			else {
				// all blocks are done, experiment is done
				_expState = ExperimentState::EXPERIMENTDONE;
			}
		}
		break;

	case ExperimentState::TRIALBREAK:
		if ((time - _breakStartTime) <= _currentTrial.pauseDuration) {
			// trial break is running, show feedback on display
			double timeRemaining = _currentTrial.pauseDuration - (time - _breakStartTime);
			string msg = "BREAK  -  " + ofToString(timeRemaining, 0) + " seconds remaining";
			display1->showMessage(true);
			display2->showMessage(true);
			display1->setMessage(msg);
			display2->setMessage(msg);
		}
		else {
			display1->showMessage(false);
			display2->showMessage(false);
			_expState = ExperimentState::TRIALBREAKDONE;
		}
		break;

	case ExperimentState::TRIALBREAKDONE:
		// trial break done, on to the next trial!
		_currentTrialNumber++;
		_expState = ExperimentState::NEWTRIAL;
		break;

	case ExperimentState::BLOCKBREAK:
		if ((time - _breakStartTime) <= _currentBlock.breakDuration) {
			// block break is running, show feedback on display
			double timeRemaining = _currentBlock.breakDuration - (time - _breakStartTime);
			string msg = "BREAK  -  " + ofToString(timeRemaining, 0) + " seconds remaining";
			display1->showMessage(true);
			display2->showMessage(true);
			display1->setMessage(msg);
			display2->setMessage(msg);
		}
		else {
			display1->showMessage(false);
			display2->showMessage(false);
			_expState = ExperimentState::BLOCKBREAKDONE;
		}
		break;

	case ExperimentState::BLOCKBREAKDONE:
		// block break done, on to the next block!
		_currentBlockNumber++;
		_expState = ExperimentState::NEWBLOCK;
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
}

//--------------------------------------------------------------
void ofAppExperiment::start()
{
	_expState = ExperimentState::EXPERIMENTSTART;
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
	int _expState = ExperimentState::IDLE;
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
	}

}

