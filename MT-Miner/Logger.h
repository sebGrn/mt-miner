#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <cstdio>

//the following are UBUNTU/LINUX, and MacOS ONLY terminal color codes.
#define RESET		"\033[0m"
#define BLACK		"\033[30m"				// Black
#define RED			"\033[31m"				// Red
#define GREEN		"\033[32m"				// Green
#define YELLOW		"\033[33m"				// Yellow
#define BLUE		"\033[34m"				// Blue
#define MAGENTA		"\033[35m"				// Magenta
#define CYAN		"\033[36m"				// Cyan
#define WHITE		"\033[37m"				// White
#define BOLDBLACK   "\033[1m\033[30m"		// Bold Black
#define BOLDRED     "\033[1m\033[31m"		// Bold Red
#define BOLDGREEN   "\033[1m\033[32m"		// Bold Green
#define BOLDYELLOW  "\033[1m\033[33m"		// Bold Yellow
#define BOLDBLUE    "\033[1m\033[34m"		// Bold Blue 
#define BOLDMAGENTA "\033[1m\033[35m"		// Bold Magenta
#define BOLDCYAN    "\033[1m\033[36m"		// Bold Cyan 
#define BOLDWHITE   "\033[1m\033[37m"		// Bold White 

class Logger
{
	static std::string filename;

public:
	struct dataset_row
	{
		std::string filename;
		double computeTime;
		double sparcity;
		unsigned int cloneCount;
		unsigned int itemCount;
		unsigned int objectCount;
		unsigned int minimalTransverseCount;
		unsigned int minimalSizeOfTransverse;
	};
	static dataset_row dataset;

public:
	static void init(const std::string& _filename, bool _verboseToScreen = false)
	{
		filename = _filename;
	}

	static void setFilename(const std::string& filename)
	{
		dataset.filename = filename;
	}

	/*static void setComputeTime(double time)
	{
		dataset.computeTime = time;
	}

	static void setSparcity(double sparcity)
	{
		dataset.sparcity = sparcity;
	}

	static void setCloneCount(double cloneCount)
	{
		dataset.cloneCount = cloneCount;
	}

	static void setItemCount(double itemCount)
	{
		dataset.itemCount = itemCount;
	}

	static void setObjectCount(double objectCount)
	{
		dataset.objectCount = objectCount;
	}*/

	static void log()
	{
		std::ofstream fileStream = std::ofstream(filename, std::ofstream::out | std::ofstream::app);

		fileStream << dataset.filename << ";"
			<< dataset.sparcity << ";"
			<< dataset.cloneCount << ";"
			<< dataset.itemCount << ";"
			<< dataset.objectCount << ";"
			<< dataset.minimalTransverseCount << ";"
			<< dataset.minimalSizeOfTransverse << ";"
			<< dataset.computeTime << std::endl;
		fileStream.close();
	}

	//template <typename T>
	//static void log(T t)
	//{
	//	if (verboseToScreen)
	//	{
	//		std::cout << t;
	//	}
	//	if (verboseIntoFile)
	//	{
	//		fileStream << t;
	//	}
	//}

	//// recursive variadic function with beautiful templates
	//template<typename T, typename... Args>
	//static void log(T t, Args... args)
	//{
	//	if (verboseToScreen)
	//	{
	//		std::cout << t;
	//	}
	//	if (verboseIntoFile)
	//	{
	//		fileStream << t;
	//	}
	//	log(args...);
	//}
};