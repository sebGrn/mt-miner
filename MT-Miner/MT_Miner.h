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
	//  - note1 : we should use a dynamic bitset here to optimise & operation
	//  - note2 : not sure if a map is usefull here
	std::map<int, std::vector<bool>> binaryRep;

	bool verbose;

public:
	unsigned int computeDisjonctifSupport(const std::string& pattern);

public:
	MT_Miner();
	~MT_Miner();

	void init(unsigned int itemCount, unsigned int objectCount, const std::vector<std::vector<unsigned int>>& hypergraph);

	void computeMinimalTransversals(std::vector<std::string>& toTraverse, std::vector<std::string>& mt);
	
};

