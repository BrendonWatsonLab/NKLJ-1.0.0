#include "global.h"
#include <mutex>
#include <string>


std::mutex flagMutexStop;
int globalStopFlag = 1;
std::string global_control_LJ = "";
std::string name_timestamp = "";
std::string output_directory = "";
std::atomic<bool> print_feature = 0;
bool light_control = 0;
int startTimeMins = 0;
int stopTimeMins = 0;