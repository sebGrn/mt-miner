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

#include "utils.h"
#include "HypergraphParser.h"

using Intset = std::vector<unsigned int>;

///
/*
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
*/
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
	std::vector<Itemset> toTraverse;
	for (unsigned int i = 1; i <= parser.getItemCount(); i++)
	{
		Itemset v(1);
		v[0] = i;
		toTraverse.push_back(v);
	}

	// call mt_miner and compute minimal transverse
	std::vector<Intset> minimalTransversals;
	MT_Miner miner(false);
	miner.init(parser.getHypergraph());
	miner.computeMinimalTransversals(toTraverse, minimalTransversals);
	// sort results
	minimalTransversals = sortVectorOfItemset(minimalTransversals);

	// print minimal transversals	
	// print minimal transversals	
	std::cout << std::endl;
	std::cout << "minimal transversals count : " << minimalTransversals.size() << std::endl;
	if (minimalTransversals.size() > 5)
		for_each(minimalTransversals.begin(), minimalTransversals.begin() + 5, [&](const Itemset& elt) { std::cout << itemsetToString(elt) << std::endl; });
	else
		for_each(minimalTransversals.begin(), minimalTransversals.end(), [&](const Itemset& elt) { std::cout << itemsetToString(elt) << std::endl; });
	std::cout << std::endl;


	std::vector<Itemset> mt_resuls;
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
			{
				Itemset data = splitToVectorOfInt(line, ' ');
				mt_resuls.push_back(data);
			}
		}
		inputFile.close();
	}

	// sort results before comparing
	mt_resuls = sortVectorOfItemset(mt_resuls);

	// check with our results
	for_each(minimalTransversals.begin(), minimalTransversals.end(), [&](const Itemset& item) {
		auto it = std::find_if(mt_resuls.begin(), mt_resuls.end(), compare_itemset(item));
		if (it == mt_resuls.end())
		{
			std::cout << itemsetToString(item) << " from our computed transverals list has not been found in " << res_file << std::endl;
			notGood = true;
		}
		});

	std::cout << std::endl;

	for_each(mt_resuls.begin(), mt_resuls.end(), [&](const Itemset& item) {
		auto it = std::find_if(minimalTransversals.begin(), minimalTransversals.end(), compare_itemset(item));
		if (it == minimalTransversals.end())
		{
			std::cout << itemsetToString(item) << "from " << res_file << " has not been found in our computed transverals list" << std::endl;
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
	if (argc <= 1)
	{
		std::cout << "Usage " << argv[0] << "<filename> <option><outputfile>" << std::endl;
		return 0;
	}

	std::string file = argv[1];
	
	std::string resfile;
	if (argc > 2)
		resfile = argv[2];

	// performs tests
	//unitaryTesting();
	//compareResults(file, resfile);

	std::cout << "computing minimal transversals on file " << file << std::endl;
	
	unsigned int objectCount = 0;
	unsigned int itemCount = 0;
	std::shared_ptr<HyperGraph> hypergraph;

	// parsing
	auto beginTime = std::chrono::system_clock::now();
	{
		HypergraphParser parser;
		parser.parse(file);
		// get data from parser
		hypergraph = parser.getHypergraph();
		objectCount = parser.getObjectCount();
		itemCount = parser.getItemCount();
	}
	int64_t duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - beginTime).count();
	std::cout << "parsing hypergraph done in " << duration << " ms" << std::endl;
	std::cout << "itemCount " << itemCount << std::endl;
	std::cout << "objectCount " << objectCount << std::endl;
	std::cout << std::endl;
	
	// minimal transversals computing
	std::vector<Itemset> minimalTransversals;
	beginTime = std::chrono::system_clock::now();
	{			
		MT_Miner miner(false);
		miner.init(hypergraph);		

		std::vector<Itemset> toTraverse;
		for (unsigned int i = 1; i <= itemCount; i++)
		{
			Itemset v(1);
			v[0] = i;
			toTraverse.push_back(v);
		}

		// compute minimal transversals		
		miner.computeMinimalTransversals(toTraverse, minimalTransversals);
	}
	duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - beginTime).count();
	std::cout << "minimal transversals done in " << duration << " ms" << std::endl;

	// sort transversals computing
	//minimalTransversals = sortVectorOfItemset(minimalTransversals);

	// print minimal transversals	
	std::cout << std::endl;
	std::cout << "minimal transversals count : " << minimalTransversals.size() << std::endl;
	//if(minimalTransversals.size() > 5)
	//	for_each(minimalTransversals.begin(), minimalTransversals.begin() + 5, [&](const Itemset& elt) { std::cout << itemsetToString(elt) << std::endl; });
	//else
	//	for_each(minimalTransversals.begin(), minimalTransversals.end(), [&](const Itemset& elt) { std::cout << itemsetToString(elt) << std::endl; });
	//std::cout << "..." << std::endl << std::endl;

	// ----------------------------------------------------- //

	// save minimal transversals into a file
	if (!resfile.empty())
	{
		std::cout << "saving minimal transversals into file : " << resfile << std::endl;
		std::ofstream outputStream;
		outputStream.open(resfile);
		for_each(minimalTransversals.begin(), minimalTransversals.end(), [&](const Itemset& elt) { outputStream << itemsetToString(elt) << std::endl; });
		outputStream.close();
	}
}

