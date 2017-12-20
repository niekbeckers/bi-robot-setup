#pragma once

#include "ofMain.h"
#include <functional>

#include "ofAppExperiment.h"

struct experimentSettings {
	TrialFeedback trialFeedbackType;
	double trialPerformanceThreshold;
	double trialMovementTimeRangeSec[2];
	bool vpDoVirtualPartner;
	vector<int> activeBROSIDs;
	double cdDuration;
	int numTrials;
};

// structs
struct trialData {
	int trialNumber = -1;
	bool connected = false;					// default: not connected
	vector<ConnectedToTypes> connectedTo;	// specify to which people are connected
	bool fitVirtualPartner = false;			// specify whether model fit is performed on this trial (after trial is done)
	vector<int> fitVPBROSIDs;				// BROS ID 
	double connectionStiffness = 0.0;		// default: 0.0 (no connection stiffness)
	double connectionDamping = 0.0;			// default: 0.0
	int condition = 0;						// condition type
	double trialDuration = -1.0;			// - 1.0 seconds: define trialDone in Simulink
	double breakDuration = -1.0;			// pause after each trial
	double trialRandomization = 0.0;		// random start time
};

struct blockData {
	int blockNumber = -1;
	int numTrials = 0;
	double breakDuration = 4.0*60.0;	// default: 5 minute break
	int homingType = 302;				// homing type. 301: manual homing, 302: auto homing (default)
	vector<trialData> trials;
};

enum TrialFeedback {
	NONE = 0,
	RMSE,
	MT
};

enum ConnectedToTypes {
	NOONE = 0,
	HUMANPARTNER,
	VIRTUALPARTNER
};

class ofProtocolReader : public ofThread {

private:
	vector<blockData> _blocks; // vector of vector<trials> to store all trials per block
	experimentSettings _settings;

	bool _experimentLoaded;

	void threadedFunction();
	void processOpenFileSelection(ofFileDialogResult openFileResult);
public:
	ofXml XML;

	ofProtocolReader();
	~ofProtocolReader();
};