﻿// MT-Miner.cpp : This file contains the 'main' function. Program execution begins and ends there.
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

using Itemset = std::vector<unsigned int>;

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
		USE_CLONE,
		NB_PARAM
	};

	std::vector<std::pair<ParameterType, std::string>> argumentValues;

	void buildParameters()
	{
		// give values to arguments
		argumentValues.push_back(std::pair<ParameterType, std::string>(ParameterType::VERBOSE_MODE, "--verbose"));
		argumentValues.push_back(std::pair<ParameterType, std::string>(ParameterType::LOG_TO_FILE, "--log"));
		argumentValues.push_back(std::pair<ParameterType, std::string>(ParameterType::USE_CLONE, "--use-clone"));
	}

	void showUsage(const std::string& name)
	{
		std::cout << "Usage: " << name << " intput <option(s)>"
			<< "Options:\n"
			<< "\t-h,--help\t\tShow this help message\n"
			<< "\t--verbose\t\ttrue/false\n"
			<< "\t--log\t\ttrue/false\n"
			<< "\t--use-clone\t\ttrue/false\n"
			<< std::endl;
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

void runMinimalTransversals(const std::string& file, bool useCloneOptimization, bool verbose, bool useOutputLogFile)
{
	std::cout << "computing minimal transversals on file " << file << std::endl;

	unsigned int objectCount = 0;
	unsigned int itemCount = 0;
	std::shared_ptr<HyperGraph> hypergraph;

	std::string logFile = file;
	logFile += ".log";
	Logger::init(logFile, verbose, useOutputLogFile);

	// parsing
	auto beginTime = std::chrono::system_clock::now();
	
	HypergraphParser parser;
	if (parser.parse(file))
	{
		// get data from parser
		hypergraph = parser.getHypergraph();
		objectCount = parser.getObjectCount();
		itemCount = parser.getItemCount();

		int64_t duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - beginTime).count();
		Logger::setVerbose(true);
		Logger::log("parsing hypergraph done in ", duration, " ms\n");
		Logger::log("itemCount ", itemCount, "\n");
		Logger::log("objectCount ", objectCount, "\n\n");
		Logger::setVerbose(verbose);

		// minimal transversals computing
		int64_t isEssentialDuration = 0;
		std::vector<Itemset> minimalTransversals;
		beginTime = std::chrono::system_clock::now();
		{
			MT_Miner miner(useCloneOptimization);
			std::vector<Itemset> toTraverse;
			miner.init(hypergraph, toTraverse);

			// compute minimal transversals		
			minimalTransversals = miner.computeMinimalTransversals(toTraverse);
			isEssentialDuration = miner.getIsEssentialDuration();
		}

		Logger::close();

		Logger::setVerbose(true);
		duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - beginTime).count();
		Logger::log("\nminimal transversals done in ", duration, " ms\n");
		Logger::log("isEssential total duraton ", isEssentialDuration, " ms\n\n");
		Logger::setVerbose(verbose);

		// sort transversals itemset
		//minimalTransversals = sortVectorOfItemset(minimalTransversals);

		// print minimal transversals	
		std::cout << "minimal transversals count : " << minimalTransversals.size() << std::endl;
		if (minimalTransversals.size() > 6)
			for_each(minimalTransversals.begin(), minimalTransversals.begin() + 5, [&](const Itemset& elt) { std::cout << Utils::itemsetToString(elt) << std::endl; });
		else
			for_each(minimalTransversals.begin(), minimalTransversals.end(), [&](const Itemset& elt) { std::cout << Utils::itemsetToString(elt) << std::endl; });
		std::cout << "..." << std::endl << std::endl;

		// ----------------------------------------------------- //

		// save minimal transversals into a file
		if (useOutputLogFile)
		{
			std::string outFile = file;
			outFile += ".out";

			std::cout << "saving minimal transversals into file : " << outFile << std::endl;
			std::ofstream outputStream;
			outputStream.open(outFile);
			for_each(minimalTransversals.begin(), minimalTransversals.end(), [&](const Itemset& elt) { outputStream << Utils::itemsetToString(elt) << std::endl; });
			outputStream.close();
		}
	}
}

// ----------------------------------------------------------------------------------------------------------- //

int main(int argc, char* argv[])
{
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
	bool useCloneOptimization = parameterList[ArgumentParser::USE_CLONE] == "true" || parameterList[ArgumentParser::USE_CLONE] == "True" || parameterList[ArgumentParser::USE_CLONE] == "TRUE";

	runMinimalTransversals(file, useCloneOptimization, verboseMode, useOutputLogFile);
}