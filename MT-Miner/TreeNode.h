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
public:
	static std::atomic_uint nbTaskCreated;

private:
	// to avoid interleaved outputs
	static std::mutex output_guard;

	// synchro stuff
	// https://www.youtube.com/watch?v=2Xad9bCYbJE&list=PL1835A90FC78FF8BE&index=6
	// task queue, contains all task to be processed by threads
	static std::deque<std::future<void>> task_queue;

	// used to lock tasks execution from task queue
	static std::mutex task_guard;
	static std::mutex trace_guard;
	// used to pause / unpause thread before tasks in process units
	static std::condition_variable task_signal;

	static std::atomic_bool only_minimal;

	static std::atomic_uint threshold;


	// binary representation from the transactional base
	static std::shared_ptr<BinaryRepresentation> binaryRepresentation;

	/// true if we want to use clone optimization
	/// a clone is an item from binary representation 
	bool useCloneOptimization;

	// shared memory between threads, contains computed minimaltransverse
	std::deque<std::shared_ptr<Itemset>> minimalTransverse;

	bool isMinimalTrasverse(const std::shared_ptr<Itemset>& itemset) const;
	void updateMinimalTraverseList(const std::shared_ptr<Itemset>& itemset);
	bool isCandidateForMaxClique(const Itemset& cumulatedItemset, const std::shared_ptr<Itemset>& itemset) const;

public:
	// stored tasks in the list, waiting to be managed, memory consuming
	static std::atomic_uint pending_task_count;
	//
	static std::atomic_ullong nbTotalMt;
	// minimal size of the minimal transverse
	static std::atomic_ullong minimalMt;

private: 
	/// compute maxClique list, toExplore list and mt list
	/// update graph_mt with new minimal transversal itemset
	void generateCandidates(std::deque<std::shared_ptr<Itemset>>&& toTraverse, std::deque<std::shared_ptr<Itemset>>&& toExplore, std::vector<std::vector<unsigned int>>&& maxClique);
	
	void addTaskIntoQueue(std::deque<std::shared_ptr<Itemset>>&& toTraverse);

	void computeMinimalTransversals_task(std::deque<std::shared_ptr<Itemset>>&& toExplore, std::vector<std::vector<unsigned int>>&& maxClique);
	
	void recurseOnClonedItemset(std::shared_ptr<Itemset> itemset, unsigned int iItem);

public:
	TreeNode(bool useCloneOptimization, bool only_minimal, float threshold);
	~TreeNode();

	std::deque<std::shared_ptr<Itemset>> computeMinimalTransversals(std::deque<std::shared_ptr<Itemset>>&& toTraverse);
};

