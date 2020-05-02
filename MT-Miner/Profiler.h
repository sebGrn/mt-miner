#pragma once
#include <string>
#include <chrono>
#include <map>

#define START_PROFILING(name)	auto profiling##name = std::chrono::high_resolution_clock::now();
#define END_PROFILING(name)		if (Profiler::useProfiling) { Profiler::addDuration(name, std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - profiling##name)); }

class Profiler
{
public:
	static bool useProfiling;
	static std::map<std::string, std::chrono::duration<long long, std::milli>> functionDurationMap;

	static void addDuration(const std::string& functionName, const std::chrono::duration<long long, std::milli>& duration)
	{
		if (functionDurationMap.find(functionName) != functionDurationMap.end())
		{
			std::chrono::duration<long long, std::milli> lastDuration = functionDurationMap[functionName];
			lastDuration += duration;
			functionDurationMap[functionName] = lastDuration;
		}
		else
		{
			functionDurationMap[functionName] = duration;
		}
	}
};

