#pragma once

#include <iostream>
#include <windows.h>
#include <conio.h>
#include <vector>
#include <algorithm>

#include "C:\TwinCAT\AdsApi\TcAdsDll\Include\TcAdsDef.h"
#include "C:\TwinCAT\AdsApi\TcAdsDll\Include\TcAdsApi.h"

using namespace std;

class tcAdsClient
{
	private:
		AmsAddr _addr;
		PAmsAddr _pAddr = &_addr;
		long _nErr, _nPort;
		ULONG _pcbReturn;

		// params for event-driven interactions with ADS
		vector<ULONG> _hNotifications, _hVariables;
		

	public:
		tcAdsClient(USHORT port);

		ULONG getVariableHandle(char* szVarIn, int numBytes);
		void releaseVariableHandle(ULONG hVar);
		ULONG registerTCAdsDeviceNotification(ULONG lhUser, ULONG vhUser,PAdsNotificationFuncEx callback, ULONG cbLength = 8);
		void unregisterTCAdsDeviceNotification(ULONG hNotification);
		void read(ULONG lHdlVar, void *pData, int numBytes);
		void write(ULONG lHdlVar, void *pData, int numBytes);
		void disconnect();
};

