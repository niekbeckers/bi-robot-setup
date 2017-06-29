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

	_nowTrialRunning = (bool)mainApp->AdsData[8];

	// check whether the system is in error
	if (mainApp->systemIsInError()) {
		_experimentRunning = false;
		setExperimentState(ExperimentState::SYSTEMFAULT); 
	}

	//
	// experiment state machine
	//
	switch(_expState) {
	case ExperimentState::IDLE:
		// do nothing
		break;
	case ExperimentState::SYSTEMFAULT:
		break;
	case ExperimentState::EXPERIMENTSTART:
		esmExperimentStart();
		break;
	case ExperimentState::EXPERIMENTSTOP:
		esmExperimentStop();
		break;
	case ExperimentState::EXPERIMENTPAUSE:
		break;
	case ExperimentState::EXPERIMENTCONTINUE:
		break;
	case ExperimentState::NEWBLOCK:
		esmNewBlock();
		break;
	case ExperimentState::NEWTRIAL:
		esmNewTrial();
		break;
	case ExperimentState::HOMINGBEFORE:
		esmHomingBefore();
		break;
	case ExperimentState::HOMINGBEFOREDONE:
		esmHomingBeforeDone();
		break;
	case ExperimentState::GETREADY:
		esmGetReady();
		break;
	case ExperimentState::GETREADYDONE:
		esmGetReadyDone();
		break;
	case ExperimentState::COUNTDOWN:
		esmCountdown();
		break;
	case ExperimentState::COUNTDOWNDONE:
		esmCountdownDone();
		break;
	case ExperimentState::TRIALRUNNING:
		esmTrialRunning();
		break;
	case ExperimentState::TRIALDONE:
		esmTrialDone();
		break;
	case ExperimentState::TRIALFEEDBACK:
		esmTrialFeedback();
		break;
	case ExperimentState::HOMINGAFTER:
		esmHomingAfter();
		break;
	case ExperimentState::HOMINGAFTERDONE:
		esmHomingAfterDone();
		break;
	case ExperimentState::CHECKNEXTSTEP:
		esmCheckNextStep();
		break;
	case ExperimentState::TRIALBREAK:
		esmTrialBreak();
		break;
	case ExperimentState::TRIALBREAKDONE:
		esmTrialBreakDone();
		break;
	case ExperimentState::BLOCKBREAK:
		esmBlockBreak();
		break;
	case ExperimentState::BLOCKBREAKDONE:
		esmBlockBreakDone();
		break;
	case ExperimentState::EXPERIMENTDONE:
		break;
	}

	_prevTrialRunning = _nowTrialRunning;
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

	char szVar0[] = { "Object1 (ModelBROS).ModelParameters.ExpStartTrial_Value" };
	_lHdlVar_Write_StartTrial = _tcClient->getVariableHandle(szVar0, sizeof(szVar0));

	char szVar1[] = { "Object1 (ModelBROS).ModelParameters.ExpCondition_Value" };
	_lHdlVar_Write_Condition = _tcClient->getVariableHandle(szVar1, sizeof(szVar1));

	char szVar2[] = { "Object1 (ModelBROS).ModelParameters.KpConnection_Value" };
	_lHdlVar_Write_ConnectionStiffness = _tcClient->getVariableHandle(szVar2, sizeof(szVar2));

	char szVar3[] = { "Object1 (ModelBROS).ModelParameters.Connected_Value" };
	_lHdlVar_Write_Connected = _tcClient->getVariableHandle(szVar3, sizeof(szVar3));

	char szVar4[] = { "Object1 (ModelBROS).ModelParameters.ExpTrialDuration_Value" };
	_lHdlVar_Write_TrialDuration = _tcClient->getVariableHandle(szVar4, sizeof(szVar4));

	char szVar5[] = { "Object1 (ModelBROS).ModelParameters.ExpTrialNumber_Value" };
	_lHdlVar_Write_TrialNumber = _tcClient->getVariableHandle(szVar5, sizeof(szVar5));

	char szVar6[] = { "Object1 (ModelBROS).ModelParameters.ExpTrialRandom_Value" };
	_lHdlVar_Write_TrialRandom = _tcClient->getVariableHandle(szVar6, sizeof(szVar6));

	char szVar7[] = { "Object1 (ModelBROS).ModelParameters.KdConnection_Value" };
	_lHdlVar_Write_ConnectionDamping = _tcClient->getVariableHandle(szVar7, sizeof(szVar7));

	char szVar8[] = { "Object1 (ModelBROS).BlockIO.PerformanceFeedback" };
	_lHdlVar_Read_PerformanceFeedback = _tcClient->getVariableHandle(szVar8, sizeof(szVar8));
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
		if (!debugMode) {
			display1->drawTask = false;
			display2->drawTask = false;
		}
		else {
			display1->drawTask = true;
			display2->drawTask = true;
		}

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
void ofAppExperiment::restartExperiment()
{

	// restart trial. Only possible when in error mode / idle mode
	if ((_expState == ExperimentState::SYSTEMFAULT && _experimentRunning) || _expState == ExperimentState::IDLE) {

		if (!debugMode) {
			display1->drawTask = false;
			display2->drawTask = false;
		}

		_experimentRunning = true;

		// set new block
		esmNewBlock(_currentTrialNumber);

		setExperimentState(ExperimentState::NEWTRIAL);
	}

	
}

//--------------------------------------------------------------
void ofAppExperiment::setTrialDataADS()
{
	// write trial data to ADS/Simulink model

	// trial number
	_tcClient->write(_lHdlVar_Write_TrialNumber, &_currentTrial.trialNumber, sizeof(_currentTrial.trialNumber));

	// connected
	_tcClient->write(_lHdlVar_Write_Connected, &_currentTrial.connected, sizeof(_currentTrial.connected));

	// connectionStiffness
	_tcClient->write(_lHdlVar_Write_ConnectionStiffness, &_currentTrial.connectionStiffness, sizeof(_currentTrial.connectionStiffness));

	// connection damping
	_tcClient->write(_lHdlVar_Write_ConnectionDamping, &_currentTrial.connectionDamping, sizeof(_currentTrial.connectionDamping));

	// condition
	int c = _currentTrial.condition;
	_tcClient->write(_lHdlVar_Write_Condition, &c, sizeof(c));
	
	// trialDuration
	if (_currentTrial.trialDuration > 0.0) {
		_tcClient->write(_lHdlVar_Write_TrialDuration, &_currentTrial.trialDuration, sizeof(_currentTrial.trialDuration));
	}

	// trial randomization (phase set)
	_tcClient->write(_lHdlVar_Write_TrialRandom, &_currentTrial.trialRandomization, sizeof(_currentTrial.trialRandomization));
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

	// set labels in the GUI
	mainApp->lblTrialNumber = _currentTrialNumber + 1;
	mainApp->lblBlockNumber = _currentBlockNumber + 1;
	mainApp->lblExpLoaded = openFileResult.fileName;

	if (XML.load(openFileResult.getPath())) {
		ofLogVerbose("Loaded: " + openFileResult.getPath());
	}
	// experiment settings (attributes)
	if (XML.getValue<double>("countDownDuration")) { _cdDuration = XML.getValue<double>("countDownDuration"); }
	if (XML.getValue<int>("trialFeedback")) { 
		_trialFeedbackType = static_cast<TrialFeedback>(XML.getValue<int>("trialFeedback"));
		ofLogVerbose("TrialFeedback " + ofToString(_trialFeedbackType));
		// depending on trial feedback, check if a performance metric is given in the XML
		switch (_trialFeedbackType) {
		case TrialFeedback::RMSE:
			if (XML.getValue<int>("trialPerformanceThreshold")) { _trialPerformanceThreshold = XML.getValue<double>("trialPerformanceThreshold"); }
			break;
		case TrialFeedback::MT:
			if (XML.getValue<int>("trialMTRangeLower")) { _trialMovementTimeRangeSec[0] = XML.getValue<double>("trialMTRangeLower"); }
			if (XML.getValue<int>("trialMTRangeUpper")) { _trialMovementTimeRangeSec[1] = XML.getValue<double>("trialMTRangeUpper"); }
			break;
		}
	}
	else { _trialFeedbackType = TrialFeedback::NONE; } // trialFeedback is either 0 or not present


	int trialNumber = 0;
	int blockNumber = 0;

	// load blocks and trials
	if (XML.exists("block")) {
		XML.setTo("block[0]"); // set to first block

		do {
			blockData block;

			// read block data
			block.blockNumber = ++blockNumber;
			if (XML.getValue<double>("breakDuration")) { block.breakDuration = XML.getValue<double>("breakDuration"); }
			if (XML.getValue<int>("homingType")) { block.homingType = XML.getValue<int>("homingType"); }
			

			// set our "current" trial to the first one
			if (XML.getName() == "block" && XML.setTo("trial[0]"))
			{
				//vector<trialData> trials;
				// read each trial
				do {
					// read and store trial data
					trialData trial;
					trial.trialNumber = ++trialNumber;
					//if (XML.getValue<int>("condition")) 
					{ trial.condition = XML.getValue<int>("condition"); }
					if (XML.getValue<bool>("connected")) { trial.connected = true; } 
					else { trial.connected = false; }
					if (XML.getValue<double>("connectionStiffness")) { trial.connectionStiffness = XML.getValue<double>("connectionStiffness"); }
					if (XML.getValue<double>("connectionDamping")) { trial.connectionDamping = XML.getValue<double>("connectionDamping"); }
					if (XML.getValue<double>("breakDuration")) { trial.breakDuration = XML.getValue<double>("breakDuration"); }
					if (XML.getValue<double>("trialDuration")) { trial.trialDuration = XML.getValue<double>("trialDuration"); }
					trial.trialRandomization = XML.getValue<int>("trialRandomization");

					block.trials.push_back(trial); // add trial to (temporary) trials list

				} while (XML.setToSibling()); // go the next trial		

				block.numTrials = block.trials.size();
				_numTrials += block.numTrials;

				_blocks.push_back(block); // add trials to vTrialsBlocks list

				XML.setToParent(); // go back up to the block level
			}

		} while (XML.setToSibling()); // go to the next block

		_experimentLoaded = true;

		// set labels in the GUI
		mainApp->lblTrialNumber.setMax(_blocks[0].trials.size());
		mainApp->lblBlockNumber.setMax(_blocks.size());
	}
}

//--------------------------------------------------------------
void ofAppExperiment::setCurrentBlockNumber(int blockNr)
{
	if (!_experimentLoaded) { return; }

	if (blockNr > _blocks.size()) {
		blockNr = _blocks.size();
		ofLogWarning("ofAppExperiment::setCurrentBlockNumber","Block number out of bounds, reset to last block");
	}

	_currentBlockNumber = blockNr - 1;
	mainApp->lblBlockNumber = blockNr;
	mainApp->lblTrialNumber.setMax(_blocks[_currentBlockNumber].trials.size());
	mainApp->lblBlockNumber.setMax(_blocks.size());
}

//--------------------------------------------------------------
void ofAppExperiment::setCurrentTrialNumber(int trialNr)
{
	if (!_experimentLoaded) { return; }

	if (trialNr > _currentBlock.trials.size()) {
		trialNr = _currentBlock.trials.size();
		ofLogWarning("ofAppExperiment::setCurrentTrialNumber", "Trial number out of bounds, reset to last trial of this block");
	}
	_currentTrialNumber = trialNr - 1;
	mainApp->lblTrialNumber = trialNr;
}

//--------------------------------------------------------------
void ofAppExperiment::showVisualReward()
{
	//
	// BROS 1
	//
	double performanceDiff = _trialPerformance[0] - _trialPerformancePrev[0];

	if (performanceDiff < -_trialPerformanceThreshold) { // better performance compared to last trial

	}
	else if (performanceDiff > _trialPerformanceThreshold) { // worse performance compared to last trial

	}
	else {  // similar performance compared to last trial

	}

	//
	// BROS 2
	//
	performanceDiff = _trialPerformance[1] - _trialPerformancePrev[1];

	if (performanceDiff < -_trialPerformanceThreshold) { // better performance compared to last trial

	}
	else if (performanceDiff > _trialPerformanceThreshold) { // worse performance compared to last trial

	}
	else {  // similar performance compared to last trial

	}
}

//
// Experiment State Machine functions
//

//--------------------------------------------------------------
void ofAppExperiment::esmExperimentStart()
{
	_currentBlockNumber = 0;
	_currentTrialNumber = 0;
	_experimentRunning = true;

	setExperimentState(ExperimentState::NEWBLOCK);
}

//--------------------------------------------------------------
void ofAppExperiment::esmExperimentStop()
{
	_experimentRunning = false;

	display1->drawTask = true;
	display2->drawTask = true;

	setExperimentState(ExperimentState::EXPERIMENTDONE);
}

//--------------------------------------------------------------
void ofAppExperiment::esmNewBlock(int trialNumber)
{
	if (!_experimentRunning) { return; }

	_currentBlock = _blocks[_currentBlockNumber];
	_currentTrialNumber = trialNumber;
	mainApp->lblBlockNumber = _currentBlockNumber + 1;
	mainApp->lblTrialNumber.setMax(_currentBlock.trials.size());

	// start first trial
	setExperimentState(ExperimentState::NEWTRIAL);
}

//--------------------------------------------------------------
void ofAppExperiment::esmNewTrial()
{
	if (_experimentPaused || !_experimentRunning) { return; }

	_currentTrial = _currentBlock.trials[_currentTrialNumber];
	mainApp->lblTrialNumber = _currentTrialNumber + 1;

	// send trial data to ADS
	setTrialDataADS();

	// check if the robot is at home position
	if (!mainApp->systemIsInState(SystemState::ATHOME)) {
		mainApp->requestStateChange(static_cast<SystemState>(_currentBlock.homingType));
		setExperimentState(ExperimentState::HOMINGBEFORE);
	}
	else {
		setExperimentState(ExperimentState::HOMINGBEFOREDONE);
	}
}

//--------------------------------------------------------------
void ofAppExperiment::esmHomingBefore()
{
	// when robot is in state 399 (at home), start countdown
	if (mainApp->systemIsInState(SystemState::ATHOME)) {
		setExperimentState(ExperimentState::HOMINGBEFOREDONE);
	}
}

//--------------------------------------------------------------
void ofAppExperiment::esmHomingBeforeDone()
{
	_getReadyStartTime = ofGetElapsedTimef();
	display1->showMessage(true, "GET READY");
	display2->showMessage(true, "GET READY");
	setExperimentState(ExperimentState::GETREADY);
}

//--------------------------------------------------------------
void ofAppExperiment::esmGetReady()
{
	if ((ofGetElapsedTimef() - _getReadyStartTime) <= _getReadyDuration) {
		//
	}
	else {
		display1->showMessage(false, "");
		display2->showMessage(false, "");
		setExperimentState(ExperimentState::GETREADYDONE);
	}
}

//--------------------------------------------------------------
void ofAppExperiment::esmGetReadyDone()
{
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
}

//--------------------------------------------------------------
void ofAppExperiment::esmCountdown()
{
	double time = ofGetElapsedTimef();
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
}

//--------------------------------------------------------------
void ofAppExperiment::esmCountdownDone()
{
	// countdown is done, so make sure the robot is in run mode!
	mainApp->requestStateChange(SystemState::RUN);

	// countdown done, start trial
	display1->showMessage(false);
	display2->showMessage(false);
	display1->showCountDown(false);
	display2->showCountDown(false);
	display1->drawTask = true;
	display2->drawTask = true;

	requestStartTrialADS();

	setExperimentState(ExperimentState::TRIALRUNNING);
}

//--------------------------------------------------------------
void ofAppExperiment::esmTrialRunning()
{
	// check if trial is running (data from simulink)
	if (_prevTrialRunning && !_nowTrialRunning) {
		setExperimentState(ExperimentState::TRIALDONE);
	}
}

//--------------------------------------------------------------
void ofAppExperiment::esmTrialDone()
{
	// set display to black
	if (!debugMode) {
		display1->drawTask = false;
		display2->drawTask = false;
	}
	display1->showMessage(true, "Trial done");
	display2->showMessage(true, "Trial done");
	_trialDoneTime = ofGetElapsedTimef();

	// call for homing after trial
	mainApp->requestStateChange(static_cast<SystemState>(_currentBlock.homingType));
	setExperimentState(ExperimentState::TRIALFEEDBACK);
}

//--------------------------------------------------------------
void ofAppExperiment::esmTrialFeedback()
{
	// show feedback (if enabled)
	if (_trialFeedbackType > 0) {  // if trialFeedbackType is not NONE
		ofLogVerbose("feedback?");
		// save previous trial performance
		_trialPerformancePrev[0] = _trialPerformance[0];
		_trialPerformancePrev[1] = _trialPerformance[1];

		// request trial performance feedback from the RT model
		_tcClient->read(_lHdlVar_Read_PerformanceFeedback, &_trialPerformance, sizeof(_trialPerformance));

		string msg1 = "Trial done\n\n";
		string msg2 = "Trial done\n\n";

		// depending on feedback type, adjust method
		switch (_trialFeedbackType) {
		case TrialFeedback::RMSE:
			// show RMSE

			msg1 += "Performance: " + ofToString(_trialPerformance[0], 2);
			msg2 += "Performance: " + ofToString(_trialPerformance[1], 2);

			if (_trialPerformance[0] < _trialPerformancePrev[0]) {
				msg1 += "\nYou improved!";
			}
			if (_trialPerformance[1] < _trialPerformancePrev[1]) {
				msg2 += "\nYou improved!";
			}

			display1->showMessage(true, msg1);
			display2->showMessage(true, msg2);
			
			

			// visual reward
			//showVisualReward();

			break;
		case TrialFeedback::MT:
			// show movement time
			// we need the most accurate movement time, so retrieve it from the RT model

			// BROS1
			if (_trialPerformance[0] < _trialMovementTimeRangeSec[0])
				display1->showMessage(true, "Too fast");
			else if (_trialPerformance[1] > _trialMovementTimeRangeSec[1])
				display1->showMessage(true, "Too slow");

			// BROS2
			if (_trialPerformance[1] < _trialMovementTimeRangeSec[0])
				display2->showMessage(true, "Too fast");
			else if (_trialPerformance[1] > _trialMovementTimeRangeSec[1])
				display2->showMessage(true, "Too slow");
			break;
		}
	}

	// go to homing after
	setExperimentState(ExperimentState::HOMINGAFTER);
}

//--------------------------------------------------------------
void ofAppExperiment::esmHomingAfter()
{
	// check whether system is 'at home' (399)
	if (mainApp->systemIsInState(SystemState::ATHOME)) 
		setExperimentState(ExperimentState::HOMINGAFTERDONE);
}

//--------------------------------------------------------------
void ofAppExperiment::esmHomingAfterDone()
{
	// wait at least a couple of seconds after trial is done to show message etc to user before going to new trial
	if (ofGetElapsedTimef() - _trialDoneTime > 4.0f) { 
		display1->showMessage(false, "");
		display2->showMessage(false, "");

		// check next step
		setExperimentState(ExperimentState::CHECKNEXTSTEP);
	}
}

//--------------------------------------------------------------
void ofAppExperiment::esmCheckNextStep()
{
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
}

//--------------------------------------------------------------
void ofAppExperiment::esmTrialBreak()
{
	if (_currentTrial.breakDuration < -1.0) { // no break
		display1->showMessage(false);
		display2->showMessage(false);
		setExperimentState(ExperimentState::TRIALBREAKDONE);
		return; 
	} 

	double time = ofGetElapsedTimef();
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
}

//--------------------------------------------------------------
void ofAppExperiment::esmTrialBreakDone()
{
	// trial break done, on to the next trial!
	_currentTrialNumber++;
	setExperimentState(ExperimentState::NEWTRIAL);
}

//--------------------------------------------------------------
void ofAppExperiment::esmBlockBreak()
{
	double time = ofGetElapsedTimef();
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
}

//--------------------------------------------------------------
void ofAppExperiment::esmBlockBreakDone()
{
	// block break done, on to the next block!
	_currentBlockNumber++;
	setExperimentState(ExperimentState::NEWBLOCK);
}