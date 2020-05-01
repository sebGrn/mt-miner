#pragma once
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <mutex>
#include <thread>
#include <mutex>
#include <atomic>
#include <future>

#include "utils.h"
#include "BinaryRepresentation.h"

class TreeNode
{
private:
	static std::atomic_int processorCount;
	static std::atomic_ullong nbTotalChildren;
	static std::atomic_int nbRunningThread;

	std::shared_ptr<BinaryRepresentation> binaryRepresentation;

	/// true if we want to use clone optimization
	/// a clone is a item from binary representation 
	bool useCloneOptimization;

	bool useMultitheadOptimization;

	std::vector<std::shared_ptr<TreeNode>> children;

	// https://www.youtube.com/watch?v=2Xad9bCYbJE&list=PL1835A90FC78FF8BE&index=6
	std::vector<std::future<std::vector<Utils::Itemset>>> futures;

private:
	void buildClonedCombinaison(const Utils::Itemset& currentItem, std::vector<Utils::Itemset>& clonedCombination, const std::vector<std::pair<unsigned int, unsigned int>>& originalClonedIndexes);

	/// compute maxClique list, toExplore list and mt list
	/// update graph_mt with new minimal transversal itemset
	void updateListsFromToTraverse(const std::vector<Utils::Itemset>& toTraverse, std::vector<Utils::Itemset>& maxClique, std::vector<Utils::Itemset>& toExplore, std::vector<Utils::Itemset>& graph_mt);

	void exploreNextBranch(const std::vector<Utils::Itemset>& toTraverse, const std::vector<Utils::Itemset>& maxClique, const std::vector<Utils::Itemset>& toExplore, std::vector<Utils::Itemset>& graph_mt);
	
public:
	TreeNode(bool useCloneOptimization, const std::shared_ptr<BinaryRepresentation>& binaryRepresentation);
	~TreeNode();

	/// recursive function, trasvere treen node to compute minimal transversals for binary representation
	/// @param toTraverse transversal itemset list
	std::vector<Utils::Itemset> computeMinimalTransversals(const std::vector<Utils::Itemset> & toTraverse);

	unsigned long long getTotalChildren() const
	{
		return nbTotalChildren;
	};
};

