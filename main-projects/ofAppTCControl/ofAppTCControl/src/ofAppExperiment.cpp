#include "ofAppExperiment.h"

using namespace std;

//--------------------------------------------------------------
void ofAppExperiment::setup()
{
	setupTCADS();	// setup TwinCAT ADS
	setExperimentState(ExperimentState::IDLE);

	// register protocol reader callback function
	using namespace std::placeholders;
	_protocol.registerCBFunction(std::bind(&ofAppExperiment::onProtocolLoaded, this, _1, _2));
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

	// update virtual partner and check if partner is still fitting
	partner.update();
	_runningModelFit = partner.modelFitIsRunning();

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

	char szVar3[] = { "Object1 (ModelBROS).ModelParameters.Connected_Value" };
	_lHdlVar_Write_Connected = _tcClient->getVariableHandle(szVar3, sizeof(szVar3));

	char szVar4[] = { "Object1 (ModelBROS).ModelParameters.ExpTrialDuration_Value" };
	_lHdlVar_Write_TrialDuration = _tcClient->getVariableHandle(szVar4, sizeof(szVar4));

	char szVar5[] = { "Object1 (ModelBROS).ModelParameters.ExpTrialNumber_Value" };
	_lHdlVar_Write_TrialNumber = _tcClient->getVariableHandle(szVar5, sizeof(szVar5));

	char szVar6[] = { "Object1 (ModelBROS).ModelParameters.ExpTrialRandom_Value" };
	_lHdlVar_Write_TrialRandom = _tcClient->getVariableHandle(szVar6, sizeof(szVar6));

	char szVar8[] = { "Object1 (ModelBROS).BlockIO.PerformanceFeedback" };
	_lHdlVar_Read_PerformanceFeedback = _tcClient->getVariableHandle(szVar8, sizeof(szVar8));

	//char szVar2[] = { "Object1 (ModelBROS).ModelParameters.KpConnection_Value" };
	//_lHdlVar_Write_ConnectionStiffness = _tcClient->getVariableHandle(szVar2, sizeof(szVar2));

	//char szVar7[] = { "Object1 (ModelBROS).ModelParameters.KdConnection_Value" };
	//_lHdlVar_Write_ConnectionDamping = _tcClient->getVariableHandle(szVar7, sizeof(szVar7));
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
void ofAppExperiment::setTrialDataADS()
{
	// write trial data to ADS/Simulink model

	// trial number
	_tcClient->write(_lHdlVar_Write_TrialNumber, &_currentTrial.trialNumber, sizeof(_currentTrial.trialNumber));

	// connected
	mainApp->setConnectionEnabled(_currentTrial.connected);

	// connectionStiffness
	mainApp->setConnectionStiffness(_currentTrial.connectionStiffness);
	// connection damping
	mainApp->setConnectionDamping(_currentTrial.connectionDamping);

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
	
	ofLogVerbose() << "(" << typeid(this).name() << ") " << "loadExperimentXML";

	_protocol.startThread();
}

//--------------------------------------------------------------
void ofAppExperiment::onProtocolLoaded(experimentSettings settings, vector<blockData> blocks) {
	
	// copy settings and blocks
	_settings = settings;
	_blocks = blocks;

	// reset current block number
	_currentBlockNumber = 0;
	_currentTrialNumber = 0;
	_numTrials = 0;

	// set labels in the GUI
	mainApp->lblTrialNumber.setMin(1);
	mainApp->lblTrialNumber.setMax(_blocks[0].trials.size());
	mainApp->lblBlockNumber.setMin(1);
	mainApp->lblBlockNumber.setMax(_blocks.size());
	mainApp->lblTrialNumber = _currentTrialNumber + 1;
	mainApp->lblBlockNumber = _currentBlockNumber + 1;
	mainApp->lblExpLoaded = _settings.protocolname;

	// virtual partner
	if (_settings.vpDoVirtualPartner) {
		partner.initialize(_settings.activeBROSIDs);
	}

	/*
	// debug
	for (int i = 0; i < _blocks[1].trials.size(); i++) {
		ofLogVerbose() << "trial " << i << " c " <<_blocks[1].trials[i].connected;
	}
	*/

	_experimentLoaded = true;
}

//--------------------------------------------------------------
void ofAppExperiment::showVisualReward()
{
	//
	// BROS 1
	//
	double performanceDiff = 0.0;
	if (fabs(_trialPerformancePrev[0]) > 0) {
		// relative improvement
		performanceDiff = (_trialPerformance[0] - _trialPerformancePrev[0]) / _trialPerformancePrev[0];
	}
	else {
		performanceDiff = (_trialPerformance[0] - _trialPerformancePrev[0]);
	}

	if (performanceDiff < -_settings.trialPerformanceThreshold) { // better performance compared to last trial
		display1->cursor.setMode(PARENTPARTICLE_MODE_EXPLODE);
	}
	else if (performanceDiff > _settings.trialPerformanceThreshold) { // worse performance compared to last trial
		// do nothing
	}
	else {  // similar performance compared to last trial
		// do nothing
	}

	//
	// BROS 2
	//
	if (fabs(_trialPerformancePrev[1]) > 0) {
		// relative improvement
		performanceDiff = (_trialPerformance[1] - _trialPerformancePrev[1]) / _trialPerformancePrev[1];
	}
	else {
		performanceDiff = (_trialPerformance[1] - _trialPerformancePrev[1]);
	}

	if (performanceDiff < -_settings.trialPerformanceThreshold) { // better performance compared to last trial
		display2->cursor.setMode(PARENTPARTICLE_MODE_EXPLODE);
	}
	else if (performanceDiff > _settings.trialPerformanceThreshold) { // worse performance compared to last trial
		// do nothing
	}
	else {  // similar performance compared to last trial
		// do nothing
	}
}

//
// Experiment State Machine functions
//

//--------------------------------------------------------------
void ofAppExperiment::esmExperimentStart()
{
	//_currentBlockNumber = 0;
	//_currentTrialNumber = 0;
	_experimentRunning = true;
	display1->showMessageNorth(false);
	display2->showMessageNorth(false);

	setExperimentState(ExperimentState::NEWBLOCK);
}

//--------------------------------------------------------------
void ofAppExperiment::esmExperimentStop()
{

	_experimentRunning = false;

	display1->drawTask = true;
	display2->drawTask = true;
	display1->cursor.setMode(PARENTPARTICLE_MODE_EXPLODE);
	display2->cursor.setMode(PARENTPARTICLE_MODE_EXPLODE);

	// stop the data recorder
	mainApp->stopDataLogger();

	display1->showMessageNorth(true, "EXPERIMENT FINISHED");
	display2->showMessageNorth(true, "EXPERIMENT FINISHED");

	setExperimentState(ExperimentState::EXPERIMENTDONE);
}

//--------------------------------------------------------------
void ofAppExperiment::esmNewBlock()
{
	if (!_experimentRunning) { return; }

	// request data recorder start
	mainApp->startDataLogger();

	_currentBlock = _blocks[_currentBlockNumber];
	_currentTrialNumber = 0;
	mainApp->lblBlockNumber = _currentBlockNumber + 1;
	mainApp->lblTrialNumber.setMax(_currentBlock.trials.size());

	// reset trial performance
	_trialPerformancePrev[0] = 0.0;
	_trialPerformancePrev[1] = 0.0;

	// start first trial
	setExperimentState(ExperimentState::NEWTRIAL);
}

//--------------------------------------------------------------
void ofAppExperiment::esmNewTrial()
{
	if (!_experimentRunning) { return; }
	if (_experimentPaused) {
		display1->showMessageNorth(true, "EXPERIMENT PAUSED");
		display2->showMessageNorth(true, "EXPERIMENT PAUSED"); 
		return; 
	}

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
	display1->showMessageNorth(true, "GET READY");
	display2->showMessageNorth(true, "GET READY");
	setExperimentState(ExperimentState::GETREADY);
}

//--------------------------------------------------------------
void ofAppExperiment::esmGetReady()
{
	if ((ofGetElapsedTimef() - _getReadyStartTime) <= _settings.getReadyDuration) {
		//
	}
	else {
		display1->showMessageNorth(false);
		display2->showMessageNorth(false);
		setExperimentState(ExperimentState::GETREADYDONE);
	}
}

//--------------------------------------------------------------
void ofAppExperiment::esmGetReadyDone()
{
	if (_settings.cdDuration < 0.0) {
		// if countdown is negative (i.e. no countdown needed), return
		setExperimentState(ExperimentState::COUNTDOWNDONE);
	}
	else {
		// start countdown
		_cdStartTime = ofGetElapsedTimef();
		display1->cursor.setMode(PARENTPARTICLE_MODE_NORMAL);
		//display1->target.setMode(PARENTPARTICLE_MODE_NORMAL);
		display1->target.reset();
		display2->cursor.setMode(PARENTPARTICLE_MODE_NORMAL);
		//display2->target.setMode(PARENTPARTICLE_MODE_NORMAL);
		display1->target.reset();
		display1->drawTask = true;
		display2->drawTask = true;
		setExperimentState(ExperimentState::COUNTDOWN);
	}
}

//--------------------------------------------------------------
void ofAppExperiment::esmCountdown()
{
	double time = ofGetElapsedTimef();
	if ((time - _cdStartTime) <= _settings.cdDuration) {
		double cdTimeRemaining = _settings.cdDuration - (time - _cdStartTime);
		display1->showMessageNorth(true, "COUNTDOWN");
		display2->showMessageNorth(true, "COUNTDOWN");
		display1->showCountDown(true, cdTimeRemaining, _settings.cdDuration);
		display2->showCountDown(true, cdTimeRemaining, _settings.cdDuration);
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
	display1->showMessageNorth(false);
	display2->showMessageNorth(false);
	display1->showCountDown(false);
	display2->showCountDown(false);
	display1->target.doDraw = true;
	display2->target.doDraw = true;
	display1->drawTask = true;
	display2->drawTask = true;
	display1->cursor.setMode(PARENTPARTICLE_MODE_NORMAL);
	//display1->target.setMode(PARENTPARTICLE_MODE_NORMAL);
	display1->target.reset();
	display2->cursor.setMode(PARENTPARTICLE_MODE_NORMAL);
	//display2->target.setMode(PARENTPARTICLE_MODE_NORMAL);
	display2->target.reset();

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

	display1->target.doDraw = false;
	display2->target.doDraw = false;

	display1->showMessageNorth(true, "TRIAL DONE");
	display2->showMessageNorth(true, "TRIAL DONE");
	_trialDoneTime = ofGetElapsedTimef();

	// write trial done to log file 
	// In case the software/pc crashes, we can backtrace where we were. 
	// Edit the experiment XML (remove the trials that already have been done) and start experiment again.
	ofLogToFile(_logFilename, true);
	ofLogVerbose() << "(" << typeid(this).name() << ") " << "Trial done. Trial " << _currentTrialNumber << " Block " << _currentBlockNumber;
	ofLogToConsole(); // log back to console

	// call for homing after trial
	mainApp->requestStateChange(static_cast<SystemState>(_currentBlock.homingType));
	setExperimentState(ExperimentState::TRIALFEEDBACK);

	// check if we need to fit the virtual partner
	if (_currentTrial.fitVirtualPartner) {
		// pause data logger before doing optimization
		mainApp->stopDataLogger();

		// setup optimization (settings)
		matlabInput settings;
		settings.doFitForBROSIDs = _currentTrial.fitVPBROSIDs;
		settings.trialID = _currentTrialNumber;
		settings.condition = _currentTrial.condition;
		partner.runVPOptimization(settings);
		_runningModelFit = true;
	}
}

//--------------------------------------------------------------
void ofAppExperiment::esmTrialFeedback()
{
	// show feedback (if enabled)
	if (_settings.trialFeedbackType > 0) {  // if trialFeedbackType is not NONE

		// request trial performance feedback from the RT model
		_tcClient->read(_lHdlVar_Read_PerformanceFeedback, &_trialPerformance, sizeof(_trialPerformance));

		string msg1 = "TRIAL DONE\n\n";
		string msg2 = "TRIAL DONE\n\n";


		// depending on feedback type, adjust method
		switch (_settings.trialFeedbackType) {
		case TrialFeedback::RMSE:
			// show RMSE

			msg1 += "Performance: " + ofToString(_trialPerformance[0], 2);
			msg2 += "Performance: " + ofToString(_trialPerformance[1], 2);

			if (_trialPerformance[0] < _trialPerformancePrev[0]) {
				msg1 += "\nYou improved! Keep up the good work!";
			}
			if (_trialPerformance[1] < _trialPerformancePrev[1]) {
				msg2 += "\nYou improved! Keep up the good work!";
			}

			display1->showMessageNorth(true, msg1);
			display2->showMessageNorth(true, msg2);
			display1->drawTask = true;
			display2->drawTask = true;

			// visual reward
			showVisualReward();

			break;
		case TrialFeedback::MT:
			// show movement time
			// we need the most accurate movement time, so retrieve it from the RT model

			// BROS1
			if (_trialPerformance[0] < _settings.trialMovementTimeRangeSec[0])
				display1->showMessageNorth(true, "Too fast");
			else if (_trialPerformance[1] > _settings.trialMovementTimeRangeSec[1])
				display1->showMessageNorth(true, "Too slow");

			// BROS2
			if (_trialPerformance[1] < _settings.trialMovementTimeRangeSec[0])
				display2->showMessageNorth(true, "Too fast");
			else if (_trialPerformance[1] > _settings.trialMovementTimeRangeSec[1])
				display2->showMessageNorth(true, "Too slow");
			break;
		}

		// set trial performance in experiment leader GUI
		mainApp->lblTrialPerformance = "[" + ofToString(_trialPerformance[0],4) +", " + ofToString(_trialPerformance[1],4) + "]";

		// save previous trial performance
		_trialPerformancePrev[0] = _trialPerformance[0];
		_trialPerformancePrev[1] = _trialPerformance[1];
	}

	// occasionaly show instructions
	if (((_currentTrialNumber + 1) % _instructionMessageInterval == 0) && (_instructionMessage != "")) {
		display1->showMessageCenter(true, _instructionMessage);
		display2->showMessageCenter(true, _instructionMessage);
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
		display1->showMessageNorth(false);
		display2->showMessageNorth(false);

		display1->drawTask = false;
		display2->drawTask = false;

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

			// switch off the data recorder
			mainApp->stopDataLogger();

			setExperimentState(ExperimentState::BLOCKBREAK);
		}
		else {
			// all blocks are done, experiment is done
			setExperimentState(ExperimentState::EXPERIMENTSTOP);
			return;
		}
	}

	
}

//--------------------------------------------------------------
void ofAppExperiment::esmTrialBreak()
{
	bool breakDone = false;
	if (_currentTrial.breakDuration < -1.0) { // no break
		breakDone = true;
	} 

	double time = ofGetElapsedTimef();
	if ((time - _breakStartTime) <= _currentTrial.breakDuration) {
		// trial break is running, show feedback on display
		double timeRemaining = _currentTrial.breakDuration - (time - _breakStartTime);
		if (timeRemaining < 0.0) { timeRemaining = 0.0; }
		
		string msg = "BREAK\n" + secToMin(timeRemaining) + " remaining";

		display1->showMessageNorth(true, msg);
		display2->showMessageNorth(true, msg);
	}
	else {
		breakDone = true;
	}

	if (breakDone) {
		if (!_runningModelFit) {
			// make sure to upause logger (if it was not running anymore)
			mainApp->startDataLogger();

			// clear screen messages
			display1->showMessageNorth(false);
			display2->showMessageNorth(false);
			display1->showMessageCenter(false);
			display2->showMessageCenter(false);

			setExperimentState(ExperimentState::TRIALBREAKDONE);
		}
		else {
			// wait a bit longer, show message
			string msg = "Wait just a little bit longer...";
			display1->showMessageNorth(true, msg);
			display2->showMessageNorth(true, msg);
		}
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
		display1->showMessageNorth(true, msg);
		display2->showMessageNorth(true, msg);
	}
	else {
		display1->showMessageNorth(false);
		display2->showMessageNorth(false);
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

//--------------------------------------------------------------
void ofAppExperiment::setCurrentTrialNumber(int nr) {
	// set current trial number (externally called by user, for instance after restart of experiment).
	_currentTrialNumber = nr - 1;
	ofLogVerbose() << "(" << typeid(this).name() << ") " << "_currentTrialNumber set to " << _currentTrialNumber << " by user.";
}

//--------------------------------------------------------------
void ofAppExperiment::setCurrentBlockNumber(int nr) {
	// set current block number (externally called by user, for instance after restart of experiment).
	_currentBlockNumber = nr - 1;
	ofLogVerbose() << "(" << typeid(this).name() << ") " << "_currentBlockNumber set to " << _currentBlockNumber << " by user.";
}