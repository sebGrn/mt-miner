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
	std::vector<std::future<ItemsetList>> futures;

private:
	void buildClonedCombinaison(const Itemset& currentItem, ItemsetList& clonedCombination, const std::vector<std::pair<unsigned int, unsigned int>>& originalClonedIndexes);

	/// compute maxClique list, toExplore list and mt list
	/// update graph_mt with new minimal transversal itemset
	void updateListsFromToTraverse(const ItemsetList& toTraverse, ItemsetList& maxClique, ItemsetList& toExplore, ItemsetList& graph_mt);

	void exploreNextBranch(const ItemsetList& maxClique, const ItemsetList& toExplore, ItemsetList& graph_mt);
	
public:
	TreeNode(bool useCloneOptimization, const std::shared_ptr<BinaryRepresentation>& binaryRepresentation);
	~TreeNode();

	/// recursive function, trasvere treen node to compute minimal transversals for binary representation
	/// @param toTraverse transversal itemset list
	std::vector<Itemset> computeMinimalTransversals_recursive(const std::vector<Itemset> & toTraverse);

	std::vector<Itemset> computeMinimalTransversals_iterative(const std::vector<Itemset>& toTraverse);

	unsigned long long getTotalChildren() const
	{
		return nbTotalChildren;
	};
};

