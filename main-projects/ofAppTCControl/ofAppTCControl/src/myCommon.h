#pragma once

const unsigned long adsPort = 350;

struct displayData {
	double posCursorX = 0.0;		// position cursor X [m]
	double posCursorY = 0.0;		// position cursor Y [m]
	double posTargetX = 0.0;		// position target X [m]
	double posTargetY = 0.0;		// position target Y [m]
	double wsSemiMajor = 0.10;		// workspace semimajor axis [m]
	double wsSemiMinor = 0.10;		// workspace semiminor axis [m]
	double posVPX = 0.0;			// position virtual partner X [m]
	double posVPY = 0.0;			// position virtual partner Y [m]
};

//
// Experiment Stuff
//

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

// structs
struct experimentSettings {
	TrialFeedback trialFeedbackType = TrialFeedback::NONE;
	double trialPerformanceThreshold = 0.05;
	double trialMovementTimeRangeSec[2] = { 0.8, 1.2 };
	bool vpDoVirtualPartner = false;
	vector<int> activeBROSIDs;
	double cdDuration = 3.0;
	double getReadyDuration = 1.0;
	int numTrials = 0;
	string protocolname;
};
