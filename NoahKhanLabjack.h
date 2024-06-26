#pragma once
/**
 * program used for Watson Lab data collection via Labjack T7.
**/

enum { MAX_FRAMES = 13 };
enum { MAX_LABJACKS = 4 };
#define INITIAL_ERR_ADDRESS -2;
#include <iostream>
#include <fstream>
#include <vector>
#include <cstdint>
#include <ctime>
#include <cstdlib>
#include <string>
#include <thread>
//#include "LabJackM.h"

// For LabJackM helper functions, such as OpenOrDie, PrintDeviceInfoFromHandle,
// ErrorCheck, etc., such as OpenOrDie, PrintDeviceInfoFromHandle,
// ErrorCheck, etc.
//#include "LJM_Utilities.h"
//#include "global.h"


// Delay between readings in milliseconds
constexpr int READ_DELAY_MS = 16;

// Threshold for detecting changes in light index
constexpr float LIGHT_CHANGE_THRESHOLD = 0.009;

// Threshold for detecting changes in wheel index
constexpr float WHEEL_CHANGE_THRESHOLD = 0.01;

// Address for writing to a LabJack register for light control
constexpr int LIGHT_CONTROL_ADDRESS = 2017;

const int INTERVAL_HANDLE = 1;



class NoahKhanLabjack
{
public:
	//NoahKhanLabJack();
	struct DataRecord {
		uint64_t timestamp;
		uint8_t digitalStatus; // 8-bit binary status represented as an integer
		unsigned char lightStatus;
		float wheel;
		bool pulse;
		bool camera;
	};
	NoahKhanLabjack(int frameNum, const char* names[MAX_FRAMES], const char* trigChannel, const char* serialNo, std::string boxName);
	~NoahKhanLabjack();

	//bool triggerCheck();
	bool printCheck();
	void start();
	void initializeDevice();
	void monitoringLoop();
	void processSensorData(uint64_t currentTime);
	void encodeAndSaveBinary(const std::string& filename, const DataRecord& record);
	void finalizeMonitoring();
	void handleLJMError(int errorCode, const std::string& message);
	void updateLightStatus(uint64_t currentTime);
	bool shouldTurnLightOn(int currentTime);
	void turnLightOn();
	void turnLightOff();
	void printSensorValues();
	std::string boxName;
private:
	int err;
	int handle;
	int deviceType;
	int msDelay;
	int numFrames;
	int errorAddress = INITIAL_ERR_ADDRESS;
	const int start_counter = 0;
	const int start_index = 0;//FIO2
	const int digital_startindex = 5;//EIO0
	const int light_index = 4;//AIN1
	const int pulse_index = 2;//FIO1
	const int wheel_index = 3;//AIO0
	const int camera_index = 1;//FIO0
	bool check = 1;
	bool print = false;
	bool light_flag = false;

	
	
	const char* aNames[MAX_FRAMES];
	double aValues[MAX_FRAMES];
	double prev_aValues[MAX_FRAMES];
	
	// temporary stuff all for the trigger channel
	const char* tempNames[MAX_FRAMES];
	const char* triggerChannel;
	int tempNumFrames;
	
};

