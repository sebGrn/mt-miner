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
	static std::mutex print_guard;
	//static std::mutex shared_toTraverseItemset_guard;
	static std::mutex shared_minimalTransverse_guard;

	// synchro stuff
	// https://www.youtube.com/watch?v=2Xad9bCYbJE&list=PL1835A90FC78FF8BE&index=6
	static std::deque<std::future<void>> task_queue;
	static std::mutex task_guard;
	static std::condition_variable task_signal;

	// egal à la taille des tâches arrêtées
	static int pending_task_count;
	
	static std::shared_ptr<BinaryRepresentation> binaryRepresentation;

	/// true if we want to use clone optimization
	/// a clone is an item from binary representation 
	bool useCloneOptimization;

	//std::vector<Itemset*> shared_toTraverse_itemset;
	std::vector<Itemset*> shared_minimalTransverse;

public:
	static std::atomic_ullong nbTotalChildren;
	static std::atomic_ullong nbTotalMt;
	static std::atomic_ullong minimalMt;
	static std::atomic_uint nbThread;
	static std::atomic_uint cptSharedItemsets;

private: 
	/// compute maxClique list, toExplore list and mt list
	/// update graph_mt with new minimal transversal itemset
	//void updateListsFromToTraverse( const std::vector<Itemset*>& toTraverse, std::vector<Itemset*>& maxClique, std::vector<Itemset*>& toExplore, std::vector<Itemset*>& graph_mt);
	void updateListsFromToTraverse(const std::vector<Itemset*>& toTraverse, std::vector<Itemset*>& maxClique, std::vector<Itemset*>& toExplore);

	//std::vector<Itemset*> computeMinimalTransversals_task(const std::vector<Itemset*>& toTraverse);
	void computeMinimalTransversals_task_test(const std::vector<Itemset*>& toTraverse);
	
	void recurseOnClonedItemset(Itemset* itemset, unsigned int iItem);

public:
	TreeNode(bool useCloneOptimization);
	~TreeNode();

	void computeMinimalTransversals(std::vector<Itemset*>& graph_mt, std::vector<Itemset*>& toTraverse);
};

