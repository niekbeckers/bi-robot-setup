#pragma once

#include <iostream>
#include <windows.h>
#include <conio.h>

#include "C:\TwinCAT\AdsApi\TcAdsDll\Include\TcAdsDef.h"
#include "C:\TwinCAT\AdsApi\TcAdsDll\Include\TcAdsApi.h"

using namespace std;

class tcAdsClient
{
	private:
		AmsAddr _addr;
		PAmsAddr _pAddr = &_addr;
		long _nErr, _nPort;
		unsigned long _pcbReturn;

	public:
		tcAdsClient(unsigned long port);
		~tcAdsClient();

		unsigned long getVariableHande(char* szVarIn, int numBytes);
		void Read(unsigned long lHdlVar, void *pData, int numBytes);
		void Write(unsigned long lHdlVar, void *pData, int numBytes);
		void Disconnect();
};

