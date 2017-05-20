#include "tcAdsClient.h"



tcAdsClient::tcAdsClient(USHORT port)
{
	// Open communication port on the ADS router
	_nPort = AdsPortOpenEx();
	_nErr = AdsGetLocalAddressEx(_nPort, _pAddr);
	if (_nErr) cerr << "Error: AdsGetLocalAddressEx: " << _nErr << '\n';

	// Select Port: TwinCAT 3 PLC
	_pAddr->port = port;
}

tcAdsClient::~tcAdsClient()
{
}

ULONG tcAdsClient::getVariableHandle(char* szVarIn, int numBytes)
{
	unsigned long lHdlVar;
	// Fetch handle for the PLC variable 
	_nErr = AdsSyncReadWriteReqEx2(
				_nPort, 
				_pAddr, 
				ADSIGRP_SYM_HNDBYNAME, 
				0x0, 
				sizeof(lHdlVar), 
				&lHdlVar, 
				numBytes, 
				szVarIn, 
				&_pcbReturn);

	if (_nErr) {
		cerr << "Error: AdsSyncReadWriteReqEx2: " << _nErr << " - variable " << szVarIn << " not found." << '\n';
		return 0;
	} 
	else {
		_hVariables.push_back(lHdlVar);
		return lHdlVar;
	}
}

void tcAdsClient::releaseVariableHandle(ULONG hVar)
{
	// release handle
	_nErr = AdsSyncWriteReqEx(
				_nPort, 
				_pAddr, 
				ADSIGRP_SYM_RELEASEHND, 
				0, 
				sizeof(hVar), 
				&hVar);

	if (_nErr) cerr << "Error: AdsSyncWriteReq: " << _nErr << '\n';
	_hVariables.erase(remove(_hVariables.begin(), _hVariables.end(), hVar), _hVariables.end()); // remove _hVariables from list
}

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
	_nErr = AdsSyncAddDeviceNotificationReqEx(
				_nPort,
				_pAddr,
				ADSIGRP_SYM_VALBYHND,
				lhVar,
				&_adsNotificationAttrib,
				callback,
				lhUser,
				&hNotification);

	if (_nErr) cerr << "Error: AdsSyncAddDeviceNotificationReq: " << hex <<_nErr << '\n';
	cout << "Notification: " << hNotification << "\n\n";

	_hNotifications.push_back(hNotification); // add hUser to list
	return hNotification;
}

void tcAdsClient::unregisterTCAdsDeviceNotification(ULONG hNotification)
{
	// finish the transmission of the PLC-variable 
	_nErr = AdsSyncDelDeviceNotificationReqEx(_nPort, _pAddr, hNotification);
	if (_nErr) cerr << "Error: AdsSyncDelDeviceNotificationReq: " << _nErr << '\n';
	
	_hNotifications.erase(remove(_hNotifications.begin(), _hNotifications.end(), hNotification), _hNotifications.end()); // remove hNotification from list
}

void tcAdsClient::read(ULONG lHdlVar, void *pData, int numBytes)
{
	// Read values of the PLC variables (by handle)
	_nErr = AdsSyncReadReqEx2(
				_nPort, 
				_pAddr, 
				ADSIGRP_SYM_VALBYHND, 
				lHdlVar, 
				numBytes, 
				pData, 
				&_pcbReturn);

	//if (_nErr) cerr << "Error: AdsSyncReadReqEx2: " << _nErr << '\n';
}

void tcAdsClient::write(ULONG lHdlVar, void *pData, int numBytes)
{
	// Write to ADS (bytes).
	_nErr = AdsSyncWriteReqEx(
				_nPort, // Port
				_pAddr, // Address
				ADSIGRP_SYM_VALBYHND, // IndexGroup 
				lHdlVar, // IndexOffset
				numBytes, // Size of data
				pData);

	//if (_nErr) cerr << "Error: AdsSyncWriteReqEx: " << _nErr << '\n';
}

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
	_nErr = AdsPortCloseEx(_nPort);
	if (_nErr) cerr << "Error: AdsPortCloseEx: " << _nErr << " on nPort: " << _nPort << '\n';
}




