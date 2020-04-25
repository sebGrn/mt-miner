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
#include "TreeNode.h"

class MT_Miner
{
private:
	///
	bool useCloneOptimization;
	// binary representation 
	std::shared_ptr<BinaryRepresentation> binaryRepresentation;
		
public:
	MT_Miner(bool useCloneOptimization);
	~MT_Miner();

	/// initialize the minimal transversals miner
	/// this function build a format context from the hypergraph, then build the binary representation
	void init(const std::shared_ptr<HyperGraph>& hypergraph);

	///
	std::vector<Utils::Itemset> computeMinimalTransversals(const std::vector<Utils::Itemset>& toTraverse) const;
};

