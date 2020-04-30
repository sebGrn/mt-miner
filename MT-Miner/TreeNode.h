#pragma once
#include <vector>
#include <map>
#include <string>
#include <algorithm>
#include <mutex>
#include <thread>
#include <mutex>
#include <atomic>

#include "utils.h"
#include "BinaryRepresentation.h"

class TreeNode
{
private:	
	int recursionLevel;
	static std::atomic_int nbTotalChildren;
	static std::atomic_int nbRunningThread;

	/// true if we want to use clone optimization
	/// a clone is a item from binary representation 
	bool useCloneOptimization;
	std::vector<std::shared_ptr<TreeNode>> children;
	//std::vector<std::unique_ptr<std::thread>> childrenThread;
	//std::unique_ptr<std::thread> thread;
	std::vector<std::thread> threadList;

	std::shared_ptr<BinaryRepresentation> binaryRepresentation;

	//std::vector<Utils::Itemset> toTraverse;
	//std::vector<Utils::Itemset> maxClique;
	//std::vector<Utils::Itemset> toExplore;

	//std::mutex myMutex;
		
private:
	void buildClonedCombinaison(const Utils::Itemset& currentItem, std::vector<Utils::Itemset>& clonedCombination, const std::vector<std::pair<unsigned int, unsigned int>>& originalClonedIndexes);

	/// compute maxClique list, toExplore list and mt list
	/// update graph_mt with new minimal transversal itemset
	void computeListsFromToTraverse(const std::vector<Utils::Itemset>& toTraverse, std::vector<Utils::Itemset>& maxClique, std::vector<Utils::Itemset>& toExplore, std::vector<Utils::Itemset>& graph_mt);

	//void computeMinimalTransversalsCb(int nb, std::vector<Utils::Itemset>& graph_mt);
	
public:
	TreeNode(bool useCloneOptimization, const std::shared_ptr<BinaryRepresentation>& binaryRepresentation);
	~TreeNode();

	void joinThead();

	/// recursive function, trasvere treen node to compute minimal transversals for binary representation
	/// @param graph_mt minimal transversal itemset list, updated at each branch trasversal
	void computeMinimalTransversals(const std::vector<Utils::Itemset>& toTraverse, std::vector<Utils::Itemset>& graph_mt);
};

