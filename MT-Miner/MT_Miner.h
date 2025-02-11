#pragma once
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <cassert>
#include <omp.h>
#include <chrono>

#include "utils.h"
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
	bool useMinimalSizeOnly;
	float threshold;
	
private:
	/// return the minimal transversals miner
	void computeInitialToTraverseList(std::deque<std::shared_ptr<Itemset>>& toTraverse) const;

public:
	MT_Miner(bool useCloneOptimization = true, bool useMinimalSizeOnly = false, float threshold = 1.0f);
	~MT_Miner();

	/// this function build a format context from the hypergraph, then build the binary representation
	void createBinaryRepresentation(const HyperGraph& hypergraph);
	
	///
	void computeMinimalTransversals(std::deque<std::shared_ptr<Itemset>>& mt);
};

