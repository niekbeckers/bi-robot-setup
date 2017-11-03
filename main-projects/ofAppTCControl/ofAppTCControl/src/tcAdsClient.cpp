#include "tcAdsClient.h"

//--------------------------------------------------------------
tcAdsClient::tcAdsClient(USHORT port)
{
	// Open communication port on the ADS router
	_nPort = AdsPortOpenEx();
	nErr = AdsGetLocalAddressEx(_nPort, _pAddr);
	if (nErr) {
		cerr << "[error] tcAdsClient: Error AdsGetLocalAddressEx: " << nErr << '\n';
		cerr << "[error] tcAdsClient: TC ADS client not running, please start TwinCAT and restart this app" << '\n';
	}

	// Select Port: TwinCAT 3 PLC
	_pAddr->port = port;
}

//--------------------------------------------------------------
ULONG tcAdsClient::getVariableHandle(char* szVarIn, int numBytes)
{
	unsigned long lHdlVar;
	// Fetch handle for the PLC variable 
	nErr = AdsSyncReadWriteReqEx2(
				_nPort, 
				_pAddr, 
				ADSIGRP_SYM_HNDBYNAME, 
				0x0, 
				sizeof(lHdlVar), 
				&lHdlVar, 
				numBytes, 
				szVarIn, 
				&_pcbReturn);

	if (nErr) {
		cerr << "[error] tcAdsClient: Error AdsSyncReadWriteReqEx2: " << nErr << " - variable " << szVarIn << " not found." << '\n';
		return -1;
	} 
	else {
		_hVariables.push_back(lHdlVar);
		return lHdlVar;
	}
}

//--------------------------------------------------------------
void tcAdsClient::releaseVariableHandle(ULONG hVar)
{
	// release handle
	nErr = AdsSyncWriteReqEx(
				_nPort, 
				_pAddr, 
				ADSIGRP_SYM_RELEASEHND, 
				0, 
				sizeof(hVar), 
				&hVar);

	if (nErr) cerr << "[error] tcAdsClient: Error AdsSyncWriteReq: " << nErr << '\n';
	_hVariables.erase(remove(_hVariables.begin(), _hVariables.end(), hVar), _hVariables.end()); // remove _hVariables from list
}

//--------------------------------------------------------------
ULONG tcAdsClient::registerTCAdsDeviceNotification(ULONG lhVar, ULONG lhUser, PAdsNotificationFuncEx callback, ULONG cbLength)
{

	// set the attributes of the notification
	AdsNotificationAttrib  _adsNotificationAttrib;
	_adsNotificationAttrib.cbLength = cbLength;
	_adsNotificationAttrib.nTransMode = ADSTRANS_SERVERONCHA;
	//_adsNotificationAttrib.nTransMode = ADSTRANS_SERVERCYCLE;
	_adsNotificationAttrib.nMaxDelay = 0;
	_adsNotificationAttrib.nCycleTime = 10000000; // 0.5sec 

	// initiate the transmission of the PLC-variable 
	ULONG hNotification;
	nErr = AdsSyncAddDeviceNotificationReqEx(
				_nPort,
				_pAddr,
				ADSIGRP_SYM_VALBYHND,
				lhVar,
				&_adsNotificationAttrib,
				callback,
				lhUser,
				&hNotification);

	if (nErr) cerr << "[error] tcAdsClient: Error AdsSyncAddDeviceNotificationReq: " << hex <<nErr << '\n';
	cout << "[verbose] tcAdsClient: Registered notification: " << hNotification << "\n";

	_hNotifications.push_back(hNotification); // add hUser to list
	return hNotification;
}

void tcAdsClient::unregisterTCAdsDeviceNotification(ULONG hNotification)
{
	// finish the transmission of the PLC-variable 
	nErr = AdsSyncDelDeviceNotificationReqEx(_nPort, _pAddr, hNotification);
	if (nErr) cerr << "[error] tcAdsClient: Error AdsSyncDelDeviceNotificationReq: " << nErr << '\n';
	
	_hNotifications.erase(remove(_hNotifications.begin(), _hNotifications.end(), hNotification), _hNotifications.end()); // remove hNotification from list
}

//--------------------------------------------------------------
void tcAdsClient::read(ULONG lHdlVar, void *pData, int numBytes)
{
	// Read values of the PLC variables (by handle)
	nErr = AdsSyncReadReqEx2(
				_nPort, 
				_pAddr, 
				ADSIGRP_SYM_VALBYHND, 
				lHdlVar, 
				numBytes, 
				pData, 
				&_pcbReturn);

	//if (nErr) cerr << "Error: AdsSyncReadReqEx2: " << nErr << '\n';
}

//--------------------------------------------------------------
void tcAdsClient::write(ULONG lHdlVar, void *pData, int numBytes)
{
	// Write to ADS (bytes).
	nErr = AdsSyncWriteReqEx(
				_nPort, // Port
				_pAddr, // Address
				ADSIGRP_SYM_VALBYHND, // IndexGroup 
				lHdlVar, // IndexOffset
				numBytes, // Size of data
				pData);

	//if (nErr) cerr << "Error: AdsSyncWriteReqEx: " << nErr << '\n';
}

//--------------------------------------------------------------
void tcAdsClient::disconnect()
{
	// unregister notifications
	while (_hNotifications.size() > 0) {
		unregisterTCAdsDeviceNotification(_hNotifications.back());
	}

	// unregister variable handles
	while (_hVariables.size() > 0) {
		releaseVariableHandle(_hVariables.back());
	}

	// close port
	nErr = AdsPortCloseEx(_nPort);
	if (nErr) cerr << "[error] tcAdsClient: Error AdsPortCloseEx: " << nErr << " on nPort: " << _nPort << '\n';
}




