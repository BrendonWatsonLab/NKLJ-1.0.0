#ifndef GLOBAL_H
#define GLOBAL_H
#include <mutex>
#include <string>


extern std::mutex flagMutexStop;
extern int globalStopFlag;
extern std::string global_control_LJ;
extern std::string name_timestamp;
extern std::string output_directory;
extern bool print_feature;
extern bool light_control;
extern int startTimeMins;
extern int stopTimeMins;

#endif // GLOBAL_H