#pragma once
#include <vector>
#include <map>
#include <string>
#include <algorithm>

#include "utils.h"
#include "BinaryRepresentation.h"

class GraphNode
{
private:
	bool verbose;
public:
	std::vector<std::shared_ptr<GraphNode>> children;

	std::shared_ptr<BinaryRepresentation> binaryRepresentation;

	std::vector<Utils::Itemset> toTraverse;

	// minimal transversal return list
	std::vector<Utils::Itemset> mt;

	//std::vector<Utils::Itemset> cumulated_mt;

	std::vector<Utils::Itemset> maxClique;
	std::vector<Utils::Itemset> toExplore;

	GraphNode(const std::vector<Utils::Itemset>& toTraverse, const std::shared_ptr<BinaryRepresentation> binaryRepresentation);
	void addChild(const std::shared_ptr<GraphNode>& node);

	std::vector<Utils::Itemset> computeMinimalTransversals();

};

