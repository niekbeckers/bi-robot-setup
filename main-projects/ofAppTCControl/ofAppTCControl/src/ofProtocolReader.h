#pragma once

#include "ofMain.h"
#include <functional>

class ofProtocolReader : public ofThread {

private:
	void threadedFunction();

public:
	ofProtocolReader();
	~ofProtocolReader();
};