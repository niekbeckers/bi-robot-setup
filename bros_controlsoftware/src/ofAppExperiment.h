#pragma once

#include <algorithm>    // std::reverse
#include <vector>       // std::vector
#include <map>

#include "ofMain.h"
#include "ofAppMain.h"
#include "ofAppDisplay.h"
#include "ofProtocolReader.h"
//#include "virtualPartner.h"
#include "ofUtils.h"
#include "tcAdsClient.h"
#include "myCommon.h"


#define initializeMATLABRuntime 0

enum ExperimentState {
	IDLE = 0,
	SYSTEMFAULT,
	EXPERIMENTSTART,
	EXPERIMENTSTOP,
	EXPERIMENTPAUSE,
	EXPERIMENTCONTINUE,
	EXPERIMENTDONE,
	NEWBLOCK,
	NEWTRIAL,
	HOMINGBEFORE,
	HOMINGBEFOREDONE,
	GETREADY,
	GETREADYDONE,
	COUNTDOWN,
	COUNTDOWNDONE,
	TRIALRUNNING,
	TRIALDONE,
	TRIALFEEDBACK,
	HOMINGAFTER,
	HOMINGAFTERDONE,
	CHECKNEXTSTEP,
	TRIALBREAK,
	TRIALBREAKDONE,
	BLOCKBREAK,
	BLOCKBREAKDONE,
	BLOCKDONE
};



// ExperimentStateLabel: in order to print a string of the currect experimentstate
static std::string StringExperimentStateLabel(const ExperimentState value) {
	static std::map<ExperimentState, std::string> strings;
	if (strings.size() == 0) {
#define INSERT_ELEMENT(p) strings[p] = #p
		INSERT_ELEMENT(IDLE);
		INSERT_ELEMENT(SYSTEMFAULT);
		INSERT_ELEMENT(EXPERIMENTSTART);
		INSERT_ELEMENT(EXPERIMENTSTOP);
		INSERT_ELEMENT(EXPERIMENTPAUSE);
		INSERT_ELEMENT(EXPERIMENTCONTINUE);
		INSERT_ELEMENT(EXPERIMENTDONE);
		INSERT_ELEMENT(NEWBLOCK);
		INSERT_ELEMENT(NEWTRIAL);
		INSERT_ELEMENT(HOMINGBEFORE);
		INSERT_ELEMENT(HOMINGBEFOREDONE);
		INSERT_ELEMENT(GETREADY);
		INSERT_ELEMENT(GETREADYDONE);
		INSERT_ELEMENT(COUNTDOWN);
		INSERT_ELEMENT(COUNTDOWNDONE);
		INSERT_ELEMENT(TRIALRUNNING);
		INSERT_ELEMENT(TRIALDONE);
		INSERT_ELEMENT(TRIALFEEDBACK);
		INSERT_ELEMENT(HOMINGAFTER);
		INSERT_ELEMENT(HOMINGAFTERDONE);
		INSERT_ELEMENT(CHECKNEXTSTEP);
		INSERT_ELEMENT(TRIALBREAK);
		INSERT_ELEMENT(TRIALBREAKDONE);
		INSERT_ELEMENT(BLOCKBREAK);
		INSERT_ELEMENT(BLOCKBREAKDONE);
		INSERT_ELEMENT(BLOCKDONE);
#undef INSERT_ELEMENT
	}

	return strings[value];
};

class ofAppMain;
class ofAppDisplay;
//class VirtualPartner;

class ofAppExperiment : public ofBaseApp
{
private:

	//
	// variables
	//

	// tcAdsClient
	tcAdsClient *_tcClient;
	unsigned long _lHdlVar_Write_Condition, _lHdlVar_Write_Connected, _lHdlVar_Write_TrialDuration,
		_lHdlVar_Write_TrialNumber, _lHdlVar_Write_StartTrial, _lHdlVar_Write_StopTrial, _lHdlVar_Write_TrialRandom, _lHdlVar_Read_PerformanceFeedback;

	// experiment state
	ExperimentState _expState = ExperimentState::IDLE;

	int _currentTrialNumber = 0, _currentBlockNumber = 0, _numTrials = 0;

	bool _experimentRunning = false, _experimentLoaded = false, _experimentPaused = false;
	bool _prevTrialRunning = false, _nowTrialRunning = false;

	// virtual partner fit bool
	bool _runningModelFit = false;

	// block and trial data for current trial/block
	blockData _currentBlock;
	trialData _currentTrial;
	vector<blockData> _blocks; // vector of vector<trials> to store all trials per block

	// countdown and break parameters
	double _cdStartTime, _breakStartTime, _getReadyStartTime, _trialDoneTime;
	double _trialPerformance[2] = { 0.0, 0.0 }, _trialPerformancePrev[2] = { 0.0, 0.0 }, _trialScore[2] = { 0.0 , 0.0 }, _trialMaxScore[2] = { 100.0, 100.0 };// approximate mean-squared error
	double _trialMovementTimeSec = 0.0;
	

	ofProtocolReader _protocol;
	experimentSettings _settings;

	// log
	string _logFilename;

	// every 4 trials, show instructions during the break
	int _instructionMessageInterval = 6;
	string _instructionMessage = ""; // "Great job so far!\n\nSome reminders:\nTry to track the target as accurately as possible\nRemember to avoid stiffening up your arm!";

	// performance log vectors 
	vector<float> _trackingPerformanceLog_BROS1, _trackingPerformanceLog_BROS2;


	//
	// functions
	//
	void setTrialDataADS();
	void requestStartTrialADS();
	void requestStopTrialADS();
	void setExperimentState(ExperimentState newState);
	string secToMin(double seconds);
	void showVisualReward();

	void esmExperimentStart();
	void esmExperimentStop();
	void esmNewBlock();
	void esmNewTrial();
	void esmHomingBefore();
	void esmHomingBeforeDone();
	void esmGetReady();
	void esmGetReadyDone();
	void esmCountdown();
	void esmCountdownDone();
	void esmTrialRunning();
	void esmTrialDone();
	void esmTrialFeedback();
	void esmHomingAfter();
	void esmHomingAfterDone();
	void esmCheckNextStep();
	void esmTrialBreak();
	void esmTrialBreakDone();
	void esmBlockBreak();
	void esmBlockBreakDone();

public:

	//
	// variables
	//
	ofXml XML, _XMLWrite;

	shared_ptr<ofAppMain> mainApp;
	//shared_ptr<VirtualPartner> vpApp;
	shared_ptr<ofAppDisplay> display1;
	shared_ptr<ofAppDisplay> display2;

	string experimentStateLabel = StringExperimentStateLabel(_expState);

	//VirtualPartner partner;

	bool debugMode = false;
	// 
	// functions
	//

	// openframeworks
	void setup();
	void update();
	void exit();

	// custom
	void setupTCADS();
	void loadExperimentXML();
	void onProtocolLoaded(bool success, std::string filename, experimentSettings settings, vector<blockData> blocks);

	void startExperiment();
	void stopExperiment();
	void pauseExperiment();
	void resumeExperiment();

	ExperimentState experimentState() { return _expState; };
	inline int getCurrentTrialNumber() { return _currentTrialNumber; };
	inline trialData getCurrentTrial() { return _currentTrial; };
	void setCurrentTrialNumber(int nr);
	void setCurrentBlockNumber(int nr);
	inline experimentSettings getExperimentSettings() { return _settings; };
	inline bool experimentIsLoaded() { return _experimentLoaded; };
};

