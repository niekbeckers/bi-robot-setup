#pragma once
#include "ofMain.h"
#include "ofAppMain.h"
#include <algorithm>    // std::reverse
#include <vector>       // std::vector

using namespace std;

class ofAppMain;

class ofAppExperiment : public ofBaseApp
{
	struct trialData {
		int trialNumber;
		bool connected = false;				// default: not connected
		double connectionStiffness = 0.0;	// default: 0.0 (no connection stiffness)
		int condition;						// condition type
		double trialDuration = -1.0;		// - 1.0 seconds: define trialDone in Simulink
		double pauseDuration;				// pause after each trial
		double countDownDuration = -1.0;	// if countDownDuration = -1.0, no countdown
	};

	struct blockData {
		int blockNumber;
		int numTrials;
		double breakDuration;
		int homingType;
		vector<trialData> trials;
	};

	private:

		//
		// variables
		//
		int _currentTrialNumber = 0, _currentBlockNumber = 0, _numBlocks = 0, _numTrials = 0;
		bool _correctExpLoaded = false, _experimentStarted = false;
		blockData _currentBlock;
		trialData _currentTrial;

		vector<blockData> _blocks; // vector of vector<trials> to store all trials per block

		//
		// functions
		//
		void startTrail();

	public:
		ofAppExperiment();

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
		void loadExperimentXML();
		void start();
		void stop();
		void pause();
		void resume();

		void eventTrialDone();
		void eventCountDownDone();
		void eventPauseDone();
		void eventAtHome();

		void processOpenFileSelection(ofFileDialogResult openFileResult);
};

