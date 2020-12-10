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
		VERBOSE_MODE,
		LOG_TO_FILE,
		FILENAME_OUTPUT_LOG,
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
		argumentValues.push_back(std::pair<ParameterType, std::string>(ParameterType::FILENAME_OUTPUT_LOG, "--log-file"));
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
			<< "\t--log-file\t\toutput.txt\n"
			<< "\t--output\t\ttrue/false\n" 
			<< RESET << std::endl;
	}

	std::map<ParameterType, std::string> extractArguments(int argc, char* argv[])
	{
		std::map<ParameterType, std::string> parameterList;
		// Parse the command line and the option file
		//if (argc < ParameterType::NB_PARAM)
		//{
		//	showUsage(argv[0]);
		//}
		//else
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

void runMinimalTransversals(const std::string& file, bool useCloneOptimization, bool verbose, bool useOutputFile, bool useOutputLogFile, const std::string& outputLogFile)
{
	//std::cout << BOLDYELLOW << "\n***** Running MT Miner *****" << RESET << std::endl << std::endl;
	
	unsigned int objectCount = 0;
	unsigned int itemCount = 0;
	
	Logger::init(outputLogFile, verbose, useOutputLogFile);

	// parser file
	HyperGraph hypergraph;
	if (hypergraph.load(file))
	{
		// allocate miner 
		MT_Miner miner(useCloneOptimization);
		// load hypergraph into formal context, then into binary representation
		// build clone list if needed
		miner.createBinaryRepresentation(hypergraph);

		// build formal context from hypergraph
		FormalContext formalContext(hypergraph);
		//formalContext.serialize("format_context.csv");

		// build binary representation from formal context
		BinaryRepresentation::buildFromFormalContext(formalContext);
		//BinaryRepresentation<bitset_type>::serialize("binary_rep.csv");
		
		// compute minimal transverses
		std::vector<Itemset*> minimalTransversals;
		miner.computeMinimalTransversals(minimalTransversals);

		// save minimal transversals into a file
		if (useOutputFile)
		{
			std::string outFile = file;
			outFile += ".out";

			Logger::log("saving minimal transversals into file : ", outFile, "\n");
			std::ofstream outputStream;
			outputStream.open(outFile);
			for_each(minimalTransversals.begin(), minimalTransversals.end(), [&](const Itemset* elt) { outputStream << elt->toString() << std::endl; });
			outputStream.close();			
		}

		for (auto it = minimalTransversals.begin(); it != minimalTransversals.end(); it++) { delete *it; }
		minimalTransversals.clear();
	}
	Logger::close();	
}


// ----------------------------------------------------------------------------------------------------------- //
// ----------------------------------------------------------------------------------------------------------- //

int main(int argc, char* argv[])
 {
	/*Logger::init("", true, "");

	PROCESS_MEMORY_COUNTERS_EX pmc;
	GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
	SIZE_T physMemUsedByMe0 = pmc.WorkingSetSize;

	{
		HyperGraph hypergrah;
		bool parserResult = hypergrah.load("../data/dualmatching34.dat");

		// build formal context from hypergraph
		FormalContext formalContext(hypergrah);

		// build binary representation from formal context
		BinaryRepresentation::buildFromFormalContext(formalContext);

		unsigned int cloneListSize = BinaryRepresentation::buildCloneList();
		Logger::log(cloneListSize, " clones found\n");

	}

	//std::shared_ptr<Itemset> tmp = std::make_shared<Itemset>();
	//tmp->addFirstItem()

	GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
	SIZE_T physMemUsedByMe1 = pmc.WorkingSetSize;

	std::cout << "allocated memory " << physMemUsedByMe1 - physMemUsedByMe0 << std::endl;

	Logger::close();

	return 0;*/


	// --------------------------------------------------------------------------------------------------------- //
	

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

	if(parameterList.find(ArgumentParser::VERBOSE_MODE) != parameterList.end())
		verboseMode = parameterList[ArgumentParser::VERBOSE_MODE] == "true" || parameterList[ArgumentParser::VERBOSE_MODE] == "True" || parameterList[ArgumentParser::VERBOSE_MODE] == "TRUE";
	if (parameterList.find(ArgumentParser::LOG_TO_FILE) != parameterList.end())
		useOutputLog = parameterList[ArgumentParser::LOG_TO_FILE] == "true" || parameterList[ArgumentParser::LOG_TO_FILE] == "True" || parameterList[ArgumentParser::LOG_TO_FILE] == "TRUE";
	if (parameterList.find(ArgumentParser::FILENAME_OUTPUT_LOG) != parameterList.end())
		useOutputLogFile = parameterList[ArgumentParser::FILENAME_OUTPUT_LOG];
	if (parameterList.find(ArgumentParser::OUTPUT_TO_FILE) != parameterList.end())
		useOutputFile = parameterList[ArgumentParser::OUTPUT_TO_FILE] == "true" || parameterList[ArgumentParser::OUTPUT_TO_FILE] == "True" || parameterList[ArgumentParser::OUTPUT_TO_FILE] == "TRUE";
	if (parameterList.find(ArgumentParser::USE_CLONE) != parameterList.end())
		useCloneOptimization = parameterList[ArgumentParser::USE_CLONE] == "true" || parameterList[ArgumentParser::USE_CLONE] == "True" || parameterList[ArgumentParser::USE_CLONE] == "TRUE";

	runMinimalTransversals(file, useCloneOptimization, verboseMode, useOutputFile, useOutputLog, useOutputLogFile);

	return 0;	
}