#include "NoahKhanLabjack.h"
/**
 * program used for Watson Lab data collection via Labjack T7.
**/

// For printf
#include <stdio.h>

// For cin/cout
#include <iostream>

// For csv files
#include <fstream>
// For the LabJackM Library


// For time
#include <chrono>

#include<string>
#include "global.h"
#include "LJM_Utilities.h"




NoahKhanLabjack::NoahKhanLabjack(int frameNum, const char* names[MAX_FRAMES], const char* trigChannel, const char* serialNo, std::string boxName)
{

	
	err = LJM_Open(LJM_dtANY, LJM_ctANY, serialNo, &handle);
	ErrorCheck(err, "LJM_Open");
	std::string serialnumber(serialNo);
	this->boxName = boxName;
	//std::cout << this->boxName << std::endl;
	//printf("%d",handle);
	deviceType = GetDeviceType(handle);
	msDelay = READ_DELAY_MS;
	tempNumFrames = frameNum;
	triggerChannel = trigChannel; 


	//PrintDeviceInfoFromHandle(handle);
	for (int i = 0; i < tempNumFrames; i++) {
		aNames[i] = " ";
		tempNames[i] = names[i];
		aValues[i] = 2.36;//assigned random values.
	}
	numFrames = 0;
	
	
}

NoahKhanLabjack::~NoahKhanLabjack() {
	printf("Destructor called on LabJack \n");
	if (handle != -1) { // Check if handle is valid
		LJM_Close(handle);
	}
}


inline std::uint64_t getTime() {
	using namespace std::chrono;
	milliseconds ms = duration_cast<milliseconds>(
		system_clock::now().time_since_epoch()
	);
	
	return ms.count();
}




bool NoahKhanLabjack::printCheck() {
	bool print_check = false;
	for (int j = 0; j < numFrames; j++)
	{
		if (j == light_index and std::abs(aValues[j] - prev_aValues[j]) > LIGHT_CHANGE_THRESHOLD) {
			print_check = true;

		}
		else if (j == wheel_index and std::abs(aValues[j] - prev_aValues[j]) > WHEEL_CHANGE_THRESHOLD) {
			print_check = true;

		}
		else if (j != light_index and j != wheel_index and aValues[j] != prev_aValues[j]) {

			print_check = true;
		}
		prev_aValues[j] = aValues[j];

	}
	return print_check;



}




void NoahKhanLabjack::start() {
	
	printf("\nStarting the loop for LabJack : %s\n", this->boxName.c_str());

	initializeDevice();
	monitoringLoop();
	finalizeMonitoring();
}

void NoahKhanLabjack::initializeDevice() {
	// Initial light off attempt
	handleLJMError(LJM_eWriteAddress(handle, LIGHT_CONTROL_ADDRESS, 0, 0), "Error writing to address for turning off the light");

	// Start data collection interval
	err = LJM_StartInterval(INTERVAL_HANDLE, msDelay * 1000);
	ErrorCheck(err, "Start interval failed");

	// Initialize monitoring setup
	for (int i = 0; i < tempNumFrames; i++) {
		aNames[i] = tempNames[i];
		prev_aValues[i] = 0;  // Reset previous values
	}
	numFrames = tempNumFrames;
}

void NoahKhanLabjack::monitoringLoop() {
	int skippedIntervals;
	while (1) {
		uint64_t currentTime = getTime();
		updateLightStatus(currentTime);

		err = LJM_eReadNames(handle, numFrames, (const char**)aNames, aValues, &errorAddress);
		ErrorCheckWithAddress(err, errorAddress, "Read data failed");

		processSensorData(currentTime);

		// Handle timing for the next data interval
		err = LJM_WaitForNextInterval(INTERVAL_HANDLE, &skippedIntervals);
		ErrorCheck(err, "Waiting for next interval failed");
		if (skippedIntervals > 3) {
			printf("Skipped %d intervals.\n", skippedIntervals);
		}
	}
}

void NoahKhanLabjack::processSensorData(uint64_t currentTime) {
	if (this->boxName == global_control_LJ) {
		std::lock_guard<std::mutex> lock(flagMutexStop);
		printf("\n %f", aValues[0]);
		
		if (aValues[0] == 1 && check == 1)
		{
			printf("\n\n\nWaiting for the starting pulse on Labjack %s.\n", this->boxName.c_str());
			globalStopFlag = 1; check = 0;
		}
		else if (aValues[0] == 0 && check == 0) {
			check = 1; globalStopFlag = 0;
			name_timestamp = std::to_string(currentTime);
			
			printf("\nStarting Labjacks at time stamp %s : %s \n", this->boxName.c_str(),name_timestamp.c_str());
		}
	}

	if (globalStopFlag != 1 ) {

		DataRecord record{};
		try {
			record.digitalStatus = 0;
			uint8_t digital_status = 0;
			for (int j = 0; j < 8; ++j) {
				digital_status |= (static_cast<uint8_t>(aValues[j + digital_startindex]) << j);
			}
			record.digitalStatus = digital_status;

			constexpr double LIGHT_SCALE_FACTOR = 0.5;
			constexpr int MAX_LIGHT_CATEGORY = 9;
			int category = static_cast<int>(aValues[light_index] / LIGHT_SCALE_FACTOR);
			record.lightStatus = static_cast<unsigned char>((category <= MAX_LIGHT_CATEGORY) ? category : MAX_LIGHT_CATEGORY);

			record.pulse = static_cast<bool>(aValues[pulse_index]);  // Explicit cast for clarity
			record.timestamp = currentTime;  // Direct assignment
			record.wheel = static_cast<float>(aValues[wheel_index]);  // Type casting to float
			record.camera = static_cast<bool>(aValues[camera_index]);  // Explicit cast for clarity
		}
		catch (const std::runtime_error& e) {
			std::cerr << "Failed to convert data: " << e.what() << std::endl;

		}
		// Save data to binary file
		encodeAndSaveBinary(output_directory + "/" + this->boxName + "_LJ_" + name_timestamp + ".bin", record);


		if (print_feature) {
			if (printCheck()) {
				printSensorValues();
			}
		}
	}

	
}



void NoahKhanLabjack::encodeAndSaveBinary(const std::string& filename, const DataRecord& record) {
	std::ofstream outputFile(filename, std::ios::out | std::ios::binary | std::ios::app);

	if (!outputFile.is_open()) {
		std::cerr << "\nError opening binary file for writing." << std::endl;
		return;
	}

	outputFile.write(reinterpret_cast<const char*>(&record), sizeof(DataRecord));

	outputFile.close();

}

void NoahKhanLabjack::finalizeMonitoring() {
	err = LJM_CleanInterval(INTERVAL_HANDLE);
	ErrorCheck(err, "LJM_CleanInterval");

	err = LJM_Close(handle);
	ErrorCheck(err, "LJM_Close");

	WaitForUserIfWindows();
}




void NoahKhanLabjack::updateLightStatus(uint64_t currentTime) {
	std::time_t currentTimeSec = currentTime / 1000;
	std::tm* now_tm = std::localtime(&currentTimeSec);
	int currentMinuteOfDay = now_tm->tm_hour * 60 + now_tm->tm_min;

	bool shouldLightBeOn = shouldTurnLightOn(currentMinuteOfDay);
	if (shouldLightBeOn && !light_flag) {
		turnLightOn();
		light_flag = true;
	}
	else if (!shouldLightBeOn && light_flag) {
		turnLightOff();
		light_flag = false;
	}
}

bool NoahKhanLabjack::shouldTurnLightOn(int currentMinute) {
	if (startTimeMins <= stopTimeMins) {
		return currentMinute >= startTimeMins && currentMinute <= stopTimeMins && startTimeMins != stopTimeMins;
	}
	else { // Handles overnight scenario
		return currentMinute >= startTimeMins || currentMinute <= stopTimeMins;
	}
}

void NoahKhanLabjack::turnLightOn() {
	handleLJMError(LJM_eWriteAddress(handle, LIGHT_CONTROL_ADDRESS, 0, 1.0), "Turning on the light");
	printf("Light turned on for: %s\n", boxName.c_str());
}

void NoahKhanLabjack::turnLightOff() {
	handleLJMError(LJM_eWriteAddress(handle, LIGHT_CONTROL_ADDRESS, 0, 0.0), "Turning off the light");
	printf("Light turned off for: %s\n", boxName.c_str());
}

void NoahKhanLabjack::printSensorValues() {
	printf("\nSensor data for %s:\n", boxName.c_str());
	for (int i = 0; i < numFrames; i++) {
		printf("%s = %.4f\n", aNames[i], aValues[i]);
	}
}

inline void NoahKhanLabjack::handleLJMError(int errorCode, const std::string& message) {
	if (errorCode != LJME_NOERROR) {
		std::cerr << "Error: " << message << std::endl;
	}
}

