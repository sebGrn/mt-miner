#include "Graph.h"

GraphNode::GraphNode(const std::vector<Utils::Itemset>& toTraverse, const std::shared_ptr<BinaryRepresentation> binaryRepresentation)
{
	this->binaryRepresentation = binaryRepresentation;
	this->verbose = true;
	this->toTraverse = toTraverse;
	this->traverseDone = false;
}

void GraphNode::addChild(const std::shared_ptr<GraphNode>& node)
{
	this->children.push_back(node);

	//node->computeMinimalTransversals();
		
	// concatenating parent mt from the branch with current mt list
	//this->graph_mt.insert(this->graph_mt.end(), node->node_mt.begin(), node->node_mt.end());
}

std::vector<Utils::Itemset> GraphNode::computeMinimalTransversals(std::vector<Utils::Itemset>& graph_mt)
{
	// results of cumulated combined items
	// must be declared outside of the loop
	Utils::Itemset previousItem;
	
	// build maxClique, toExplore lists
	for_each(toTraverse.begin(), toTraverse.end(), [&](const Utils::Itemset& currentItem) {

		unsigned int disjSup = this->binaryRepresentation->computeDisjonctifSupport(currentItem);
		if (disjSup == this->binaryRepresentation->getObjectCount())
		{
			this->node_itemset = currentItem;
			this->node_mt.push_back(currentItem);
			graph_mt.push_back(currentItem);
			if (verbose)
				std::cout << "-------> minimalTraversal list : " << Utils::itemsetListToString(this->node_mt) << std::endl;
		}
		else
		{
			if (currentItem == *toTraverse.begin() && currentItem.size() == 1)
			{
				// must be the 1st element with only one element
				previousItem = currentItem;
				maxClique.push_back(currentItem);
				if (verbose)
					std::cout << "--> maxClique list : " << Utils::itemsetListToString(maxClique) << std::endl;
			}
			else
			{
				// here, we can combine with previous element
				// make a union on 2 elements
				Utils::Itemset combinedItem = Utils::combineItemset(previousItem, currentItem);
				// compute disjonctif support of the concatenation
				unsigned int disjSup = this->binaryRepresentation->computeDisjonctifSupport(combinedItem);
				if (verbose)
					std::cout << "disjonctive support for element \"" << Utils::itemsetToString(combinedItem) << "\" : " << disjSup << std::endl;
				if (disjSup != this->binaryRepresentation->getObjectCount())
				{
					previousItem = combinedItem;
					maxClique.push_back(currentItem);
					if (verbose)
						std::cout << "--> maxClique list : " << Utils::itemsetListToString(maxClique) << std::endl;
				}
				else
				{
					// check if columnKey has a clone
					// if it has a clone, use the clone value computation instead of computing the support
					//{
					//	for (unsigned int i = 0; i < toExplore.size(); i++)
					//	{

					//	}
					//}

					toExplore.push_back(currentItem);
					if (verbose)
						std::cout << "--> toExplore list : " << Utils::itemsetListToString(toExplore) << std::endl;
				}
			}
		}
	});

	if (toExplore.empty())
	{
		this->traverseDone = true;
		if (verbose)
			std::cout << "toExplore list is empty, end of the branch" << std::endl;
	}
	else
	{
		if (verbose)
		{
			std::cout << "----------------------------------------------------------" << std::endl;
			std::cout << "toExplore list" << Utils::itemsetListToString(toExplore) << std::endl;
			std::cout << "maxClique list" << Utils::itemsetListToString(maxClique) << std::endl;
		}

		// store toExploreList max index
		unsigned int lastIndexToTest = toExplore.size();
		// combine toExplore (left part) with maxClique list (right part) into a combined list
		std::vector<Utils::Itemset> combinedList = toExplore;
		combinedList.insert(combinedList.end(), maxClique.begin(), maxClique.end());

		for (unsigned int i = 0; i < lastIndexToTest; i++)
		{
			//if (isClone(i))
			//{

			//}

			std::vector<Utils::Itemset> newToTraverse;
			Utils::Itemset toCombinedLeft = combinedList[i];

			for (unsigned int j = i + 1; j < combinedList.size(); j++)
			{
				assert(j < combinedList.size());
				Utils::Itemset toCombinedRight = combinedList[j];
				Utils::Itemset combinedItemset= Utils::combineItemset(toCombinedLeft, toCombinedRight);

				if (binaryRepresentation->isEssential(combinedItemset))
					newToTraverse.push_back(combinedItemset);
				else
				{
					if (verbose)
						std::cout << Utils::itemsetToString(combinedItemset) << " is not essential" << std::endl;
				}
			}

			if (verbose)
			{
				std::cout << "new toTraverse list" << Utils::itemsetListToString(newToTraverse) << std::endl;
				std::cout << "----------------------------------------------------------" << std::endl;
			}

			std::shared_ptr<GraphNode> node = std::make_shared<GraphNode>(newToTraverse, this->binaryRepresentation);
			this->addChild(node);

			// compute minimal transversals for the branch
			std::vector<Utils::Itemset> branch_mt = node->computeMinimalTransversals(graph_mt);
		}
	}
	return this->node_mt;
}

