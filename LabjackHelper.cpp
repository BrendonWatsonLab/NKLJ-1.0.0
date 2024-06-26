#include "LabjackHelper.h"

#include <string>


// Create a vector containing pointers (references) to Labjack Objects

	LabjackHelper::LabjackHelper() {}
	int LabjackHelper::findAllDevice(int* aSerials){
	//std::vector<NoahKhanLabjack*> outputVector = {};
	err = LJM_ListAllExtended(DeviceType, ConnectionType, NumAddresses, aAddresses, aNumRegs, maxNumFound, &NumFound, aDeviceTypes, aConnectionTypes, aSerialNumbers, aIPAddresses, aBytes);
	for (int i = 0; i < NumFound; i++) {
		aSerials[i] = aSerialNumbers[i];
		
	}
	return NumFound;
	}

