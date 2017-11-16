#pragma once

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

const unsigned long adsPort = 350;