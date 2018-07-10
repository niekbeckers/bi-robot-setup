#pragma once

#include <sys/stat.h>
#include <time.h>

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

enum DisplayType {
	PURSUIT = 0,
	PURSUIT_1D,
	COMPENSATORY,
	PURSUIT_ROLL
};

enum ParticleShape {
	PARTICLESHAPE_CIRCLE = 0,
	PARTICLESHAPE_LINE,
	PARTICLESHAPE_CROSS,
	PARTICLESHAPE_CROSSHAIR
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
	bool executeVirtualPartner = false;
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
	bool vpFitOnHeRoC = false;
	vector<int> activeBROSIDs;
	double cdDuration = 3.0;
	double getReadyDuration = 1.0;
	int numTrials = 0;
	string protocolname;
	DisplayType displayType = DisplayType::PURSUIT; // 0 = pursuit, 1 = compensatory
	ParticleShape cursorShape = ParticleShape::PARTICLESHAPE_CIRCLE; //  0 = circle
};


/*
 * TODO: This class is OS-specific; you might want to use Pointer-to-Implementation
 * Idiom to hide the OS dependency from clients
 * Found this code here: https://stackoverflow.com/questions/7337651/file-sort-in-c-by-modification-time#7348347
 */
struct CompareDateModified{
    // Returns true if and only if lhs < rhs
    bool operator() (const std::string& lhs, const std::string& rhs){
        struct stat attribLhs;
        struct stat attribRhs;  //File attribute structs
        stat( lhs.c_str(), &attribLhs);
        stat( rhs.c_str(), &attribRhs); //Get file stats
        return attribLhs.st_mtime < attribRhs.st_mtime; //Compare last modification dates
    }
};

//
// HeRoC PC stuff
//

const string ipAddressHeRoC = "130.89.64.28";
const string userHeRoC = "niek";
const string strSSHKey = "C:\\Users\\TCUser\\keys\\niek.ppk";
const string pwKeyHeRoC = "EthercatisCool";

const string matlabSettingsFilePath_TC = "C:\\Users\\TCUser\\Documents\\repositories\\bros_experiments\\main-projects\\matlab-vp-modelfit\\settings\\";
const string matlabResultsFilePath_TC = "C:\\Users\\TCUser\\Documents\\repositories\\bros_experiments\\main-projects\\matlab-vp-modelfit\\results\\";
const string matlabDataFilePath_TC = "C:\\Users\\TCUser\\Documents\\repositories\\bros_experiments\\experiments\\virtual-agent\\data\\";

const string matlabSettingsFilePath_HeRoC = "/home/niek/repositories/bros_experiments/main-projects/matlab-vp-modelfit/settings/";
const string matlabResultsFilePath_HeRoC = "/home/niek/repositories/bros_experiments/main-projects/matlab-vp-modelfit/results/";
const string matlabDataFilePath_HeRoC = "/home/niek/repositories/bros_experiments/experiments/virtual-agent/data/";

class SystemCmdThreaded : public ofThread {

private:
	string _cmd = "";
	void threadedFunction() { std::system(_cmd.c_str()); };
public:
	bool done = false;
	SystemCmdThreaded(string c) { _cmd = c; };
	~SystemCmdThreaded() { waitForThread(true); }
};