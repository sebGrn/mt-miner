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
	std::vector<std::shared_ptr<GraphNode>> children;
	std::shared_ptr<BinaryRepresentation> binaryRepresentation;

	std::vector<Utils::Itemset> toTraverse;
	std::vector<Utils::Itemset> maxClique;
	std::vector<Utils::Itemset> toExplore;
	
public:
	GraphNode(bool verbose, const std::vector<Utils::Itemset>& toTraverse, const std::shared_ptr<BinaryRepresentation> binaryRepresentation);
	void addChild(const std::shared_ptr<GraphNode>& node);

	void computeMinimalTransversals(std::vector<Utils::Itemset>& graph_mt);
};

