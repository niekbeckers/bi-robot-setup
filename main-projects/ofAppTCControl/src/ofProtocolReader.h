#pragma once

#include "ofMain.h"
#include <functional>
#include "myCommon.h"



class ofProtocolReader : public ofThread {

private:
	vector<blockData> _blocks; // vector of vector<trials> to store all trials per block
	experimentSettings _settings;

	bool _experimentLoaded;

	// callback function 
	std::function<void(bool, std::string, experimentSettings, vector<blockData>)> _cbFunction;

	void threadedFunction();
	void processOpenFileSelection(ofFileDialogResult openFileResult);

public:
	//ofXml XML;
	ofXml XML;

	ofProtocolReader();
	~ofProtocolReader();
	void registerCBFunction(std::function<void(bool, std::string, experimentSettings, vector<blockData>)> cb);
	inline vector<blockData> blocks() { return _blocks; };
	inline experimentSettings settings() { return _settings; };
};