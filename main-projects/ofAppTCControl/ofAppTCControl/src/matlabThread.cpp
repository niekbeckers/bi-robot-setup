/*
 * ImgAnalysisThread.cpp
 *
 *  Created on: Oct 8, 2014
 *      Author: arturo
 */

#include "matlabThread.h"
#include "ofConstants.h"

 //--------------------------------------------------------------
MatlabThread::MatlabThread():
	_newData(true)
{

#if INCLUDEMATLABFUNCTION
	// Initialize the MATLAB Compiler Runtime global state
	if (!mclInitializeApplication(NULL, 0))
	{
		ofLogError("Could not initialize the application properly.");
	}

	// Initialize the Vigenere library
	if (!libtestInitialize())
	{
		ofLogError("Could not initialize the library properly.");
	}
#endif

	// start the thread as soon as the
	// class is created, it won't use any CPU
	// until we send a new frame to be analyzed
	startThread();

}

//--------------------------------------------------------------
MatlabThread::~MatlabThread(){
	// when the class is destroyed
	// close both channels and wait for
	// the thread to finish
	_toAnalyze.close();
	_analyzed.close();
	waitForThread(true);

#if INCLUDEMATLABFUNCTION
	// Shut down the library and the application global state.
	libtestTerminate();
	mclTerminateApplication();
#endif
}

//--------------------------------------------------------------
void MatlabThread::analyze(matlabInput input) {
	// send the frame to the thread for analyzing
	// this makes a copy but we can't avoid it anyway if
	// we want to update the grabber while analyzing
    // previous frames
	_toAnalyze.send(input);
}

//--------------------------------------------------------------
void MatlabThread::update(){
	// check if there's a new analyzed frame and upload it to the texture. we use a while loop to drop any
	// extra frame in case the main thread is slower than the analysis
	// tryReceive doesn't reallocate or make any copies
	_newData = false;
	while(_analyzed.tryReceive(_output)){
		_newData = true;
	}

	if(_newData){
        // do stuff
		ofLogError("Message from MATLAB thread: trialID = " + ofToString(_output.trialID) + " x(1)=", ofToString(_output.d[0]) + " x(2) = " + ofToString(_output.d[1]));
	}
}

//--------------------------------------------------------------
void MatlabThread::threadedFunction(){
    // wait until there's a new frame
    // this blocks the thread, so it doesn't use
    // the CPU at all, until a frame arrives.
    // also receive doesn't allocate or make any copies
	matlabInput input;
    while(_toAnalyze.receive(input)){
		matlabOutput data;

		//
		// call MATLAB functions here
		//

		// Must declare all MATLAB data types after initializing the
		// application and the library, or their constructors will fail.
		//mwArray key(i);
		//mwArray result(2,1, mxDOUBLE_CLASS);

		//myrandom(1,result,key);

		//result.GetData(data.d, 2);

		//data.trialID = input.trialID;

#if __cplusplus>=201103
        _analyzed.send(std::move(data));
#else
        _analyzed.send(data);
#endif
	}
}

