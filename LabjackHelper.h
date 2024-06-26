#pragma once
//#include "NoahKhanLabjack.h"
#include <vector>

// For the LabJackM Library
#include "LabJackM.h"

class LabjackHelper
{
public:
	LabjackHelper();
	int findAllDevice(int * aSerials);
private:
	const int DeviceType = LJM_dtANY;
	const int ConnectionType = LJM_ctANY;
	enum { NumAddresses = 2 };
	const char* aNames[NumAddresses] = { "DEVICE_NAME_DEFAULT", "FIRMWARE_VERSION" };

	int err, i;
	int aAddresses[NumAddresses];
	int aTypes[NumAddresses];
	int aNumRegs[NumAddresses];
	int totalNumBytes;
	int maxNumFound = LJM_LIST_ALL_SIZE;
	unsigned char* aBytes = NULL;

	int aDeviceTypes[LJM_LIST_ALL_SIZE];
	int aConnectionTypes[LJM_LIST_ALL_SIZE];
	int aSerialNumbers[LJM_LIST_ALL_SIZE];
	int aIPAddresses[LJM_LIST_ALL_SIZE];
	int NumFound ;

	char IPv4String[LJM_IPv4_STRING_SIZE];

};