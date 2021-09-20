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
		LOG_TO_FILE,
		FILENAME_OUTPUT_LOG,
		USE_CLONE,
		MINIMAL_SIZE,
		USE_CONSJONCTIVE,
		USE_THRESHOLD,
		NB_PARAM
	};

	std::vector<std::pair<ParameterType, std::string>> argumentValues;

	void buildParameters()
	{
		// give values to arguments
		argumentValues.push_back(std::pair<ParameterType, std::string>(ParameterType::MINIMAL_SIZE, "--m"));
		argumentValues.push_back(std::pair<ParameterType, std::string>(ParameterType::LOG_TO_FILE, "--log"));
		argumentValues.push_back(std::pair<ParameterType, std::string>(ParameterType::FILENAME_OUTPUT_LOG, "--log-file"));
		argumentValues.push_back(std::pair<ParameterType, std::string>(ParameterType::USE_CLONE, "--use-clone"));
		argumentValues.push_back(std::pair<ParameterType, std::string>(ParameterType::USE_CONSJONCTIVE, "--consjonctiv"));
		argumentValues.push_back(std::pair<ParameterType, std::string>(ParameterType::USE_THRESHOLD, "--threshold"));
	}

	void showUsage(const std::string& name)
	{
		std::cout << RED << "Usage: " << name << " intput <option(s)>"
			<< "Options:\n"
			<< "\t-h,--help\t\tShow this help message\n"
			<< "\t--log\t\ttrue/false\n"
			<< "\t--log-file\t\tlog.txt\n"
			<< "\t--use-clone\t\ttrue/false\n"
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

void runMinimalTransversals(const std::string& file, bool useCloneOptimization, bool verbose, bool useOutputFile, 
	bool useOutputLogFile, const std::string& outputLogFile, bool useMinimalSizeOnly, bool useConsjonctive, float threshold)
{
	//std::cout << BOLDYELLOW << "\n***** Running MT Miner *****" << RESET << std::endl << std::endl;
	unsigned int objectCount = 0;
	unsigned int itemCount = 0;
	
	Logger::init(outputLogFile, verbose);
	Logger::setFilename(file);

	if (useConsjonctive)
		Itemset::itemsetType = Itemset::ItemsetType::CONSJONCTIVE;
	else
		Itemset::itemsetType = Itemset::ItemsetType::DISJUNCTIVE;

	// parser file
	HyperGraph hypergraph;
	if (hypergraph.load(file))
	{
		// allocate miner 
		MT_Miner miner(useCloneOptimization, useMinimalSizeOnly, threshold);

		// load hypergraph into formal context, then into binary representation
		miner.createBinaryRepresentation(hypergraph);
	
		// compute minimal transverses
		std::deque<std::shared_ptr<Itemset>> minimalTransversals;
		miner.computeMinimalTransversals(minimalTransversals);

		// save minimal transversals into a file
		if (useOutputFile)
		{
			std::string outFile = file;
			outFile += ".out";

			std::cout << "saving minimal transversals into file : " << outFile << "\n";
			std::ofstream outputStream;
			outputStream.open(outFile);
			for_each(minimalTransversals.begin(), minimalTransversals.end(), [&](const std::shared_ptr<Itemset> elt) { outputStream << elt->toString() << std::endl; });
			outputStream.close();			
		}

		for (auto it = minimalTransversals.begin(); it != minimalTransversals.end(); it++) { it->reset(); }
		minimalTransversals.clear();
	}
}


// ----------------------------------------------------------------------------------------------------------- //
// ----------------------------------------------------------------------------------------------------------- //

int main(int argc, char* argv[])
{
	// http://research.nii.ac.jp/~uno/dualization.html
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

	bool verboseMode = true;
	bool useOutputLog = false;
	std::string useOutputLogFile = "log.txt";
	bool useOutputFile = false;
	bool useCloneOptimization = true;
	bool useMinimalSizeOnly = false;
	bool useConsjonctive = false;
	float threshold = 1.0;
	
	if (parameterList.find(ArgumentParser::MINIMAL_SIZE) != parameterList.end())
		useMinimalSizeOnly = parameterList[ArgumentParser::MINIMAL_SIZE] == "true" || parameterList[ArgumentParser::MINIMAL_SIZE] == "True" || parameterList[ArgumentParser::MINIMAL_SIZE] == "TRUE";
	if (parameterList.find(ArgumentParser::LOG_TO_FILE) != parameterList.end())
		useOutputLog = parameterList[ArgumentParser::LOG_TO_FILE] == "true" || parameterList[ArgumentParser::LOG_TO_FILE] == "True" || parameterList[ArgumentParser::LOG_TO_FILE] == "TRUE";
	if (parameterList.find(ArgumentParser::FILENAME_OUTPUT_LOG) != parameterList.end())
		useOutputLogFile = parameterList[ArgumentParser::FILENAME_OUTPUT_LOG];
	if (parameterList.find(ArgumentParser::USE_CLONE) != parameterList.end())
		useCloneOptimization = parameterList[ArgumentParser::USE_CLONE] == "true" || parameterList[ArgumentParser::USE_CLONE] == "True" || parameterList[ArgumentParser::USE_CLONE] == "TRUE";
	if (parameterList.find(ArgumentParser::USE_CONSJONCTIVE) != parameterList.end())
		useConsjonctive = parameterList[ArgumentParser::USE_CONSJONCTIVE] == "true" || parameterList[ArgumentParser::USE_CONSJONCTIVE] == "True" || parameterList[ArgumentParser::USE_CONSJONCTIVE] == "TRUE";
	if (parameterList.find(ArgumentParser::USE_THRESHOLD) != parameterList.end())
		threshold = std::stof(parameterList[ArgumentParser::USE_THRESHOLD]);

	runMinimalTransversals(file, useCloneOptimization, verboseMode, useOutputFile, useOutputLog, useOutputLogFile, useMinimalSizeOnly, useConsjonctive, threshold);

	return 0;	
}