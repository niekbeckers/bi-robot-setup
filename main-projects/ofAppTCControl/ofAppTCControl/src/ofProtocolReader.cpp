#include "ofProtocolReader.h"


//--------------------------------------------------------------
ofProtocolReader::ofProtocolReader() {

}

//--------------------------------------------------------------
ofProtocolReader::~ofProtocolReader() {
	// when the class is destroyed
	// close both channels and wait for
	// the thread to finish
	waitForThread(true);
}