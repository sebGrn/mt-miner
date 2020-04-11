#pragma once
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <cassert>

#include "FormalContext.h"
#include "HyperGraph.h"
#include "BinaryRepresentation.h"

class MT_Miner
{
	using Bitset = boost::dynamic_bitset<>;
	using Itemset = std::vector<unsigned int>;

private:
	// number of lines
	unsigned int itemCount;
	// number of columns
	unsigned int objectCount;
	// binary representation 
	//  - TODO: we should use a dynamic bitset here to optimise & operation
	//  - TODO: not sure if a map is usefull here
	//std::map<int, std::vector<bool>> binaryRep;
	std::unique_ptr<BinaryRepresentation> binaryRepresentation;

	bool verbose;

private:
	Itemset combineItemset(const Itemset& str1, const Itemset& str2) const;
	bool isEssential(const Itemset& pattern) const;
	bool checkOneItem(int itemBar, const Itemset& itemsOfpattern) const;

public:
	MT_Miner(bool verbose = true);
	~MT_Miner();

	/// initialize the minimal transversals miner
	/// this function build a format context from the hypergraph, then build the binary representation
	void init(const std::shared_ptr<HyperGraph>& hypergraph);
	
	///
	unsigned int computeDisjonctifSupport(const Itemset& pattern) const;

	///
	void computeMinimalTransversals(std::vector<Itemset>& toTraverse, std::vector<Itemset>& mt) const;
};

