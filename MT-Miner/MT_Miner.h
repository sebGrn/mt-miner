#pragma once
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <cassert>
#include <omp.h>
#include <chrono>

#include "utils.h"
#include "Bitset.h"
#include "FormalContext.h"
#include "HyperGraph.h"
#include "BinaryRepresentation.h"
#include "TreeNode.h"
#include "Logger.h"

/**
 * Minimal transversals miner
 * owns the binary represention
 * create the graphnode that will explore the hypergrapho 
 */
class MT_Miner
{
private:
	///
	bool computeMtDone;
	///
	bool useCloneOptimization;
	// binary representation 
	std::shared_ptr<BinaryRepresentation> binaryRepresentation;
		
public:
	MT_Miner(bool useCloneOptimization);
	~MT_Miner();

	/// initialize the minimal transversals miner
	/// this function build a format context from the hypergraph, then build the binary representation
	void init(const std::shared_ptr<HyperGraph>& hypergraph, ItemsetList& toTraverse, bool oneIndexedBase = true);

	///
	ItemsetList computeMinimalTransversals(const ItemsetList& toTraverse);

	///
	unsigned int computeDisjonctifSupport(const Itemset& pattern) const
	{
		return binaryRepresentation->computeDisjonctifSupport(pattern);
	}
};

