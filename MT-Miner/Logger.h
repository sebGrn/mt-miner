#pragma once
#include <string>
#include <iostream>
#include <fstream>

class Logger
{
	static bool verbose;
	static bool verboseIntoFile;
	static std::string filename;
	static std::ofstream fileStream;

public:
	static void init(const std::string& _filename, bool _verbose, bool _verboseToFile)
	{
		filename = _filename;
		verbose = _verbose;
		verboseIntoFile = _verboseToFile;

		if (verboseIntoFile)
		{
			fileStream = std::ofstream(filename, std::ofstream::out);
		}
	}
	
	static void setVerbose(bool _verbose)
	{
		verbose = _verbose;
	}

	static void close()
	{
		if (verboseIntoFile)
		{
			fileStream.close();
		}
	}

	template <typename T>
	static void log(T t)
	{
		if (verbose)
		{
			std::cout << t;
		}
		if (verboseIntoFile)
		{
			fileStream << t;
		}
	}

	// recursive variadic function with beautiful templates
	template<typename T, typename... Args>
	static void log(T t, Args... args)
	{
		if (verbose)
		{
			std::cout << t;
		}
		if (verboseIntoFile)
		{
			fileStream << t;
		}

		log(args...);
	}


	template <typename T>
	static void forceLog(T t)
	{
		std::cout << t;
	}

	// recursive variadic function with beautiful templates
	template<typename T, typename... Args>
	static void forceLog(T t, Args... args)
	{
		std::cout << t;

		forceLog(args...);
	}
};