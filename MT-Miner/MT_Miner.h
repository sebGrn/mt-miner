#pragma once
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <cassert>

class MT_Miner
{
private:
	// number of lines
	unsigned int itemCount;
	// number of columns
	unsigned int objectCount;
	// binary representation 
	//  - TODO: we should use a dynamic bitset here to optimise & operation
	//  - TODO: not sure if a map is usefull here
	std::map<int, std::vector<bool>> binaryRep;

	bool verbose;

private:
	bool isEss(const std::string& pattern);
	bool checkOneItem(int itemBar, const std::vector<unsigned int>& itemsOfpattern);

public:
	MT_Miner(bool verbose = true);
	~MT_Miner();

	/// initialize the minimal transversals miner
	/// this function build a format context from the hypergraph, then build the binary representation
	void init(unsigned int itemCount, unsigned int objectCount, const std::vector<std::vector<unsigned int>>& hypergraph);
	
	///
	unsigned int computeDisjonctifSupport(const std::string& pattern);

	///
	void computeMinimalTransversals(std::vector<std::string>& toTraverse, std::vector<std::string>& mt);	
};

