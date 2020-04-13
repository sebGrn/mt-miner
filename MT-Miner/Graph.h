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

	bool traverseDone;
	Utils::Itemset node_itemset;
	std::vector<Utils::Itemset> node_mt;
	
public:
	GraphNode(const std::vector<Utils::Itemset>& toTraverse, const std::shared_ptr<BinaryRepresentation> binaryRepresentation);
	void addChild(const std::shared_ptr<GraphNode>& node);

	std::vector<Utils::Itemset> computeMinimalTransversals(std::vector<Utils::Itemset>& graph_mt);

	//std::vector<Utils::Itemset> getGraphMt() const
	//{
	//	return this->graph_mt;
	//}
};

