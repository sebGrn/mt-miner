#include "Profiler.h"

bool Profiler::useProfiling = false;
std::map<std::string, std::chrono::duration<long long, std::milli>> Profiler::functionDurationMap;