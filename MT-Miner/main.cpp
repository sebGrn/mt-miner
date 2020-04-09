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

#include "utils.h"
#include "HypergraphParser.h"

///
void unitaryTesting()
{
	// parse file
	HypergraphParser parser;
	parser.parse("data/test.txt");

	std::vector<std::vector<unsigned int>> hypergraph = parser.getHypergraph();
	unsigned int objectCount = parser.getObjectCount();
	unsigned int itemCount = parser.getItemCount();
	std::cout << "itemCount " << itemCount << std::endl;
	std::cout << "objectCount " << objectCount << std::endl;
	assert(objectCount == 6);
	assert(itemCount == 8);

	// allocate miner
	MT_Miner miner;
	miner.init(itemCount, objectCount, hypergraph);

	unsigned int disjonctifSupport = miner.computeDisjonctifSupport("1");
	std::cout << "disjonctifSupport(V1) " << disjonctifSupport << std::endl;
	assert(disjonctifSupport == 1);

	disjonctifSupport = miner.computeDisjonctifSupport("1 2");
	std::cout << "disjonctifSupport(V12) " << disjonctifSupport << std::endl;
	assert(disjonctifSupport == 2);

	disjonctifSupport = miner.computeDisjonctifSupport("1 2 3");
	std::cout << "disjonctifSupport(V123) " << disjonctifSupport << std::endl;
	assert(disjonctifSupport == 3);

	disjonctifSupport = miner.computeDisjonctifSupport("1 2 3 4");
	std::cout << "disjonctifSupport(V1234) " << disjonctifSupport << std::endl;
	assert(disjonctifSupport == 4);

	std::cout << "----------------------------------------------------------" << std::endl;
	std::cout << "computing minimal transversals  ..." << std::endl;

	std::vector<std::string> toTraverse;
	for (unsigned int i = 1; i <= itemCount; i++)
		toTraverse.push_back(std::to_string(i));

	std::vector<std::string> correctToTraverse = { "1", "2", "3", "4", "5", "6", "7", "8" };
	assert(correctToTraverse == toTraverse);

	// compute minimal transversals
	std::vector<std::string> minimalTransversals;
	miner.computeMinimalTransversals(toTraverse, minimalTransversals);
	minimalTransversals = sortVectorOfString(minimalTransversals);

	// print minimal transversals	
	std::cout << "show minimal transversals" << std::endl;
	for_each(minimalTransversals.begin(), minimalTransversals.end(), [&](const std::string& elt) {
		std::cout << "{" << elt << "}" << std::endl;
		});
	std::cout << std::endl;
}

/// Compare results between input and attended results
bool compareResults(const std::string& input_file, const std::string& res_file)
{
	bool notGood = false;
	std::cout << "----------------------------------------------------------" << std::endl;
	std::cout << "comparing results between " << input_file << " and " << res_file << std::endl;

	// parse file and store hypergraph
	HypergraphParser parser;
	parser.parse(input_file);

	// make toTraverseList
	std::vector<std::string> toTraverse;
	for (unsigned int i = 1; i <= parser.getItemCount(); i++)
		toTraverse.push_back(std::to_string(i));

	// call mt_miner and compute minimal transverse
	std::vector<std::string> minimalTransversals;
	MT_Miner miner(false);
	miner.init(parser.getItemCount(), parser.getObjectCount(), parser.getHypergraph());
	miner.computeMinimalTransversals(toTraverse, minimalTransversals);
	// sort results
	minimalTransversals = sortVectorOfString(minimalTransversals);

	// print minimal transversals	
	std::cout << std::endl;
	std::cout << "minimal transversals count : " << minimalTransversals.size() << std::endl;
	for_each(minimalTransversals.begin(), minimalTransversals.end(), [&](const std::string& elt) { std::cout << "{" << elt << "}" << std::endl; });
	std::cout << std::endl;

	std::vector<std::string> mt_resuls;
	std::ifstream inputFile = std::ifstream();
	inputFile.open(res_file);
	if (!inputFile.fail())
	{
		// Parse the file line by line
		while (!inputFile.eof())
		{
			// read and clean line from file
			std::string line;
			getline(inputFile, line);
			if (!line.empty())
				mt_resuls.push_back(line);
		}
		inputFile.close();
	}

	// sort results before comparing
	mt_resuls = sortVectorOfString(mt_resuls);

	// check with our results
	for_each(minimalTransversals.begin(), minimalTransversals.end(), [&](const std::string& str) {
		auto it = std::find_if(mt_resuls.begin(), mt_resuls.end(), compare_str(str));
		if (it == mt_resuls.end())
		{
			std::cout << "{" << str << "} from our computed transverals list has not been found in " << res_file << std::endl;
			notGood = true;
		}
		});

	std::cout << std::endl;

	for_each(mt_resuls.begin(), mt_resuls.end(), [&](const std::string& str) {
		auto it = std::find_if(minimalTransversals.begin(), minimalTransversals.end(), compare_str(str));
		if (it == minimalTransversals.end())
		{
			std::cout << "{" << str << "} from " << res_file << " has not been found in our computed transverals list" << std::endl;
			notGood = true;
		}
		});

	if (!notGood)
	{
		std::cout << "!!! results are the same !!!" << std::endl;
	}
	std::cout << "----------------------------------------------------------" << std::endl;

	return !notGood;
}

// ----------------------------------------------------------------------------------------------------------- //
// ----------------------------------------------------------------------------------------------------------- //

int main(int argc, char* argv[])
{
	if (argc <= 2)
	{
		std::cout << "Usage " << argv[0] << "<filename> <resultfiles>" << std::endl;
		return 0;
	}

	std::string file = argv[1];
	std::string resfile = argv[2];

	// performs tests
	//unitaryTesting();
	compareResults(file, resfile);
	/*
	// parse file
	HypergraphParser parser;
	parser.parse(file);
	// get data from parser
	std::vector<std::vector<unsigned int>> hypergraph = parser.getHypergraph();
	unsigned int objectCount = parser.getObjectCount();
	unsigned int itemCount = parser.getItemCount();
	std::cout << "itemCount " << itemCount << std::endl;
	std::cout << "objectCount " << objectCount << std::endl;

	// allocate miner
	MT_Miner miner;
	miner.init(itemCount, objectCount, hypergraph);

	std::cout << std::endl;
	std::cout << "computing minimal transversals  ..." << std::endl;

	std::vector<std::string> toTraverse;
	for (unsigned int i = 1; i <= itemCount; i++)
		toTraverse.push_back(std::to_string(i));

	// compute minimal transversals
	std::vector<std::string> minimalTransversals;
	miner.computeMinimalTransversals(toTraverse, minimalTransversals);
	minimalTransversals = sortVectorOfString(minimalTransversals);

	// print minimal transversals	
	std::cout << std::endl;
	std::cout << "minimal transversals count : " << minimalTransversals.size() << std::endl;
	for_each(minimalTransversals.begin(), minimalTransversals.end(), [&](const std::string& elt) { std::cout << "{" << elt << "}" << std::endl; });
	std::cout << std::endl;
	*/
}

