#pragma once

#include <algorithm>    // std::reverse
#include <vector>       // std::vector

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
	double pauseDuration;				// pause after each trial
};

struct blockData {
	int blockNumber;
	int numTrials;
	double breakDuration = 5.0*60.0;	// default: 5 minute break
	int homingType = 302;				// homing type. 301: manual homing, 302: auto homing (default)
	vector<trialData> trials;
};

enum ExperimentState { 
	IDLE=0,
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
			_lHdlVar_Write_TrialNumber, _lHdlVar_Write_StartTrial;

		// experiment state
		int _expState = ExperimentState::IDLE;
		int _currentTrialNumber = 0, _currentBlockNumber = 0, _numBlocks = 0, _numTrials = 0;
		bool _experimentStarted = false;

		bool prevTrialRunning = false;
		
		// block and trial data for current trial/block
		blockData _currentBlock;
		trialData _currentTrial;
		vector<blockData> _blocks; // vector of vector<trials> to store all trials per block

		// countdown and break parameters
		double _cdDuration = 3.0; // -1.0 countdown means no countdown
		double _cdStartTime;

		double _breakStartTime;

		//
		// functions
		//
		void setTrialDataADS();
		void requestStartTrialADS();

	public:

		//
		// variables
		//
		ofXml XML;

		shared_ptr<ofAppMain> mainApp;
		shared_ptr<ofAppDisplay> display1;
		shared_ptr<ofAppDisplay> display2;

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

		void start();
		void stop();
		void pause();
		void resume();
};

