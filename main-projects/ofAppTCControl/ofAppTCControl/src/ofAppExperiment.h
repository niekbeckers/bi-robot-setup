#pragma once

#include <algorithm>    // std::reverse
#include <vector>       // std::vector
#include <map>

#include "ofMain.h"
#include "ofAppMain.h"
#include "ofAppDisplay.h"
#include "ofUtils.h"
#include "tcAdsClient.h"
#include "myUtils.h"
//#include "encrypt.h"

class ofAppMain;

// structs
struct trialData {
	int trialNumber = -1;
	bool connected = false;				// default: not connected
	double connectionStiffness = 0.0;	// default: 0.0 (no connection stiffness)
	double connectionDamping = 0.0;		// default: 0.0
	int condition = 0;					// condition type
	double trialDuration = -1.0;		// - 1.0 seconds: define trialDone in Simulink
	double breakDuration = -1.0;		// pause after each trial
	double trialRandomization = 0.0;			// random start time
};

struct blockData {
	int blockNumber = -1;
	int numTrials = 0;
	double breakDuration = 5.0*60.0;	// default: 5 minute break
	int homingType = 302;				// homing type. 301: manual homing, 302: auto homing (default)
	vector<trialData> trials;
};

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

enum TrialFeedback {
	NONE = 0,
	RMSE,
	MT
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



class ofAppExperiment : public ofBaseApp
{
	private:
		
		//
		// variables
		//

		// tcAdsClient
		tcAdsClient *_tcClient;
		unsigned long _lHdlVar_Write_Condition, _lHdlVar_Write_Connected, _lHdlVar_Write_TrialDuration,
			_lHdlVar_Write_TrialNumber, _lHdlVar_Write_StartTrial, _lHdlVar_Write_TrialRandom, _lHdlVar_Read_PerformanceFeedback,
			_lHdlVar_Write_DoVirtualPartner, _lHdlVar_Write_VPModelParams, _lHdlVar_Write_VPModelParamsChanged;

		// experiment state
		ExperimentState _expState = ExperimentState::IDLE;
		
		int _currentTrialNumber = 0, _currentBlockNumber = 0, _numTrials = 0;

		// trial feedback
		int _trialFeedbackType = TrialFeedback::NONE;

		bool _experimentRunning = false, _experimentLoaded = false, _experimentPaused = false;
		bool _prevTrialRunning = false, _nowTrialRunning = false;

		// virtual partner fit bool
		bool _vpDoVirtualPartner = false;
		bool _vpOptimizationDone = false;
		string _vpOptimFunction = "";
		int _vpNumOptimParams = 4;
		
		// block and trial data for current trial/block
		blockData _currentBlock;
		trialData _currentTrial;
		vector<blockData> _blocks; // vector of vector<trials> to store all trials per block

		// countdown and break parameters
		double _cdDuration = 3.0; // -1.0 countdown means no countdown
		double _cdStartTime, _breakStartTime, _getReadyStartTime, _trialDoneTime;
		double _getReadyDuration = 1.0;
		double _trialPerformance[2] = { 0.0, 0.0 }, _trialPerformancePrev[2] = { 0.0, 0.0 }; // approximate mean-squared error
		double _trialPerformanceThreshold = 0.0015; // if the RMSE difference threshold (improvement, worse performance)s
		double _trialMovementTimeSec = 0.0; 
		double _trialMovementTimeRangeSec[2] = { 0.8, 1.2 };

		// log
		string _logFilename;

		// every 4 trials, show instructions during the break
		int _instructionMessageInterval = 6;
		string _instructionMessage = "Great job so far!\nSome reminders:\nTry to track the target as accurately as possible\nRemember to avoid stiffening up your arm!";

		//
		// functions
		//
		void setTrialDataADS();
		void requestStartTrialADS();
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

		void initVPOptimization();
		void runVPOptimization();

	public:

		//
		// variables
		//
		ofXml XML;

		shared_ptr<ofAppMain> mainApp;
		shared_ptr<ofAppDisplay> display1;
		shared_ptr<ofAppDisplay> display2;

		string experimentStateLabel = StringExperimentStateLabel(_expState);
		
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
		void processOpenFileSelection(ofFileDialogResult openFileResult);

		void startExperiment();
		void stopExperiment();
		void pauseExperiment();
		void resumeExperiment();

		ExperimentState experimentState() { return _expState; };
};

