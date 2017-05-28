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

// structs
struct trialData {
	int trialNumber;
	bool connected = false;				// default: not connected
	double connectionStiffness = 0.0;	// default: 0.0 (no connection stiffness)
	int condition;						// condition type
	double trialDuration = -1.0;		// - 1.0 seconds: define trialDone in Simulink
	double breakDuration;				// pause after each trial
	int trialRandomization;				// select whcih phase set we are selecting
};

struct blockData {
	int blockNumber;
	int numTrials;
	double breakDuration = 5.0*60.0;	// default: 5 minute break
	int homingType = 302;				// homing type. 301: manual homing, 302: auto homing (default)
	vector<trialData> trials;
};

enum ExperimentState {
	IDLE = 0,
	EXPERIMENTSTART,
	EXPERIMENTSTOP,
	EXPERIMENTPAUSE,
	EXPERIMENTCONTINUE,
	EXPERIMENTDONE,
	NEWBLOCK,
	NEWTRIAL,
	HOMINGBEFORE,
	HOMINGBEFOREDONE,
	COUNTDOWN,
	COUNTDOWNDONE,
	TRIALRUNNING,
	TRIALDONE,
	HOMINGAFTER,
	HOMINGAFTERDONE,
	CHECKNEXTSTEP,
	TRIALBREAK,
	TRIALBREAKDONE,
	BLOCKBREAK,
	BLOCKBREAKDONE,
	BLOCKDONE
};

static std::string StringExperimentStateLabel(const ExperimentState value) {
	static std::map<ExperimentState, std::string> strings;
	if (strings.size() == 0) {
#define INSERT_ELEMENT(p) strings[p] = #p
		INSERT_ELEMENT(IDLE);
		INSERT_ELEMENT(EXPERIMENTSTART);
		INSERT_ELEMENT(EXPERIMENTSTOP);
		INSERT_ELEMENT(EXPERIMENTPAUSE);
		INSERT_ELEMENT(EXPERIMENTCONTINUE);
		INSERT_ELEMENT(EXPERIMENTDONE);
		INSERT_ELEMENT(NEWBLOCK);
		INSERT_ELEMENT(NEWTRIAL);
		INSERT_ELEMENT(HOMINGBEFORE);
		INSERT_ELEMENT(HOMINGBEFOREDONE);
		INSERT_ELEMENT(COUNTDOWN);
		INSERT_ELEMENT(COUNTDOWNDONE);
		INSERT_ELEMENT(TRIALRUNNING);
		INSERT_ELEMENT(TRIALDONE);
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

class ofAppExperiment : public ofBaseApp
{
	private:
		
		//
		// variables
		//

		// tcAdsClient
		tcAdsClient *_tcClient;
		unsigned long _lHdlVar_Write_Condition, _lHdlVar_Write_ConnectionStiffness, _lHdlVar_Write_Connected, _lHdlVar_Write_TrialDuration, 
			_lHdlVar_Write_TrialNumber, _lHdlVar_Write_StartTrial, _lHdlVar_Write_TrialRandom=0;

		// experiment state
		ExperimentState _expState = ExperimentState::IDLE;
		
		int _currentTrialNumber = 0, _currentBlockNumber = 0, _numTrials = 0;

		bool _experimentRunning = false, _experimentLoaded = false;
		bool prevTrialRunning = false;
		
		// block and trial data for current trial/block
		blockData _currentBlock;
		trialData _currentTrial;
		vector<blockData> _blocks; // vector of vector<trials> to store all trials per block

		// countdown and break parameters
		double _cdDuration = 3.0; // -1.0 countdown means no countdown
		double _cdStartTime, _breakStartTime;

		//
		// functions
		//
		void setTrialDataADS();
		void requestStartTrialADS();
		void setExperimentState(ExperimentState newState);

	public:

		//
		// variables
		//
		ofXml XML;

		shared_ptr<ofAppMain> mainApp;
		shared_ptr<ofAppDisplay> display1;
		shared_ptr<ofAppDisplay> display2;

		string experimentStateLabel = StringExperimentStateLabel(_expState);

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
};

