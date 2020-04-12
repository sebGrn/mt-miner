#include "Graph.h"

GraphNode::GraphNode(const std::vector<Utils::Itemset>& toTraverse, const std::shared_ptr<BinaryRepresentation> binaryRepresentation)
{
	this->binaryRepresentation = binaryRepresentation;
	this->verbose = true;
	this->toTraverse = toTraverse;
}

void GraphNode::addChild(const std::shared_ptr<GraphNode>& node)
{
	this->children.push_back(node);
	std::vector<Utils::Itemset> mtBranch = node->computeMinimalTransversals();

	// concatenating parent mt from the branch with current mt list
	mt.insert(mt.end(), mtBranch.begin(), mtBranch.end());
}

std::vector<Utils::Itemset> GraphNode::computeMinimalTransversals()
{
	// results of cumulated combined items
	// must be declared outside of the loop
	Utils::Itemset previousItem;
	
	// build maxClique, toExplore lists
	for_each(toTraverse.begin(), toTraverse.end(), [&](const Utils::Itemset& currentItem) {

		unsigned int disjSup = this->binaryRepresentation->computeDisjonctifSupport(currentItem);
		if (disjSup == this->binaryRepresentation->getObjectCount())
		{
			mt.push_back(currentItem);
			if (verbose)
				std::cout << "--> minimalTraversal list : " << Utils::itemsetListToString(mt) << std::endl;
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
				unsigned int disjSup = this->binaryRepresentation->computeDisjonctifSupport(currentItem);
				if (disjSup == this->binaryRepresentation->getObjectCount())
				{
					mt.push_back(currentItem);
					if (verbose)
						std::cout << "--> minimalTraversal list : " << Utils::itemsetListToString(mt) << std::endl;
				}
				else
				{
					// add combinaison of previous + current
					Utils::Itemset lastElt = previousItem;
					// make a union on 2 elements
					Utils::Itemset combinedItem = Utils::combineItemset(lastElt, currentItem);
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
		}
		});

	if (toExplore.empty())
	{
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
				Utils::Itemset combinedString = Utils::combineItemset(toCombinedLeft, toCombinedRight);

				if (binaryRepresentation->isEssential(combinedString))
					newToTraverse.push_back(combinedString);
				else
				{
					if (verbose)
						std::cout << Utils::itemsetToString(combinedString) << " is not essential" << std::endl;
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
			//std::vector<Utils::Itemset> mtBranch = computeMinimalTransversals(binaryRepresentation);

			// concatenating parent mt from the branch with current mt list
			//mt.insert(mt.end(), mtBranch.begin(), mtBranch.end());
		}
	}
	return this->mt;
}

