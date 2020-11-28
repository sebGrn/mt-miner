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
	
private:
	/// return the minimal transversals miner
	void computeInitalToTraverseList(std::vector<Itemset*>& toTraverse) const;

public:
	MT_Miner(bool useCloneOptimization = true);
	~MT_Miner();

	/// this function build a format context from the hypergraph, then build the binary representation
	void createBinaryRepresentation(const HyperGraph& hypergraph);
	
	///
	void computeMinimalTransversals(std::vector<Itemset*>& mt);
};

