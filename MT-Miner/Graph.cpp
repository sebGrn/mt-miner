#include "Graph.h"

GraphNode::GraphNode(const std::vector<Utils::Itemset>& toTraverse, const std::shared_ptr<BinaryRepresentation> binaryRepresentation)
{
	this->binaryRepresentation = binaryRepresentation;
	this->verbose = true;
	this->toTraverse = toTraverse;
	//this->traverseDone = false;
}

void GraphNode::addChild(const std::shared_ptr<GraphNode>& node)
{
	this->children.push_back(node);

	//node->computeMinimalTransversals();
		
	// concatenating parent mt from the branch with current mt list
	//this->graph_mt.insert(this->graph_mt.end(), node->node_mt.begin(), node->node_mt.end());
}

void GraphNode::computeMinimalTransversals(std::vector<Utils::Itemset>& graph_mt)
{
	// results of cumulated combined items
	// must be declared outside of the loop
	Utils::Itemset previousItem;
	
	// build maxClique, toExplore lists
	for_each(toTraverse.begin(), toTraverse.end(), [&](const Utils::Itemset& currentItem) {

		unsigned int disjSup = this->binaryRepresentation->computeDisjonctifSupport(currentItem);
		if (disjSup == this->binaryRepresentation->getObjectCount())
		{
			//this->node_itemset = currentItem;
			//this->node_mt.push_back(currentItem);
			graph_mt.push_back(currentItem);

			// if this itemset contains an original, add the same minimal transverals list with the clone
			unsigned int originalIndex = 0;
			unsigned int clonedIndex = 0;
			if (this->binaryRepresentation->containsAnOriginal(currentItem, originalIndex, clonedIndex))
			{
				Utils::Itemset clonedCurrentItem = currentItem;
				replace(clonedCurrentItem.begin(), clonedCurrentItem.end(), originalIndex, clonedIndex);
				graph_mt.push_back(clonedCurrentItem);
			}

			if (verbose)
				std::cout << "-------> minimalTraversal list : " << Utils::itemsetListToString(graph_mt) << std::endl;
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
					toExplore.push_back(currentItem);
					if (verbose)
						std::cout << "--> toExplore list : " << Utils::itemsetListToString(toExplore) << std::endl;
				}
			}
		}
	});

	if (toExplore.empty())
	{
		//this->traverseDone = true;
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

		// analyse toExplore, if we have a clone, remove the clone from the list
		// the add the result of the cloned itemset for the clone itemset
		/*std::vector<std::vector<Utils::Itemset>::iterator> removeIteratorList;
		std::map<Utils::Itemset, std::vector<Utils::Itemset>> clonedItemsetsMap;
		for (auto it1 = toExplore.begin(); it1 != toExplore.end(); it1++)
		{
			std::vector<Utils::Itemset> clonedList;
			for (auto it2 = it1; it2 != toExplore.end(); it2++)
			{
				if (it1 != it2)
				{
					if (binaryRepresentation->compareItemsets(*it1, *it2))
					{
						// we have a clone here
						// remove it1 from the list
						clonedList.push_back(*it2);
						// add iterator to remove list
						removeIteratorList.push_back(it2);
					}
				}
			}
			if(!clonedList.empty())
				clonedItemsetsMap[*it1] = clonedList;
		}
		// remove clones
		for (auto it : removeIteratorList)
			toExplore.erase(it);*/


			
		// store toExploreList max index
		unsigned int lastIndexToTest = toExplore.size();
		// combine toExplore (left part) with maxClique list (right part) into a combined list
		std::vector<Utils::Itemset> combinedItemsetList = toExplore;
		combinedItemsetList.insert(combinedItemsetList.end(), maxClique.begin(), maxClique.end());

		for (unsigned int i = 0; i < lastIndexToTest; i++)
		{
			std::vector<Utils::Itemset> newToTraverse;
			Utils::Itemset toCombinedLeft = combinedItemsetList[i];
			
			// if this itemset is a clone, do not compute the minimal transverals
			unsigned int originalIndex = 0;
			unsigned int clonedIndex = 0;
			if (!this->binaryRepresentation->containsAClone(toCombinedLeft, originalIndex, clonedIndex))
			{
				bool containsAClone = false;
				for (unsigned int j = i + 1; j < combinedItemsetList.size(); j++)
				{
					assert(j < combinedItemsetList.size());
					Utils::Itemset toCombinedRight = combinedItemsetList[j];
					Utils::Itemset combinedItemset = Utils::combineItemset(toCombinedLeft, toCombinedRight);

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
				node->computeMinimalTransversals(graph_mt);
				int k = 0;

				//for (auto it = newToTraverse.begin(); it < newToTraverse.end(); it++)
				//{
				//	// if this itemset contains an original, add the same minimal transverals list with the clone
				//	if (this->binaryRepresentation->containsAnOriginal(*it))
				//	{
				//		int k = 0;
				//	}
				//}
			}

			// if this item contains a bitset index which has a clone, replace the 

			// check it itemset has a clone
			/*if (clonedItemsetsMap.find(toCombinedLeft) != clonedItemsetsMap.end())
			{
				std::vector<Utils::Itemset> clones = clonedItemsetsMap.at(toCombinedLeft);
				for (Utils::Itemset itemset_from_clones : clones)
				{
					// replace itemset by copy ones from the minimal transversals of the cloned items
					for (Utils::Itemset itemset_from_mt : graph_mt)
					{  
						// if itemset_from_clones is contained in itemset_from_mt
						// replace itemset from graph_mt (itemset_from_mt) by cloned itemset (itemset_from_clones) 
						// then build a new mt list

						//std::replace(mt_itemset.begin(), mt_itemset.end(), 
						int k = 0;
					}
				}
			}*/
		}


	}
	//return this->node_mt;
}

