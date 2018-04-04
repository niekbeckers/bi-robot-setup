#include "ofAppMain.h"

using namespace std;

//--------------------------------------------------------------
void ofAppMain::setup(){

	ofSetFrameRate(120);

	ofSetLogLevel(OF_LOG_NOTICE);

	// matlab on heroc thread
	string cmd = "putty -ssh -i " + strSSHKey + " -pw " + pwKeyHeRoC + " " + userHeRoC + "@" + ipAddressHeRoC + " -m C:\\Users\\Labuser\\Documents\\repositories\\bros_experiments\\main-projects\\matlab-vp-modelfit\\runVirtualPartnerMATLAB.sh -t";
	_herocMATLABThread = new SystemCmdThreaded(cmd);

	// set up window
	ofBackground(ofColor::blueSteel);
	ofSetWindowTitle("Control");

	// read error description file (if present)
	try {
		ofBuffer buffer = ofBufferFromFile("C:\\Users\\Labuser\\Documents\\repositories\\bros_experiments\\libraries\\BROSErrorDescriptions.txt");
		for (auto line : buffer.getLines()) {
			_errorDescriptions.push_back(line);
		}
		ofLogVerbose() << "(" << typeid(this).name() << ") " << "Loaded error descriptions:\n" << ofToString(_errorDescriptions);
	}
	catch (...) {
		ofLogError() << "(" << typeid(this).name() << ") " << "Cannot find BROSErrorDescriptions.txt";
	}

	_fontErrorMsg.loadFont("verdana.ttf", 10);

	// setup tcAdsClient
	setupTCADS();

	// set up GUI
	setupGUI();

	_timeCheck = ofGetElapsedTimef();

	// point struct pointers of displays to data structs
	display1->pData = &_display1Data;
	display2->pData = &_display2Data;

	_lblSysState[0] = StringSystemStateLabel(_systemState[0]);
	_lblSysState[1] = StringSystemStateLabel(_systemState[1]);
}

//--------------------------------------------------------------
void ofAppMain::update(){

	// read continuous ADS data
	_tcClientCont->read(_lHdlVar_Read_Data, &AdsData, sizeof(AdsData));
	 
	// set data in displayData structs
	// display 1
	_display1Data.posCursorX = AdsData[0];
	_display1Data.posCursorY = AdsData[1];
	_display1Data.posTargetX = AdsData[4];
	_display1Data.posTargetY = AdsData[5];

	// display 2
	_display2Data.posCursorX = AdsData[2];
	_display2Data.posCursorY = AdsData[3];
	_display2Data.posTargetX = AdsData[6];
	_display2Data.posTargetY = AdsData[7];

	// virtual partner data
	if (_lHdlVar_DataVP1 != -1) {
		_tcClientCont->read(_lHdlVar_DataVP1, &_VP1Data, sizeof(_VP1Data));
		_display1Data.posVPX = _VP1Data[0];
		_display1Data.posVPY = _VP1Data[1];
	}
	if (_lHdlVar_DataVP2 != -1) {
		_tcClientCont->read(_lHdlVar_DataVP2, &_VP2Data, sizeof(_VP2Data));
		_display2Data.posVPX = _VP2Data[0];
		_display2Data.posVPY = _VP2Data[1];
	}

	// periodic check
	if (ofGetElapsedTimef() - _timeCheck > _timeRefreshCheck) {
		_timeCheck = ofGetElapsedTimef();

		// Check TwinCAT/ADS
		if (_tcClientCont->nErr == 0) {
			twinCatRunning = true;
			_lblEtherCAT = "ON";
			_lblEtherCAT.setBackgroundColor(ofColor::darkGreen);
		}
		else {
			twinCatRunning = false;
			// error, probably no ADS running
			_lblEtherCAT = "OFF";
			_lblEtherCAT.setBackgroundColor(ofColor::red);
		}

		// frame rate in GUI
		_lblFRM = ofToString((int)ofGetFrameRate()) + " fps";

		// update GUI ADS data (in case something changed)
		updateADSDataGUI();

		// check if Heroc thread is running
		if (!_herocMATLABThread->isThreadRunning()) {
			if (_btnStartStopVPMATLAB)
				_btnStartStopVPMATLAB = false;
		}
	}	
}

//--------------------------------------------------------------
void ofAppMain::draw() {
	_guiSystem.draw(); 
	_guiExperiment.draw();
	_guiAdmittance.draw();

	// draw our own system error (ofxGUI doesn't allow me to properly resize ofxLabels
	ofPushMatrix();
		ofRectangle b = _fontErrorMsg.getStringBoundingBox(_errorMessage, 0, 0);	
		
		ofFill();
		ofSetColor(_errorMessageBackgroundColor);
		float padding = 6.0;
		float left = _guiSystem.getPosition()[0];
		float top = _guiSystem.getShape().getBottom()+2.0*padding;
		ofRectangle r = ofRectangle(left + 2.0, top - padding, _guiSystem.getWidth(), b.height + 2.0*padding);
		ofDrawRectangle(r);
		ofNoFill();
		ofSetColor(ofColor::gray);
		ofSetLineWidth(2.0);
		ofDrawRectangle(r);

		ofSetColor(ofColor::white);
		_fontErrorMsg.drawString(_errorMessage, left+padding, top-padding);
	ofPopMatrix();
}

//--------------------------------------------------------------
void ofAppMain::keyReleased(int key) {

}

//--------------------------------------------------------------
void ofAppMain::setupTCADS()
{
	// set up tcAdsClient for data reading
	_tcClientCont = new tcAdsClient(adsPort);

	// get variable handles for ADS
	char szVar0[] = { "Object1 (ModelBROS).Output.DataToADS" };
	_lHdlVar_Read_Data = _tcClientCont->getVariableHandle(szVar0, sizeof(szVar0));

	char szVar[] = { "Object1 (ModelBROS).BlockIO.xVP1" };
	_lHdlVar_DataVP1 = _tcClientCont->getVariableHandle(szVar, sizeof(szVar));

	char szVar9[] = { "Object1 (ModelBROS).BlockIO.xVP2" };
	_lHdlVar_DataVP2 = _tcClientCont->getVariableHandle(szVar9, sizeof(szVar9));

	// set up tcAdsClient for events
	_tcClientEvent = new tcAdsClient(adsPort);

	// get variables
	char szVar1[] = { "Object1 (ModelBROS).BlockIO.VecCon_SystemStates" };
	_lHdlVar_Read_SystemState = _tcClientEvent->getVariableHandle(szVar1, sizeof(szVar1));
	_lHdlNot_Read_SystemState = _tcClientEvent->registerTCAdsDeviceNotification(_lHdlVar_Read_SystemState, (unsigned long)(this), onEventCallbackTCADS, 16);

	char szVar2[] = { "Object1 (ModelBROS).BlockIO.VecCon_OpsEnabled" };
	_lHdlVar_Read_OpsEnabled = _tcClientEvent->getVariableHandle(szVar2, sizeof(szVar2));
	_lHdlNot_Read_OpsEnabled = _tcClientEvent->registerTCAdsDeviceNotification(_lHdlVar_Read_OpsEnabled, (unsigned long)(this), onEventCallbackTCADS, 2);

	char szVar3[] = { "Object1 (ModelBROS).BlockIO.VecCon_Errors" };
	_lHdlVar_Read_SystemError = _tcClientEvent->getVariableHandle(szVar3, sizeof(szVar3));
	_lHdlNot_Read_SystemError = _tcClientEvent->registerTCAdsDeviceNotification(_lHdlVar_Read_SystemError, (unsigned long)(this), onEventCallbackTCADS, 16);

	char szVar4[] = { "Object1 (ModelBROS).ModelParameters.CalibrateForceSensors_Value" };
	_lHdlVar_Write_CalibrateForceSensor = _tcClientEvent->getVariableHandle(szVar4, sizeof(szVar4));

	char szVar5[] = { "Object1 (ModelBROS).ModelParameters.Connected_Value" };
	_lHdlVar_Connected = _tcClientEvent->getVariableHandle(szVar5, sizeof(szVar5));

	char szVar6[] = { "Object1 (ModelBROS).ModelParameters.KpConnection_Value" };
	_lHdlVar_ConnectionStiffness = _tcClientEvent->getVariableHandle(szVar6, sizeof(szVar6));

	char szVar7[] = { "Object1 (ModelBROS).ModelParameters.KdConnection_Value" };
	_lHdlVar_ConnectionDamping = _tcClientEvent->getVariableHandle(szVar7, sizeof(szVar7));

	// Request State
	char szVar8[] = { "Object1 (ModelBROS).ModelParameters.Recorddata1yes0no_Value" };
	_lHdlVar_RecordData = _tcClientEvent->getVariableHandle(szVar8, sizeof(szVar8));
}
 
//--------------------------------------------------------------
void ofAppMain::setupGUI()
{

	// The GUI is set up 

	// add button listeners
	_btnCalibrateForceSensor.addListener(this, &ofAppMain::buttonPressed);
	_btnReqState_Reset.addListener(this, &ofAppMain::buttonPressed);
	_btnReqState_Init.addListener(this, &ofAppMain::buttonPressed);
	_btnReqState_Calibrate.addListener(this, &ofAppMain::buttonPressed);
	_btnReqState_HomingAuto.addListener(this, &ofAppMain::buttonPressed);
	_btnReqState_HomingManual.addListener(this, &ofAppMain::buttonPressed);
	_btnReqState_Run.addListener(this, &ofAppMain::buttonPressed);
	_btnEnableDrive.addListener(this, &ofAppMain::buttonPressed);
	_btnDisableDrive.addListener(this, &ofAppMain::buttonPressed);
	_btnExpLoad.addListener(this, &ofAppMain::buttonPressed);
	_btnExpStart.addListener(this, &ofAppMain::buttonPressed);
	_btnExpStop.addListener(this, &ofAppMain::buttonPressed);
	_btnConnSetStiffness.addListener(this, &ofAppMain::buttonPressed);
	_btnConnSetDamping.addListener(this, &ofAppMain::buttonPressed);
	_btnConfirmBlockTrialNr.addListener(this, &ofAppMain::buttonPressed);
	
	// setup GUIs
	float width = 300.0;
	_guiSystem.setDefaultWidth(width);
	_guiExperiment.setDefaultWidth(width);

	_guiSystem.setup("System Control");
	_guiSystem.setPosition(10.0, 10.0);
	
	_guiExperiment.setup("Experiment");
	_guiExperiment.setPosition(width + 40.0, 10.0);

	_guiAdmittance.setup("Admittance");
	_guiAdmittance.setPosition(2.0 * width + 70, 10.0);

	_guiSystem.setDefaultHeight(30.0);
	_guiExperiment.setDefaultHeight(30);
	_guiAdmittance.setDefaultHeight(30);
	
	// GUI system
	//_guiSystem.add(_btnQuit.setup("Quit"));
	_guiSystem.add(_lblEtherCAT.setup("EtherCAT/ADS", ""));
	_guiSystem.add(_lblFRM.set("Frame rate", ""));
	//_guiSystem.add(_btnCalibrateForceSensor.setup("Calibrate force sensors"));
	_guiSystem.loadFont("verdana.ttf", 10);

	// request state
	_grpReqState.setup("Request state");
	_grpReqState.setName("State request");
	_grpReqState.add(_btnReqState_Reset.setup("Reset"));
	_grpReqState.add(_btnReqState_Calibrate.setup("Calibrate"));
	_grpReqState.add(_btnReqState_HomingAuto.setup("Homing - Auto"));
	_grpReqState.add(_btnReqState_HomingManual.setup("Homing - Manual"));
	_grpReqState.add(_btnReqState_Run.setup("Run"));
	_guiSystem.add(&_grpReqState);

	// drive controls
	_grpDriveControl.setup("Drive control");
	_grpDriveControl.setName("Drive control");
	_grpDriveControl.add(_btnEnableDrive.setup("Enable drives"));
	_grpDriveControl.add(_btnDisableDrive.setup("Disable drives"));
	_guiSystem.add(&_grpDriveControl);

	_ofGrpSys.setName("System states");
	_ofGrpSys.add(_lblSysState[0].set("State 1", ""));
	_ofGrpSys.add(_lblSysState[1].set("State 2", ""));
	_ofGrpSys.add(_lblOpsEnabled.set("Drives Enabled", "[,]"));
	//_ofGrpSys.add(_lblSysError.set("System Error", "[,]"));
	_guiSystem.add(_ofGrpSys);
	//_guiSystem.add(_lblSysError.setup("System Error","[,]",width,40.0));

	// GUI experiment
	//_guiExperiment.add(_btnDebugMode.setup("Debug mode", false));
	_guiExperiment.add(_btnToggleRecordData.setup("Record data", false));
	_guiExperiment.add(_btnExpLoad.setup("Load"));
	_guiExperiment.add(lblExpLoaded.set("", "No protocol loaded"));
	_guiExperiment.add(lblExpState.set("ExpState", ""));

	_grpExpControl.setup("Experiment control");
	_grpExpControl.setName("Experiment control");
	_grpExpControl.add(_btnExpStart.setup("Start"));
	_grpExpControl.add(_btnExpStop.setup("Stop"));
	_grpExpControl.add(_btnExpPauseResume.setup("Pause", false));
	_guiExperiment.add(&_grpExpControl);

	_grpExpState.setName("Experiment state");
	_grpExpState.add(lblTrialPerformance.set("Trial Performance", "[,]"));
	_grpExpState.add(lblBlockNumber.set("Block number", 0, 0, 1));  // add dummy experiment
	_grpExpState.add(lblTrialNumber.set("Trial number", 0, 0, 1)); // add dummy experiment
	_guiExperiment.add(_grpExpState);
	_guiExperiment.add(_btnConfirmBlockTrialNr.setup("Set block and trial number"));

	_grpConnectionControl.setup("Connection control");
	_grpConnectionControl.setName("Connection control");
	_grpConnectionControl.add(_lblConnected.setup("Connection","disabled"));
	_grpConnectionControl.add(_btnSetConnected.setup("Enable connection", false));
	_btnSetConnected.setName("Enable connection");
	_grpConnectionControl.add(_lblConnStiffness.setup("Connection stiffness Kp", ofToString(0) + " N/m"));
	_grpConnectionControl.add(_btnConnSetStiffness.setup("Connection stiffness"));
	_grpConnectionControl.add(_lblConnDamping.setup("Connection damping Kd", ofToString(0) + " Ns/m"));
	_grpConnectionControl.add(_btnConnSetDamping.setup("Connection damping"));
	_grpConnectionControl.minimize(); // default is minimized
	_guiAdmittance.add(&_grpConnectionControl);

	_grpVirtualPartner.setup("Virtual partner control");
	_grpVirtualPartner.minimize();
	
	_grpVirtualPartner.setName("Virtual partner control");
	_grpVirtualPartner.add(_btnDrawVirtualPartner.setup("Draw virtual partner", false));
	_grpVirtualPartner.add(_btnStartStopVPMATLAB.setup("Press to start MATLAB VP on HEROC", false));
	_grpVirtualPartner.minimize();
	_guiAdmittance.add(&_grpVirtualPartner);

	_guiSystem.setWidthElements(width);
	_guiExperiment.setWidthElements(width);

	// set error message
	_errorMessage = "\n" + DecodeBROSError((int32_t)0, 1) + DecodeBROSError((int32_t)0, 2);

	// initialize GUI
	if (twinCatRunning) { updateADSDataGUI(); }

	// add toggle listeners
	_btnDrawVirtualPartner.addListener(this, &ofAppMain::drawVirtualPartnerPressed);
	_btnToggleRecordData.addListener(this, &ofAppMain::recordDataTogglePressed);
	_btnExpPauseResume.addListener(this, &ofAppMain::pauseExperimentTogglePressed);
	//_btnDebugMode.addListener(this, &ofAppMain::experimentDebugModeTogglePressed);
	_btnSetConnected.addListener(this, &ofAppMain::setConnectionEnabled);
	_btnStartStopVPMATLAB.addListener(this, &ofAppMain::startStopVPMATLAB);
}

//--------------------------------------------------------------
void ofAppMain::updateADSDataGUI()
{
	if (!twinCatRunning) return;

	// check record data
	bool record;
	_tcClientEvent->read(_lHdlVar_RecordData, &record, sizeof(record));
	_btnToggleRecordData = record;
	// give button a color
	if (record)
		_btnToggleRecordData.setBackgroundColor(ofColor::darkGreen);
	else
		_btnToggleRecordData.setBackgroundColor(ofColor::orange);

	// connection enabled?
	bool connected;
	
	_tcClientEvent->read(_lHdlVar_Connected, &connected, sizeof(connected));
	_btnSetConnected = connected;

	// connection stiffness
	double Kp;
	_tcClientEvent->read(_lHdlVar_ConnectionStiffness, &Kp, sizeof(Kp));
	_lblConnStiffness = ofToString(Kp, 1) + " N/m";

	// connection damping
	double Kd;
	_tcClientEvent->read(_lHdlVar_ConnectionDamping, &Kd, sizeof(Kd));
	_lblConnDamping = ofToString(Kd, 1) + " Ns/m";
}

//--------------------------------------------------------------
void ofAppMain::requestStateChange(int reqState)
{
	// button event in GUI, most likely a request to the ADS (TwinCAT), hence set up client
	//tcAdsClient* tcClient = new tcAdsClient(adsPort);

	// Request State
	char szVar[] = { "Object1 (ModelBROS).ModelParameters.Requestedstate_Value" };
	long lHdlVar = _tcClientEvent->getVariableHandle(szVar, sizeof(szVar));

	double state = (double)reqState; // cast to double because the simulink model/TMC object expects a double
	
	// write state request to variable handle
	_tcClientEvent->write(lHdlVar, &state, sizeof(state));

	// clean up
	//tcClient->disconnect();
}

//--------------------------------------------------------------
void ofAppMain::requestDriveEnableDisable(bool enable)
{
	long lHdlVar; // variable handle

	if (enable) { 
		// enable drives
		char szVar1[] = { "Object1 (ModelBROS).ModelParameters.EnableDrives_Value" };
		lHdlVar = _tcClientEvent->getVariableHandle(szVar1, sizeof(szVar1));
	}
	else {
		// disable drives
		char szVar1[] = { "Object1 (ModelBROS).ModelParameters.DisableDrives_Value" };
		lHdlVar = _tcClientEvent->getVariableHandle(szVar1, sizeof(szVar1));
	}
	
	// write 1 and 0 to variable handle (pulse)
	double val = 1.0;
	_tcClientEvent->write(lHdlVar, &val, sizeof(val));
	val = 0.0;
	_tcClientEvent->write(lHdlVar, &val, sizeof(val));
}

//--------------------------------------------------------------
void ofAppMain::buttonPressed(const void * sender)
{
	ofxButton * button = (ofxButton*)sender;
	string clickedBtn = button->getName();

	ofLogVerbose() << "(" << typeid(this).name() << ") " << "Button pressed: " << clickedBtn;

	if (clickedBtn.compare(ofToString("Reset")) == 0) {
		requestStateChange(0);
	} 
	else if (clickedBtn.compare(ofToString("Init")) == 0) {
		requestStateChange(1);
	}
	else if (clickedBtn.compare(ofToString("Calibrate")) == 0) {
		requestStateChange(2);
	}
	else if (clickedBtn.compare(ofToString("Homing - Auto")) == 0) {
		requestStateChange(302);
	}
	else if (clickedBtn.compare(ofToString("Homing - Manual")) == 0) {
		requestStateChange(301);
	}
	else if (clickedBtn.compare(ofToString("Run")) == 0) {
		requestStateChange(4);
	}
	else if (clickedBtn.compare(ofToString("Enable drives")) == 0) {
		requestDriveEnableDisable(true);
	}
	else if (clickedBtn.compare(ofToString("Disable drives")) == 0) {
		requestDriveEnableDisable(false);
	}
	else if (clickedBtn.compare(ofToString("Load")) == 0) {
		experimentApp->loadExperimentXML(); // load experiment XML
	}
	else if (clickedBtn.compare(ofToString("Start")) == 0) {
		// if the data recorder is not checked, force data recorder on.
		startDataLogger();
		// start experiment
		experimentApp->startExperiment();
	}
	else if (clickedBtn.compare(ofToString("Stop")) == 0) {
		stopDataLogger();
		experimentApp->stopExperiment();
	}
	else if (clickedBtn.compare(ofToString("Calibrate force sensors")) == 0) {
		calibrateForceSensors();
	}
	else if (clickedBtn.compare(ofToString("Connection stiffness")) == 0) {
		string s = ofSystemTextBoxDialog("Enter connection stiffness", "");

		try {
			if (s != "") setConnectionStiffness(strtod(s.c_str(), NULL));
		}
		catch (exception& e) {
			ofLogError() << "(" << typeid(this).name() << ") " << e.what();
		}
		
	}
	else if (clickedBtn.compare(ofToString("Connection damping")) == 0) {
		string s = ofSystemTextBoxDialog("Enter connection damping", "");
		try {
			if (s != "") setConnectionDamping(strtod(s.c_str(), NULL));
		}
		catch (exception& e) {
			ofLogError() << "(" << typeid(this).name() << ") " << e.what();
		}
	}
	else if (clickedBtn.compare(ofToString("Set block and trial number")) == 0) {
		setBlockTrialNumberByUser();
	}
	else {
		ofLogError() << "(" << typeid(this).name() << ") " << "Button " + clickedBtn + " unknown";
	}
}

//--------------------------------------------------------------
void ofAppMain::stopDataLogger()
{
	if (_btnToggleRecordData && _loggerStartedDueExperiment) { _btnToggleRecordData = false; }
}

//--------------------------------------------------------------
void ofAppMain::startDataLogger()
{
	if (!_btnToggleRecordData) { _loggerStartedDueExperiment = true; _btnToggleRecordData = true; }
}

//--------------------------------------------------------------
void ofAppMain::recordDataTogglePressed(bool & value)
{
	// write
	_tcClientEvent->write(_lHdlVar_RecordData, &value, sizeof(value));

	// give button a color
	if (value) 
		_btnToggleRecordData.setBackgroundColor(ofColor::darkGreen);
	else
		_btnToggleRecordData.setBackgroundColor(ofColor::orange);
}

//--------------------------------------------------------------
void ofAppMain::startStopVPMATLAB(bool & value)
{
	if (value) {
		// send request to HEROC computer
		_herocMATLABThread->startThread();

		ofLogNotice() << "(" << typeid(this).name() << ") " << "Sending request to start matlabVirtualPartner on HeRoC";

		_btnStartStopVPMATLAB.setName("MATLAB VP running, click to terminate");
	}
	else {
		// terminate the matlabVirtualPartner script running on the HeRoC computer by sending a XML file with one field: terminate
		ofXml xml;

		xml.addChild("VP");
		xml.setTo("VP");
		xml.addValue("terminate", true);

		string xmlfilename = "settings_vpmodelfit_trial_terminate.xml";
		xml.save(xmlfilename);

		ofLogVerbose() << ofToDataPath(xmlfilename);

		// copy to HeRoC pc
		ofLogNotice() << "(" << typeid(this).name() << ") " << "Sending terminate request to HeRoC";
		string cmd = ofToString("pscp -r -agent -i " + strSSHKey + " -pw " + pwKeyHeRoC +  " " + ofToDataPath(xmlfilename) + " " + userHeRoC + "@" + ipAddressHeRoC + ":" + matlabSettingsFilePath_HeRoC);
		int i = system(cmd.c_str());
		
		// clean up
		remove(xmlfilename.c_str());

		_btnStartStopVPMATLAB.setName("Press to start MATLAB VP on HEROC");
	}
		
}

//--------------------------------------------------------------
void ofAppMain::drawVirtualPartnerPressed(bool & value)
{
		display1->drawVirtualPartner = value;
		display2->drawVirtualPartner = value;
}

//--------------------------------------------------------------
void ofAppMain::pauseExperimentTogglePressed(bool & value)
{
	if (value) {
		_btnExpPauseResume.setName("Resume");
		experimentApp->pauseExperiment();
	}
	else {
		_btnExpPauseResume.setName("Pause");
		experimentApp->resumeExperiment();
	}
}

//--------------------------------------------------------------
void ofAppMain::experimentDebugModeTogglePressed(bool & value)
{
	experimentApp->debugMode = value;
}

//--------------------------------------------------------------
void ofAppMain::setConnectionEnabled(bool & value)
{
	// write value to connected
	try {
		bool v = value;
		_tcClientEvent->write(_lHdlVar_Connected, &v, sizeof(v));
	}
	catch (exception& e) {
		ofLogError() << "(" << typeid(this).name() << ") " << e.what();
	}

	// set toggle button name
	if (value) _lblConnected = "enabled";
	else _lblConnected = "disabled";
}

//--------------------------------------------------------------
void ofAppMain::setConnectionStiffness(double Kp)
{
	// write connection stiffness
	double val = Kp;
	_tcClientEvent->write(_lHdlVar_ConnectionStiffness, &val, sizeof(val));
	_lblConnStiffness = ofToString(Kp, 1) + " N/m";
}

//--------------------------------------------------------------
void ofAppMain::setConnectionDamping(double Kd)
{
	// write connection damping
	double val = Kd;
	_tcClientEvent->write(_lHdlVar_ConnectionDamping, &val, sizeof(val));
	_lblConnDamping = ofToString(Kd, 1) + " Ns/m";
}


//--------------------------------------------------------------
void ofAppMain::exit() {

	// switch off data logger (if this app switched it on)
	stopDataLogger();

	// remove listeners
	_btnCalibrateForceSensor.removeListener(this, &ofAppMain::buttonPressed);
	_btnReqState_Reset.removeListener(this, &ofAppMain::buttonPressed);
	_btnReqState_Init.removeListener(this, &ofAppMain::buttonPressed);
	_btnReqState_Calibrate.removeListener(this, &ofAppMain::buttonPressed);
	_btnReqState_HomingAuto.removeListener(this, &ofAppMain::buttonPressed);
	_btnReqState_HomingManual.removeListener(this, &ofAppMain::buttonPressed);
	_btnReqState_Run.removeListener(this, &ofAppMain::buttonPressed);
	_btnEnableDrive.removeListener(this, &ofAppMain::buttonPressed);
	_btnDisableDrive.removeListener(this, &ofAppMain::buttonPressed);
	_btnExpLoad.removeListener(this, &ofAppMain::buttonPressed);
	_btnExpStart.removeListener(this, &ofAppMain::buttonPressed);
	_btnExpStop.removeListener(this, &ofAppMain::buttonPressed);
	_btnConnSetStiffness.removeListener(this, &ofAppMain::buttonPressed);
	_btnConnSetDamping.removeListener(this, &ofAppMain::buttonPressed);
	_btnConfirmBlockTrialNr.removeListener(this, &ofAppMain::buttonPressed);

	_btnToggleRecordData = false;
	_btnToggleRecordData.removeListener(this, &ofAppMain::recordDataTogglePressed);
	_btnExpPauseResume.removeListener(this, &ofAppMain::pauseExperimentTogglePressed);
	_btnDebugMode.removeListener(this, &ofAppMain::experimentDebugModeTogglePressed);
	_btnSetConnected.removeListener(this, &ofAppMain::setConnectionEnabled);
	_btnStartStopVPMATLAB.removeListener(this, &ofAppMain::startStopVPMATLAB);
	_btnDrawVirtualPartner.removeListener(this, &ofAppMain::drawVirtualPartnerPressed);

	// disconnect ADS clients
	_tcClientCont->disconnect();
	_tcClientEvent->disconnect();
}

//--------------------------------------------------------------
void ofAppMain::calibrateForceSensors()
{
	// only allow force sensor calibration when in the following experiment states
	if (experimentApp->experimentState() == ExperimentState::BLOCKBREAK ||
		experimentApp->experimentState() == ExperimentState::IDLE ||
		experimentApp->experimentState() == ExperimentState::EXPERIMENTDONE) {

		double var = 1.0;
		_tcClientEvent->write(_lHdlVar_Write_CalibrateForceSensor, &var, sizeof(var));
		var = 0.0;
		_tcClientEvent->write(_lHdlVar_Write_CalibrateForceSensor, &var, sizeof(var));

		ofLogNotice() << "(" << typeid(this).name() << ") " << "Calibrate force sensor requested";
	}
	else {
		ofLogWarning() << "(" << typeid(this).name() << ") " << "Incorrect experiment state, force sensor calibration only allowed in state {IDLE, BLOCKBREAK, EXPERIMENTDONE})";
	}
}

//--------------------------------------------------------------
void ofAppMain::handleCallback(AmsAddr* pAddr, AdsNotificationHeader* pNotification)
{
	char buf[20];

	if (pNotification->hNotification == _lHdlNot_Read_OpsEnabled) {
		bool * data = (bool *)pNotification->data;
		sprintf(buf, "[%s,%s]", data[0] ? "T" : "F", data[1] ? "T" : "F");
		ofLogNotice() << "(" << typeid(this).name() << ") " << "Drive Enabled: " << ofToString(buf);
		_lblOpsEnabled = ofToString(buf);
	}
	else if (pNotification->hNotification == _lHdlNot_Read_SystemError) {
		double * data = (double *)pNotification->data;
		_errorMessage = "\n" + DecodeBROSError((int32_t)data[0], 1) + DecodeBROSError((int32_t)data[1], 2);
		
		if (data[0] != 0 || data[1] != 0) _errorMessageBackgroundColor = ofColor::red;
		else _errorMessageBackgroundColor = _guiSystem.getBackgroundColor();

		ofLogNotice() << DecodeBROSError((int32_t)data[0], 1) << DecodeBROSError((int32_t)data[1], 2);
	}
	else if (pNotification->hNotification == _lHdlNot_Read_SystemState) {
		double * data = (double *)pNotification->data;
		_systemState[0] = static_cast<SystemState>((int)data[0]);
		_systemState[1] = static_cast<SystemState>((int)data[1]);

		sprintf(buf, "[%d, %d]", _systemState[0], _systemState[1]);
		ofLogNotice() << "(" << typeid(this).name() << ") " << "System State: " << ofToString(buf);

		_lblSysState[0] = StringSystemStateLabel(_systemState[0]);
		_lblSysState[1] = StringSystemStateLabel(_systemState[1]);
	}
	
	// print (to screen)) the value of the variable 
	ofLogVerbose() << "(" << typeid(this).name() << ") " << "ADS Notification: " << ofToString(pNotification->hNotification) << " SampleSize: " << ofToString(pNotification->cbSampleSize);
}

//--------------------------------------------------------------
void ofAppMain::setBlockTrialNumberByUser() {
	// block and trial number set by user, relay this to ofAppExperiment
	ofLogNotice() << "(" << typeid(this).name() << ") " << "User set block and trial number";
	experimentApp->setCurrentTrialNumber(lblTrialNumber);
	experimentApp->setCurrentBlockNumber(lblBlockNumber);
}

//--------------------------------------------------------------
string ofAppMain::DecodeBROSError(int32_t e, int brosID) {
	// decode error message
	string s = "Errors BROS" + ofToString(brosID) + ":\n";
	bool anyError = false;
	for (int i = 0; i < sizeof(e)*8; ++i, e >>= 1) {
		if (e & 0x1) {
			s += "  " + _errorDescriptions[i] + "\n";
			anyError = true;
		}
	}

	if (!anyError) 
		s += "  No errors\n";

	return s;
}

//--------------------------------------------------------------
void __stdcall onEventCallbackTCADS(AmsAddr* pAddr, AdsNotificationHeader* pNotification, ULONG hUser)
{
	// cast hUser to class pointer
	ofAppMain* ptr = (ofAppMain*)(hUser);

	// call handleCallback for access to ofAppMain class
	ptr->handleCallback(pAddr, pNotification);
}
