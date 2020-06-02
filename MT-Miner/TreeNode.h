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
#include <numeric>

#include "utils.h"
#include "BinaryRepresentation.h"

/**
 * Minimal tranversals are found while running through a tree which is dynamically build
 */
template <class T>
class TreeNode
{
private:
	// to avoid interleaved outputs
	static std::mutex output_guard;

	// synchro stuff
	static std::deque<std::future<void>> task_queue;
	static std::mutex task_guard;
	static std::condition_variable task_signal;
	static std::atomic_int pending_task_count;

	//static std::atomic_int processorCount;
	static std::atomic_ullong nbTotalChildren;
	//static std::atomic_int nbRunningThread;

	std::shared_ptr<BinaryRepresentation<T>> binaryRepresentation;

	/// true if we want to use clone optimization
	/// a clone is an item from binary representation 
	bool useCloneOptimization;

	bool useMultitheadOptimization;

	std::vector<std::shared_ptr<TreeNode>> children;

	// https://www.youtube.com/watch?v=2Xad9bCYbJE&list=PL1835A90FC78FF8BE&index=6
	//std::vector<std::future<ItemsetList>> futures;

private:
	void buildClonedCombination(const Itemset& currentItem, ItemsetList& clonedCombination, const std::vector<std::pair<unsigned int, unsigned int>>& originalClonedIndexes);

	/// compute maxClique list, toExplore list and mt list
	/// update graph_mt with new minimal transversal itemset
	void updateListsFromToTraverse(const ItemsetList& toTraverse, ItemsetList& maxClique, ItemsetList& toExplore, ItemsetList& graph_mt);

	//void exploreNextBranch(const ItemsetList& maxClique, const ItemsetList& toExplore, ItemsetList& graph_mt);

	void computeMinimalTransversals_task(const std::vector<Itemset>& toTraverse);
	
public:
	TreeNode(bool useCloneOptimization, const std::shared_ptr<BinaryRepresentation<T>>& binaryRepresentation);	
	~TreeNode();

	/// recursive method, going through tree representation 
	/// computes minimal transversals for binary representation
	//std::vector<Itemset> computeMinimalTransversals_recursive(const std::vector<Itemset> & toTraverse);

	/// iterative method, for debug purpose only
	/// computes minimal transversals for binary representation
	//std::vector<Itemset> computeMinimalTransversals_iterative(const std::vector<Itemset>& toTraverse);

	std::vector<Itemset> computeMinimalTransversals(const std::vector<Itemset>& toTraverse);
	
	unsigned long long getTotalChildren() const
	{
		return nbTotalChildren;
	};

	//unsigned int getTotalThread() const
	//{
	//	return nbRunningThread;
	//};
	
};

