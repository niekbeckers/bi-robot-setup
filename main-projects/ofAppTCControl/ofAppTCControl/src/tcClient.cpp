#include "tcClient.h"



tcClient::tcClient(unsigned long port)
{
	// Open communication port on the ADS router
	_nPort = AdsPortOpenEx();
	_nErr = AdsGetLocalAddressEx(_nPort, _pAddr);
	if (_nErr) cerr << "Error: AdsGetLocalAddressEx: " << _nErr << '\n';

	// Select Port: TwinCAT 3 PLC
	_pAddr->port = port;
}

tcClient::~tcClient()
{
}

unsigned long tcClient::getVariableHande(char* szVarIn, int numBytes)
{
	unsigned long lHdlVar;
	// Fetch handle for the PLC variable 
	_nErr = AdsSyncReadWriteReqEx2(_nPort, _pAddr, ADSIGRP_SYM_HNDBYNAME, 0x0, sizeof(lHdlVar), &lHdlVar, numBytes, szVarIn, &_pcbReturn);

	if (_nErr) {
		cerr << "Error: AdsSyncReadWriteReqEx2: " << _nErr << " - variable " << szVarIn << " not found." << '\n';
		return 0;
	} 
	else {
		return lHdlVar;
	}
}

void tcClient::Read(unsigned long lHdlVar, void *pData, int numBytes)
{
	// Read values of the PLC variables (by handle)
	_nErr = AdsSyncReadReqEx2(_nPort, _pAddr, ADSIGRP_SYM_VALBYHND, lHdlVar, numBytes, pData, &_pcbReturn);
	if (_nErr) cerr << "Error: AdsSyncReadReqEx2: " << _nErr << '\n';
}

void tcClient::Write(unsigned long lHdlVar, void *pData, int numBytes)
{
	// Write to ADS (bytes).
	_nErr = AdsSyncWriteReqEx(
				_nPort, // Port
				_pAddr, // Address
				ADSIGRP_SYM_VALBYHND, // IndexGroup 
				lHdlVar, // IndexOffset
				numBytes, // Size of data
				pData);
	if (_nErr) cerr << "Error: AdsSyncWriteReqEx: " << _nErr << '\n';
}

void tcClient::Disconnect()
{
	_nErr = AdsPortCloseEx(_nPort);
	if (_nErr) cerr << "Error: AdsPortCloseEx: " << _nErr << " on nPort: " << _nPort << '\n';
}


