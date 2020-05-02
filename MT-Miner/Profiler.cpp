#include "Profiler.h"

bool Profiler::useProfiling = true;
std::map<std::string, std::chrono::duration<long long, std::milli>> Profiler::functionDurationMap;