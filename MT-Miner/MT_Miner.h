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
#include "Graph.h"

class MT_Miner
{
private:
	// number of lines
	//unsigned int itemCount;
	// number of columns
	//unsigned int objectCount;
	// binary representation 
	std::shared_ptr<BinaryRepresentation> binaryRepresentation;

	/// contains the list of column indexes which have the same disjoncif support 
	/// these are clones
	//std::vector<unsigned int> cloneMap;

	bool verbose;

public:
	MT_Miner(bool verbose = true);
	~MT_Miner();

	/// initialize the minimal transversals miner
	/// this function build a format context from the hypergraph, then build the binary representation
	void init(const std::shared_ptr<HyperGraph>& hypergraph);

	///
	std::vector<Utils::Itemset> computeMinimalTransversals(const std::vector<Utils::Itemset>& toTraverse) const;
};

