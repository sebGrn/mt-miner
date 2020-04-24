#include "Graph.h"
#include "Logger.h"

GraphNode::GraphNode(bool showClones, const std::vector<Utils::Itemset>& toTraverse, const std::shared_ptr<BinaryRepresentation> binaryRepresentation)
{
	this->binaryRepresentation = binaryRepresentation;
	this->showClones = showClones;
	this->toTraverse = toTraverse;
}

void GraphNode::computeLists(std::vector<Utils::Itemset>& graph_mt)
{
	maxClique.clear();
	toExplore.clear();

	// results of cumulated combined items / must be declared outside of the loop
	Utils::Itemset previousItem;
	// build maxClique, toExplore lists
	for_each(toTraverse.begin(), toTraverse.end(), [&](const Utils::Itemset& currentItem) {

		unsigned int disjSup = this->binaryRepresentation->computeDisjonctifSupport(currentItem);
		if (disjSup == this->binaryRepresentation->getObjectCount())
		{
			// we have a minimal transversal
			graph_mt.push_back(currentItem);
			Logger::log("--> minimalTraversal list, add : ", Utils::itemsetToString(currentItem), ", size : ", std::to_string(graph_mt.size()), "\n");
			//if (verbose)
			//	std::cout << "--> minimalTraversal list : " << Utils::itemsetListToString(graph_mt) << std::endl;

			// if this itemset contains an original, add the same minimal transverals list with the clone
			if (this->showClones)
			{
				unsigned int originalIndex = 0;
				unsigned int clonedIndex = 0;
				if (this->binaryRepresentation->containsAnOriginal(currentItem, originalIndex, clonedIndex))
				{
					Utils::Itemset clonedCurrentItem = currentItem;
					replace(clonedCurrentItem.begin(), clonedCurrentItem.end(), originalIndex, clonedIndex);
					graph_mt.push_back(clonedCurrentItem);
					Logger::log("--> minimalTraversal list (clone), add : ", Utils::itemsetToString(clonedCurrentItem), ", size : ", std::to_string(graph_mt.size()), "\n");
				}
			}
		}
		else
		{
			if (currentItem == *toTraverse.begin() && currentItem.size() == 1)
			{
				// must be the 1st element with only one element
				previousItem = currentItem;
				maxClique.push_back(currentItem);
				Logger::log("--> maxClique list : ", Utils::itemsetListToString(maxClique), "\n");
			}
			else
			{
				// here, we can combine with previous element / make a union on 2 elements
				Utils::Itemset combinedItem = Utils::combineItemset(previousItem, currentItem);
				// compute disjonctif support of the concatenation
				unsigned int disjSup = this->binaryRepresentation->computeDisjonctifSupport(combinedItem);
				Logger::log("disjonctive support for element ", Utils::itemsetToString(combinedItem), " : ", std::to_string(disjSup), "\n");

				if (disjSup != this->binaryRepresentation->getObjectCount())
				{
					previousItem = combinedItem;
					maxClique.push_back(currentItem);
					Logger::log("--> maxClique list : ", Utils::itemsetListToString(maxClique), "\n");
				}
				else
				{
					toExplore.push_back(currentItem);
					Logger::log("--> toExplore list : ", Utils::itemsetListToString(toExplore), "\n");
				}
			}
		}
	});
}

void GraphNode::computeMinimalTransversals(std::vector<Utils::Itemset>& graph_mt)
{
	// compute toExplore : contains list of itemsets that are candidates
	// compute maxClique : contains list of itemsets that will be combined to the candidates
	// update graph_mt : contains the final minimal transverals
	computeLists(graph_mt);

	if (toExplore.empty())
	{
		Logger::log("toExplore list is empty, end of the branch", "\n");
	}
	else
	{
		Logger::log("----------------------------------------------------------", "\n");
		Logger::log("toExplore list", Utils::itemsetListToString(toExplore), "\n");
		Logger::log("maxClique list", Utils::itemsetListToString(maxClique), "\n");

		// store toExploreList max index
		unsigned int lastIndexToTest = toExplore.size();
		// combine toExplore (left part) with maxClique list (right part) into a combined list
		std::vector<Utils::Itemset> combinedItemsetList = toExplore;
		combinedItemsetList.insert(combinedItemsetList.end(), maxClique.begin(), maxClique.end());
		
		// loop on candidates from toExplore list only
		for (unsigned int i = 0; i < lastIndexToTest; i++)
		{
			std::vector<Utils::Itemset> newToTraverse;
			Utils::Itemset toCombinedLeft = combinedItemsetList[i];			
			// if this itemset is a clone, do not compute the minimal transverals
			unsigned int originalIndex = 0;
			unsigned int clonedIndex = 0;
			if (!this->binaryRepresentation->containsAClone(toCombinedLeft, originalIndex, clonedIndex))
			{
				// combine each element between [0, lastIndexToTest] with the entire combined itemset list
				for (unsigned int j = i + 1; j < combinedItemsetList.size(); j++)
				{
					assert(j < combinedItemsetList.size());
					Utils::Itemset toCombinedRight = combinedItemsetList[j];
					Utils::Itemset combinedItemset = Utils::combineItemset(toCombinedLeft, toCombinedRight);

					// test if combined itemset is essential
					if (binaryRepresentation->isEssential(combinedItemset))
						newToTraverse.push_back(combinedItemset);
					else
					{
						Logger::log("", Utils::itemsetToString(combinedItemset), " is not essential", "\n");
					}
				}

				Logger::log("new toTraverse list", Utils::itemsetListToString(newToTraverse), "\n");
				Logger::log("----------------------------------------------------------", "\n");

				// create a new child node for this newToTraverse list
				std::shared_ptr<GraphNode> node = std::make_shared<GraphNode>(this->showClones, newToTraverse, this->binaryRepresentation);
				// add this node as a child
				this->children.push_back(node);
				// compute minimal transversals for the branch
				node->computeMinimalTransversals(graph_mt);
			}
			else
			{
				Logger::log("", Utils::itemsetListToString(combinedItemsetList), " contains a clone, do not compute mt", "\n");
			}
		}
	}
}

