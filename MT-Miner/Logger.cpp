#include "Logger.h"

bool Logger::verbose = false;
bool Logger::verboseIntoFile = false;;
std::string Logger::filename;
std::ofstream Logger::fileStream;