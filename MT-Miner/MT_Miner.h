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
	static std::atomic_bool stop;
	///
	bool useCloneOptimization;
	// binary representation 
	std::shared_ptr<BinaryRepresentation_impl> binaryRepresentation;
	
private:
	/// this function build a format context from the hypergraph, then build the binary representation
	void createBinaryRepresentation(const std::shared_ptr<HyperGraph>& hypergraph);

	/// return the minimal transversals miner
	ItemsetList computeInitalToTraverseList();

public:
	MT_Miner(const std::shared_ptr<HyperGraph>& hypergraph, bool useCloneOptimization = true);
	~MT_Miner();

	///
	ItemsetList computeMinimalTransversals();

	///
	unsigned int computeDisjonctifSupport(const Itemset& pattern) const
	{
		return binaryRepresentation->computeDisjonctifSupport(pattern);
	}
};

