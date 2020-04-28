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
	void init(const std::shared_ptr<HyperGraph>& hypergraph, std::vector<Utils::Itemset>& toTraverse);

	///
	std::vector<Utils::Itemset> computeMinimalTransversals(const std::vector<Utils::Itemset>& toTraverse);

	///
	int64_t getIsEssentialDuration() const
	{
		return binaryRepresentation->getIsEssentialDuration();
	}

	///
	unsigned int computeDisjonctifSupport(const Utils::Itemset& pattern) const
	{
		return binaryRepresentation->computeDisjonctifSupport(pattern);
	}
};

