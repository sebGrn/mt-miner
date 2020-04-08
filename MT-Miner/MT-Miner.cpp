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

#include "utils.h"
#include "HypergraphParser.h"

int main(int argc, char* argv[])
{
	if (argc <= 1)
	{
		std::cout << "Usage " << argv[0] << "<filename> <option><resultfile>" << std::endl;
		return 0;
	}

	std::string file = argv[1];

	std::string resfile;
	if (argc >= 2)
		resfile = argv[2];

	// -------------------------------------------------------------------------------------------------------- //

	// parse file
	HypergraphParser parser;
	parser.parse(file);

	std::vector<std::vector<unsigned int>> hypergraph = parser.getHypergraph();
	unsigned int objectCount_new = parser.getObjectCount();
	unsigned int itemCount_new = parser.getItemCount();
	std::cout << "itemCount " << itemCount_new << std::endl;
	std::cout << "objectCount " << objectCount_new << std::endl;


	// allocate miner
	MT_Miner miner;
	miner.init(itemCount_new, objectCount_new, hypergraph);

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

	// initialize toTraverse list with { "1", "2", "3", "4", "5", "6", "7", "8" }
	std::vector<std::string> toTraverse;
	for (unsigned int i = 1; i <= itemCount_new; i++)
		toTraverse.push_back(std::to_string(i));

	// compute minimal transversals
	std::vector<std::string> minimalTransversals;
	miner.computeMinimalTransversals(toTraverse, minimalTransversals);
	minimalTransversals = sortVectorOfString(minimalTransversals);

	// print minimal transversals	
	std::cout << "show minimal transversals" << std::endl;
	for_each(minimalTransversals.begin(), minimalTransversals.end(), [&](const std::string& elt) {
		std::cout << "{" << elt << "}, ";
		});
	std::cout << std::endl;



	// -------------------------------------------------------------------------------------------------------- //
	/*
	std::cout << "----------------------------------------------------------" << std::endl;
	{
		std::vector<std::string> mt_resuls;
		std::ifstream inputFile = std::ifstream();
		inputFile.open(resfile);
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

		// check with our results
		for_each(minimalTransversals.begin(), minimalTransversals.end(), [&](const std::string& str) {
			auto it = std::find_if(mt_resuls.begin(), mt_resuls.end(), compare_str(str));
			if (it == mt_resuls.end())
			{
				std::cout << "{" << str << "} from our computed transverals list has not been found in " << resfile << std::endl;
			}
		});

		for_each(mt_resuls.begin(), mt_resuls.end(), [&](const std::string& str) {
			auto it = std::find_if(minimalTransversals.begin(), minimalTransversals.end(), compare_str(str));
			if (it == minimalTransversals.end())
			{
				std::cout << "{" << str << "} from " << resfile << " has not been found in our computed transverals list" << std::endl;
			}
			});
		std::cout << "----------------------------------------------------------" << std::endl;
	}
	*/
}
