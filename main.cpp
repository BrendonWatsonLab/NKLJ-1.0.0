

#include "NoahKhanLabJack.h"


#include "LabjackHelper.h"
#include <map>
#include <algorithm>
#include "global.h"
#include "ThreadGuard.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>

bool parseTime(const std::string& input, int& hours, int& minutes) {
	std::istringstream stream(input);
	char sep;
	stream >> hours >> sep >> minutes;
	return stream && sep == ':' && hours >= 0 && hours < 24 && minutes >= 0 && minutes < 60;
}
bool areArraysSame(int arr1[], int arr2[], int size) {
	std::map<int, int> freq;

	// Count frequency of each element in arr1
	for (int i = 0; i < size; ++i) {
		freq[arr1[i]]++;
	}

	// Decrease frequency for each element in arr2, or return false if element not found
	for (int i = 0; i < size; ++i) {
		if (freq[arr2[i]] == 0) { // Element not found or frequency mismatch
			return false;
		}
		freq[arr2[i]]--;
	}

	return true;
}


void sortAccordingToBoxSerials(int target[], int order[], int size) {
	std::map<int, int> orderMap;
	for (int i = 0; i < size; ++i) {
		orderMap[order[i]] = i;
	}

	std::sort(target, target + size, [&orderMap](const int& a, const int& b) {
		return orderMap[a] < orderMap[b];
		});
}

std::string trim(const std::string& str) {
	size_t first = str.find_first_not_of(' ');
	if (first == std::string::npos) return "";
	size_t last = str.find_last_not_of(' ');
	return str.substr(first, (last - first + 1));
}

int main(int argc, char* argv[]) {
	std::cout << "\n\nStarting Noah Khan LabJack software v1.0\n\n";
	_sleep(1000);
	std::string filePath;
	std::string boxNames[4] = {};
	int boxSerials[4] = {};
	if (argc != 2) {
		std::cerr << "Usage: " << argv[0] << " <path_to_txt_file>" << std::endl;
		//filePath = "C:/Users/watsonlab/Downloads/LJack/configurations.txt";
		filePath = "C:/Users/WATSONLAB/Downloads/Noah's Folder/NKLJ(1.0.0)/configurations.txt";
	    std::cout << "\nUsing default path : " << filePath << std::endl;
	}
	else{
		filePath = argv[1];
		std::cout << "\nUsing the new path : " << filePath << std::endl;
	}

	
	std::ifstream file(filePath);

	if (!file) {
		std::cerr << "Error opening file, Please check if the location is : " << filePath << std::endl;
		_sleep(3000);
		return 1;
	}

	std::string line;
	std::unordered_map<std::string, std::string> values;
	std::string startTimerStr, stopTimerStr;
	try {
		while (std::getline(file, line)) {
			std::istringstream iss(line);
			std::string key, value, dummy;

			// Splitting the line into key and value
			if (std::getline(iss, key, ':') && std::getline(iss, dummy, '"') && std::getline(iss, value, '"')) {
				values[trim(key)] = trim(value); // Storing string values
			}
			else {
				// Reset stream state
				iss.clear();
				iss.seekg(0);

				// Try to read integer value
				if (std::getline(iss, key, ':') && (iss >> value)) {
					values[trim(key)] = trim(value); // Storing integer values as strings
				}
			}
		}
	

	file.close();

	// Outputting all values
	std::cout << "The Contents in Configuration File : " << std::endl;
	for (const auto& pair : values) {

		std::cout <<"\t"<<pair.first << " : " << pair.second << std::endl;
	}
	
		for (int i = 0; i < 4; ++i) {
			std::string nameKey = "labjack_" + std::to_string(i + 1) + "_name";
			std::string serialKey = "labjack_" + std::to_string(i + 1) + "_serial";

			// Check if the keys exist in the map
			if (values.find(nameKey) != values.end()) {
				boxNames[i] = values[nameKey];

			}
			else {
				std::cerr << "Name key " << nameKey << " not found." << std::endl;
			}

			if (values.find(serialKey) != values.end()) {
				// Use try-catch block for std::stoi to catch conversion exceptions
				try {
					boxSerials[i] = std::stoi(values[serialKey]);
				}
				catch (const std::exception& e) {
					std::cerr << "Error converting serial number for " << nameKey << ": " << e.what() << std::endl;
				}
			}
			else {
				std::cerr << "Serial key " << serialKey << " not found." << std::endl;
			}
		}
		std::string nameKey = "output_directory";
		if (values.find(nameKey) != values.end()) {
			output_directory = values[nameKey];

		}
		else {
			std::cerr << "Name key " << nameKey << " not found." << std::endl;
		}
		nameKey = "print_feature";
		if (values.find(nameKey) != values.end()) {
			print_feature = std::stoi(values[nameKey]);

		}
		else {
			std::cerr << "Name key " << nameKey << " not found." << std::endl;
		}
		nameKey = "controller_labjack";
		if (values.find(nameKey) != values.end()) {
			global_control_LJ = values[nameKey];

		}
		else {
			std::cerr << "Name key " << nameKey << " not found." << std::endl;
		}
		nameKey = "light_control";
		if (values.find(nameKey) != values.end()) {
			light_control = std::stoi(values[nameKey]);

		}
		else {
			std::cerr << "Name key " << nameKey << " not found." << std::endl;
		}
		nameKey = "light_time_on";
		if (values.find(nameKey) != values.end()) {
			startTimerStr = values[nameKey];

		}
		else {
			std::cerr << "Name key " << nameKey << " not found." << std::endl;
		}
		nameKey = "light_time_off";
		if (values.find(nameKey) != values.end()) {
			stopTimerStr = values[nameKey];

		}
		else {
			std::cerr << "Name key " << nameKey << " not found." << std::endl;
		}
	}
	catch (const std::exception& e) {
		std::cerr << "An exception occurred while reading the key value pairs: " << e.what() << std::endl;
		_sleep(3000);
		return 1;
	}

	int startHours, startMinutes, endHours, endMinutes;
	if (!parseTime(startTimerStr, startHours, startMinutes) || !parseTime(stopTimerStr, endHours, endMinutes)) {
		std::cerr << "Invalid time format. Please use HH:MM." << std::endl;
		_sleep(3000);
		return 1;
	}
	startTimeMins = startHours * 60 + startMinutes;
	stopTimeMins = endHours * 60 + endMinutes;
	int frames = 13; 
	const char* name[MAX_FRAMES] = { "FIO2","FIO0","FIO1","AIN0","AIN1","EIO0","EIO1","EIO2","EIO3","EIO4","EIO5","EIO6","EIO7" };
	int aSerials[LJM_LIST_ALL_SIZE];
	const char* trigChan = "FIO2"; 
	LabjackHelper ljh;
	
	int number_devices = ljh.findAllDevice(aSerials);
	printf("\nTotal number of devices connected : %d\n",number_devices);
	if (areArraysSame(boxSerials, aSerials, number_devices) and number_devices == 4) {
		std::cout << "The Serial numbers match, Sorting them.\n";
		sortAccordingToBoxSerials(aSerials, boxSerials, number_devices);
		std::cout << "Sorted Serial numbers : \n";
		for (int i = 0; i < number_devices; ++i) {
			std::cout << "\t"<<aSerials[i] << std::endl;
		}
	}
	else {
		std::cout << "The connected LabJack Serial Numbers do not match with the ones in the configuration file.\n";
		_sleep(3000);
		return 1;
	}
	

	std::vector<std::shared_ptr<NoahKhanLabjack>> LabJacks;
	LabJacks.reserve(number_devices);

	for (int i = 0; i < number_devices; i++) {
		// Create NoahKhanLabjack instances wrapped in std::shared_ptr
		LabJacks.push_back(std::make_shared<NoahKhanLabjack>(frames, name, trigChan, std::to_string(aSerials[i]).c_str(), boxNames[i]));
	}

	std::vector<std::thread> threads;
	for (auto& LabJack : LabJacks) {
		threads.emplace_back([LabJack]() {
			LabJack->start();
			});
	}

	// Instantiate ThreadGuard right after thread creation
	ThreadGuard guard(threads);


}