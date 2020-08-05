#pragma once
#include <vector>
#include <map>
#include <deque>
#include <string>
#include <algorithm>
#include <mutex>
#include <thread>
#include <mutex>
#include <atomic>
#include <future>
#include <numeric>
#include <list>
#include <memory>

#include "utils.h"
#include "BinaryRepresentation.h"

/**
 * Minimal tranversals are found while running through a tree which is dynamically build
 */
class TreeNode
{
private:
	// to avoid interleaved outputs
	static std::mutex output_guard;

	// synchro stuff
	// https://www.youtube.com/watch?v=2Xad9bCYbJE&list=PL1835A90FC78FF8BE&index=6
	static std::deque<std::future<std::vector<Itemset>>> task_queue;
	static std::mutex task_guard;
	static std::condition_variable task_signal;

	// egual à la taille des tâches arrêtées
	static int pending_task_count;
	
	//static std::shared_ptr<BinaryRepresentation<CustomULBitset>> binaryRepresentation;
	static std::shared_ptr<BinaryRepresentation> binaryRepresentation;

	/// true if we want to use clone optimization
	/// a clone is an item from binary representation 
	bool useCloneOptimization;

public:
	static std::atomic_ullong nbTotalChildren;

private: 
	void buildClonedCombination(const Itemset& currentItem, std::vector<Itemset>& clonedCombination, const std::vector<std::pair<unsigned int, unsigned int>>& originalClonedIndexes);

	/// compute maxClique list, toExplore list and mt list
	/// update graph_mt with new minimal transversal itemset
	void updateListsFromToTraverse(const std::vector<Itemset>& toTraverse, std::vector<Itemset>& maxClique, std::vector<Itemset>& toExplore, std::vector<Itemset>& graph_mt);

	//void exploreNextBranch(const ItemsetList& maxClique, const ItemsetList& toExplore, ItemsetList& graph_mt);

	std::vector<Itemset> computeMinimalTransversals_task(const std::vector<Itemset>& toTraverse);
	
public:
	TreeNode(bool useCloneOptimization);
	~TreeNode();

	/// recursive method, going through tree representation 
	/// computes minimal transversals for binary representation
	//std::vector<Itemset> computeMinimalTransversals_recursive(const std::vector<Itemset> & toTraverse);

	/// iterative method, for debug purpose only
	/// computes minimal transversals for binary representation
	//std::vector<Itemset> computeMinimalTransversals_iterative(const std::vector<Itemset>& toTraverse);

	std::vector<Itemset> computeMinimalTransversals(std::vector<Itemset>& toTraverse);
};

