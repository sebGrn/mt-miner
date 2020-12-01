#pragma once

#include <cassert>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <algorithm>
#include "windows.h"
#include "psapi.h"

#define NOMINMAX
#undef max
#undef min

class Utils
{
public:
	struct compare_second_value_of_pair
	{
		unsigned int key;
		compare_second_value_of_pair(unsigned int const& i) : key(i) { }

		// check that original (it1->first) is not already registered as a clone (it2->first) in clonedBitsetIndexes
		bool operator()(std::pair<unsigned int, unsigned int> const& item)
		{
			return item.second == key;
		}
	};

	/// <summary>
	/// extract a list of int from a string
	/// </summary>
	/// <param name="s"></param>
	/// <param name="delimiter"></param>
	/// <returns></returns>
	template <typename Out>
	static void split(const std::string& s, char delim, Out result)
	{
		std::istringstream iss(s);
		std::string item;
		while (std::getline(iss, item, delim)) {
			*result++ = std::stoi(item);
		}
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="s"></param>
	/// <param name="delimiter"></param>
	/// <returns></returns>
	static std::vector<unsigned int> splitToVectorOfInt(const std::string& s, char delim)
	{
		std::vector<unsigned int> elems;
		split(s, delim, std::back_inserter(elems));
		return elems;
	}

	// ------------------------------------------------------------------------------------------------------------------------- //

	static std::string& ltrim(std::string& str, const std::string& chars = "\t\n\v\f\r ")
	{
		str.erase(0, str.find_first_not_of(chars));
		return str;
	}

	static std::string& rtrim(std::string& str, const std::string& chars = "\t\n\v\f\r ")
	{
		str.erase(str.find_last_not_of(chars) + 1);
		return str;
	}

	static std::string& trim(std::string& str, const std::string& chars = "\t\n\v\f\r ")
	{
		return ltrim(rtrim(str, chars), chars);
	}

	// ------------------------------------------------------------------------------------------------------------------------- //

	static void printTotalVirtualMemory()
	{
		MEMORYSTATUSEX memInfo;
		memInfo.dwLength = sizeof(MEMORYSTATUSEX);
		GlobalMemoryStatusEx(&memInfo);
		// size of swap file plus installed RAM
		DWORDLONG totalVirtualMem = memInfo.ullTotalPageFile;
		std::cout << "size of swap file plus installed RAM " << totalVirtualMem << std::endl;
		// total virtual memory
		DWORDLONG totalPhysMem = memInfo.ullTotalPhys;
		std::cout << "total virtual memory " << totalPhysMem << std::endl;
	}

	static SIZE_T printUsedMemoryForCrtProcess()
	{
		PROCESS_MEMORY_COUNTERS_EX pmc;
		GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
		SIZE_T virtualMemUsedByMe = pmc.PrivateUsage;
		std::cout << "used memory for tProcess " << virtualMemUsedByMe / 1000000000.0 << " Gb" << std::endl;
		return virtualMemUsedByMe;		
	}

};