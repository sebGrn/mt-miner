#pragma once
#include <vector>
#include <map>
#include <string>
#include <algorithm>

#include "utils.h"
#include "BinaryRepresentation.h"

class TreeNode
{
private:
	/// true if we want to use clone optimization
	/// a clone is a item from binary representation 
	bool useCloneOptimization;
	std::vector<std::shared_ptr<GraphNode>> children;
	std::shared_ptr<BinaryRepresentation> binaryRepresentation;

	std::vector<Utils::Itemset> toTraverse;
	std::vector<Utils::Itemset> maxClique;
	std::vector<Utils::Itemset> toExplore;
	
private:
	void buildClonedCombinason(const Utils::Itemset& currentItem, std::vector<Utils::Itemset>& clonedCombination, const std::vector<std::pair<unsigned int, unsigned int>>& originalClonedIndexes);

	/// compute maxClique list, toExplore list and mt list
	/// update graph_mt with new minimal transversal itemset
	void computeLists(std::vector<Utils::Itemset>& graph_mt);

public:
	TreeNode(bool useCloneOptimization, const std::vector<Utils::Itemset>& toTraverse, const std::shared_ptr<BinaryRepresentation>& binaryRepresentation);

	/// add add a new node as a child of this current node
	/// @param node child node
	void addChild(const std::shared_ptr<TreeNode>& node);

	/// recursive function, trasvere treen node to compute minimal transversals for binary representation
	/// @param graph_mt minimal transversal itemset list, updated at each branch trasversal
	void computeMinimalTransversals(std::vector<Utils::Itemset>& graph_mt);
};

