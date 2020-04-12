#pragma once
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <cassert>

#include "utils.h"
#include "Bitset.h"
#include "FormalContext.h"
#include "HyperGraph.h"
#include "BinaryRepresentation.h"

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
	//std::map<int, std::vector<bool>> binaryRep;
	std::unique_ptr<BinaryRepresentation> binaryRepresentation;

	/// contains the list of column indexes which have the same disjoncif support 
	/// these are clones
	//std::vector<unsigned int> cloneMap;

	bool verbose;

private:
	Utils::Itemset combineItemset(const Utils::Itemset& str1, const Utils::Itemset& str2) const;
	bool isEssential(const Utils::Itemset& pattern) const;
	bool checkOneItem(int itemBar, const Utils::Itemset& itemsOfpattern) const;
	//bool checkClone(unsigned int index);

public:
	MT_Miner(bool verbose = true);
	~MT_Miner();

	/// initialize the minimal transversals miner
	/// this function build a format context from the hypergraph, then build the binary representation
	void init(const std::shared_ptr<HyperGraph>& hypergraph);
	
	///
	unsigned int computeDisjonctifSupport(const Utils::Itemset& pattern) const;

	///
	std::vector<Utils::Itemset> computeMinimalTransversals(std::vector<Utils::Itemset>& toTraverse) const;
};

