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
	static std::deque<std::future<std::vector<std::shared_ptr<Itemset>>>> task_queue;
	static std::mutex task_guard;
	static std::condition_variable task_signal;

	// egal à la taille des tâches arrêtées
	static int pending_task_count;
	
	static std::shared_ptr<BinaryRepresentation> binaryRepresentation;

	/// true if we want to use clone optimization
	/// a clone is an item from binary representation 
	bool useCloneOptimization;

public:
	static std::atomic_ullong nbTotalChildren;

private: 
	/// compute maxClique list, toExplore list and mt list
	/// update graph_mt with new minimal transversal itemset
	void updateListsFromToTraverse(const std::vector<std::shared_ptr<Itemset>>& toTraverse,
										 std::vector<std::shared_ptr<Itemset>>& maxClique,
										 std::vector<std::shared_ptr<Itemset>>& toExplore,
										 std::vector<std::shared_ptr<Itemset>>& graph_mt);

	std::vector<std::shared_ptr<Itemset>> computeMinimalTransversals_task(const std::vector<std::shared_ptr<Itemset>>& toTraverse);
	
public:
	TreeNode(bool useCloneOptimization);
	~TreeNode();

	std::vector<std::shared_ptr<Itemset>> computeMinimalTransversals(std::vector<std::shared_ptr<Itemset>>& toTraverse);
};

