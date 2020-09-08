// MT-Miner.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <map>
#include <vector>
#include <list>
#include <string>
#include <set>
#include <cassert>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <chrono>
#include <bitset>
#include <omp.h>
#include <iomanip>

#include "utils.h"
#include "HypergraphParser.h"
#include "MT_Miner.h"
#include "Logger.h"

///
///
///
class ArgumentParser
{
public:
	// arguments type
	enum ParameterType {
		VERBOSE_MODE,
		LOG_TO_FILE,
		OUTPUT_TO_FILE,
		USE_CLONE,
		NB_PARAM
	};

	std::vector<std::pair<ParameterType, std::string>> argumentValues;

	void buildParameters()
	{
		// give values to arguments
		argumentValues.push_back(std::pair<ParameterType, std::string>(ParameterType::VERBOSE_MODE, "--verbose"));
		argumentValues.push_back(std::pair<ParameterType, std::string>(ParameterType::LOG_TO_FILE, "--log"));
		argumentValues.push_back(std::pair<ParameterType, std::string>(ParameterType::OUTPUT_TO_FILE, "--output"));
		argumentValues.push_back(std::pair<ParameterType, std::string>(ParameterType::USE_CLONE, "--use-clone"));
	}

	void showUsage(const std::string& name)
	{
		std::cout << RED << "Usage: " << name << " intput <option(s)>"
			<< "Options:\n"
			<< "\t-h,--help\t\tShow this help message\n"
			<< "\t--verbose\t\ttrue/false\n"
			<< "\t--log\t\ttrue/false\n"
			<< "\t--output\t\ttrue/false\n" 
			<< RESET << std::endl;
	}

	std::map<ParameterType, std::string> extractArguments(int argc, char* argv[])
	{
		std::map<ParameterType, std::string> parameterList;
		// Parse the command line and the option file
		if (argc < ParameterType::NB_PARAM)
		{
			showUsage(argv[0]);
		}
		else
		{
			for (int i = 1; i < argc; ++i)
			{
				std::string arg = argv[i];
				if ((arg == "-h") || (arg == "--help"))
				{
					showUsage(argv[0]);
					break;
				}
				else
				{
					for_each(argumentValues.begin(), argumentValues.end(), [&](const std::pair< ParameterType, std::string>& argument) {
						std::string delimiter = "=";
						std::string token = arg.substr(0, arg.find(delimiter));
						std::string value = arg.substr(arg.find(delimiter) + 1, arg.size());

						if (token == argument.second)
						{
							parameterList[argument.first] = value;
						}
						});
				}
			}
		}
		return parameterList;
	}
};

// ----------------------------------------------------------------------------------------------------------- //

void runMinimalTransversals(const std::string& file, bool useCloneOptimization, bool verbose, bool useOutputFile, bool useOutputLogFile)
{
	std::cout << BOLDYELLOW << "\n***** Running MT Miner *****" << RESET << std::endl << std::endl;

	unsigned int objectCount = 0;
	unsigned int itemCount = 0;
	std::shared_ptr<HyperGraph> hypergraph;

	std::string logFile = file;
	logFile += ".log";
	Logger::init(logFile, verbose, useOutputLogFile);

	// parser file
	HypergraphParser parser;
	if (parser.parse(file))
	{
		// get data from parser
		hypergraph = parser.getHypergraph();
		objectCount = parser.getObjectCount();
		itemCount = parser.getItemCount();

		// allocate miner and compute minimal transversals
		MT_Miner miner(useCloneOptimization);
		if (miner.createBinaryRepresentation(hypergraph))
		{
			std::vector<std::shared_ptr<Itemset>> minimalTransversals = miner.computeMinimalTransversals();

			// save minimal transversals into a file
			if (useOutputFile)
			{
				std::string outFile = file;
				outFile += ".out";

				Logger::log(GREEN, "saving minimal transversals into file : ", outFile, "\n", RESET);
				std::ofstream outputStream;
				outputStream.open(outFile);
				for_each(minimalTransversals.begin(), minimalTransversals.end(), [&](const std::shared_ptr<Itemset>& elt) { outputStream << elt->toString() << std::endl; });
				outputStream.close();
			}
		}
	}
	Logger::close();
}

// ----------------------------------------------------------------------------------------------------------- //
//
//void runBitsetBenchmark()
//{
//	srand(time(NULL));
//
//	std::chrono::high_resolution_clock::time_point tick, tock;
//
//	const int bitset_size = 100000;
//
//	{
//		std::bitset<bitset_size> bitset_1, bitset_2, bitset_3;
//		for (unsigned int num = bitset_size; num--;)
//		{
//			bitset_1.set(rand() % bitset_size, rand() % 2);
//			bitset_2.set(rand() % bitset_size, rand() % 2);
//		}
//		tick = std::chrono::high_resolution_clock::now();
//		bitset_3 = bitset_1 | bitset_2;
//		tock = std::chrono::high_resolution_clock::now();
//		std::cout << "std::bitset	OR : " << std::setw(16) << std::chrono::duration_cast<std::chrono::nanoseconds>(tock - tick).count() << " nsecs" << std::endl;
//	}
//
//	{
//		CustomULBitset bitset_1(bitset_size), bitset_2(bitset_size), bitset_3(bitset_size);
//		for (unsigned int num = bitset_size; num--;)
//		{
//			bitset_1.set(rand() % bitset_size, rand() % 2);
//			bitset_2.set(rand() % bitset_size, rand() % 2);
//		}
//		tick = std::chrono::high_resolution_clock::now();
//		bitset_3 = bitset_1 | bitset_2;
//		tock = std::chrono::high_resolution_clock::now();
//		std::cout << "CustomULBitset	OR : " << std::setw(16) << std::chrono::duration_cast<std::chrono::nanoseconds>(tock - tick).count() << " nsecs" << std::endl;
//	}
//}

// ----------------------------------------------------------------------------------------------------------- //
// ----------------------------------------------------------------------------------------------------------- //

int main(int argc, char* argv[])
 {
	// http://research.nii.ac.jp/~uno/dualization.html
	//runBitsetBenchmark();
	
	if (argc <= 1)
	{
		std::cout << "Usage " << argv[0] << "<filename> <option><outputfile>" << std::endl;
		return 0;
	}
	std::string file = argv[1];

	// get parameter list
	ArgumentParser parser;
	parser.buildParameters();
	std::map<ArgumentParser::ParameterType, std::string> parameterList = parser.extractArguments(argc, argv);

	bool verboseMode = parameterList[ArgumentParser::VERBOSE_MODE] == "true" || parameterList[ArgumentParser::VERBOSE_MODE] == "True" || parameterList[ArgumentParser::VERBOSE_MODE] == "TRUE";
	bool useOutputLogFile = parameterList[ArgumentParser::LOG_TO_FILE] == "true" || parameterList[ArgumentParser::LOG_TO_FILE] == "True" || parameterList[ArgumentParser::LOG_TO_FILE] == "TRUE";
	bool useOutputFile = parameterList[ArgumentParser::OUTPUT_TO_FILE] == "true" || parameterList[ArgumentParser::OUTPUT_TO_FILE] == "True" || parameterList[ArgumentParser::OUTPUT_TO_FILE] == "TRUE";
	bool useCloneOptimization = parameterList[ArgumentParser::USE_CLONE] == "true" || parameterList[ArgumentParser::USE_CLONE] == "True" || parameterList[ArgumentParser::USE_CLONE] == "TRUE";

	runMinimalTransversals(file, useCloneOptimization, verboseMode, useOutputFile, useOutputLogFile);
}