#include "Profiler.h"

bool Profiler::useProfiling = true;
std::map<std::string, std::chrono::duration<__int64, std::milli>> Profiler::functionDurationMap;