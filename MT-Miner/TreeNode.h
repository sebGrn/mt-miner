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
	static std::atomic_uint cpt;

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

	// used to lock tasks before being put in the task queue (in order to not have a huge task queue)
	static std::mutex memory_guard;
	// used to pause / unpause thread
	static std::condition_variable memory_signal;

	// blocked tasks, waiting to bepending_memory_task_count unlock, memory is locked
	static std::atomic_uint pending_memory_task_count;

	// threshold who can vary
	static std::atomic_uint max_pending_task_count;
	// if true, a thread is checking if all tasks are pending to adapt inc/dec max_pending_task_count
	static std::atomic_bool pending_task_checker;

	static std::atomic_bool only_minimal;

	static std::atomic_uint threshold;


	// binary representation from the transactional base
	static std::shared_ptr<BinaryRepresentation> binaryRepresentation;

	/// true if we want to use clone optimization
	/// a clone is an item from binary representation 
	bool useCloneOptimization;

	// shared memory between threads, contains computed minimaltransverse
	std::vector<std::shared_ptr<Itemset>> minimalTransverse;

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
	void generateCandidates(std::vector<std::shared_ptr<Itemset>>&& toTraverse, std::vector<std::shared_ptr<Itemset>>&& toExplore, unsigned int& toExplore_MaxClique_Index);
	
	void addTaskIntoQueue(std::vector<std::shared_ptr<Itemset>>&& toTraverse);

	void computeMinimalTransversals_task(std::vector<std::shared_ptr<Itemset>>&& toExplore, unsigned int toExplore_MaxClique_Index);
	
	void recurseOnClonedItemset(std::shared_ptr<Itemset> itemset, unsigned int iItem);

public:
	TreeNode(bool useCloneOptimization, bool only_minimal, float threshold);
	~TreeNode();

	std::vector<std::shared_ptr<Itemset>> computeMinimalTransversals(std::vector<std::shared_ptr<Itemset>>&& toTraverse);
};

