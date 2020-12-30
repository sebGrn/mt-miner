#pragma once
#include <vector>
#include <map>
#include <deque>
#include <string>
#include <algorithm>
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
	static std::deque<std::future<void>> task_queue;
	static std::mutex task_guard;
	static std::condition_variable task_signal;

	//static std::condition_variable_any memory_signal;

	// egal à la taille des tâches arrêtées
	static std::atomic_uint pending_task_count;
	
	static std::shared_ptr<BinaryRepresentation> binaryRepresentation;

	/// true if we want to use clone optimization
	/// a clone is an item from binary representation 
	bool useCloneOptimization;

	// shared memory between threads, contains computed minimaltransverse
	std::vector<std::shared_ptr<Itemset>> minimalTransverse;

public:
	static std::atomic_ullong nbTasks;
	static std::atomic_ullong nbTotalMt;
	static std::atomic_ullong minimalMt;

private: 
	/// compute maxClique list, toExplore list and mt list
	/// update graph_mt with new minimal transversal itemset
	void updateListsFromToTraverse(std::vector<std::shared_ptr<Itemset>>&& toTraverse, std::deque<std::shared_ptr<Itemset>>&& maxClique, std::deque< std::shared_ptr<Itemset>>&& toExplore);
	void createNewProcess(std::vector<std::shared_ptr<Itemset>>&& toTraverse);

	void computeMinimalTransversals_task(std::vector<std::shared_ptr<Itemset>>&& toTraverse);
	
	void recurseOnClonedItemset(std::shared_ptr<Itemset> itemset, unsigned int iItem);

public:
	TreeNode(bool useCloneOptimization);
	~TreeNode();

	std::vector<std::shared_ptr<Itemset>> computeMinimalTransversals(std::vector<std::shared_ptr<Itemset>>&& toTraverse);
};

