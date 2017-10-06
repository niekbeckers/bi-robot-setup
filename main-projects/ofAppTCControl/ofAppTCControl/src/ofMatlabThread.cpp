/*
 * ImgAnalysisThread.cpp
 *
 *  Created on: Oct 8, 2014
 *      Author: arturo
 */

#include "ofMatlabThread.h"

 //--------------------------------------------------------------
MatlabThread::MatlabThread():
	_newOutput(true),
	matlabThreadInitialized(false)
{
	// start the thread as soon as the
	// class is created, it won't use any CPU
	// until we send a new frame to be analyzed
	startThread();
}

//--
void MatlabThread::initialize() {
#if INCLUDEMATLABFUNCTIONS
	// Initialize the MATLAB Compiler Runtime global state
	if (!mclInitializeApplication(NULL, 0)) {
		ofLogError("Could not initialize the application properly.");
	}

	// Initialize the Vigenere library
	if (!libtestInitialize()) {
		ofLogError("Could not initialize the library properly.");
	}
	else {
		// everything is initialized
		matlabThreadInitialized = true;
	}
#endif
}

//--------------------------------------------------------------
MatlabThread::~MatlabThread() {
	// when the class is destroyed
	// close both channels and wait for
	// the thread to finish
	_toAnalyze.close();
	_analyzed.close();
	waitForThread(true);

#if INCLUDEMATLABFUNCTIONS
	// Shut down the library and the application global state.
	libtestTerminate();
	mclTerminateApplication();
#endif
}

//--------------------------------------------------------------
void MatlabThread::analyze(matlabInput input) {
	// send the frame to the thread for analyzing this makes a copy but we can't avoid it anyway if
	// we want to update the grabber while analyzing previous frames
	_toAnalyze.send(input);
}

//--------------------------------------------------------------
void MatlabThread::update(){
	// check if there's a new analyzed frame and upload it to the texture. we use a while loop to drop any
	// extra frame in case the main thread is slower than the analysis
	// tryReceive doesn't reallocate or make any copies
	_newOutput = false;
	while(_analyzed.tryReceive(_output)){
		_newOutput = true;
	}

	if(_newOutput){
        // do stuff with the output
		ofLogVerbose("Message from MATLAB thread: trialID = " + ofToString(_output.trialID) + " x=", ofToString(_output.x));
		
		// do callback function (check if it is assigned)
		if (_cbFunction) {
			_cbFunction(_output);
		}
	}
}

//--------------------------------------------------------------
bool MatlabThread::newOutputData() {
	return _newOutput;
}

//--------------------------------------------------------------
void MatlabThread::threadedFunction(){
    // wait until there's a new frame this blocks the thread, so it doesn't use
    // the CPU at all, until a frame arrives. also receive doesn't allocate or make any copies

	matlabInput input;
    while(_toAnalyze.receive(input)){

		if (!matlabThreadInitialized) return; // matlab not initialized

		matlabOutput output;

		// perform optimization per BROS
		callMatlabOptimization(input, output);

#if __cplusplus>=201103
        _analyzed.send(std::move(data));
#else
        _analyzed.send(output);
#endif
	}
}

void MatlabThread::callMatlabOptimization(matlabInput input, matlabOutput &output)
{

	// Must declare all MATLAB data types after initializing the
	// application and the library, or their constructors will fail.

	//mwArray key(i);
	//mwArray result(2,1, mxDOUBLE_CLASS);

	//myrandom(1,result,key);

	//result.GetData(data.d, 2);

	//data.trialID = input.trialID;
}

