#include "TreeNode.h"
#include "Logger.h"

TreeNode::TreeNode(bool useCloneOptimization, const std::vector<Utils::Itemset>& toTraverse, const std::shared_ptr<BinaryRepresentation>& binaryRepresentation)
{
	this->binaryRepresentation = binaryRepresentation;
	this->useCloneOptimization = useCloneOptimization;
	this->toTraverse = toTraverse;
}

void TreeNode::buildClonedCombinason(const Utils::Itemset& currentItem, std::vector<Utils::Itemset>& clonedCombination, const std::vector<std::pair<unsigned int, unsigned int>>& originalClonedIndexes)
{
	for (auto it = originalClonedIndexes.begin(); it != originalClonedIndexes.end(); it++)
	{
		unsigned int originalIndex = it->first;
		unsigned int clonedIndex = it->second;
		Utils::Itemset clonedCurrentItem = currentItem;
		replace(clonedCurrentItem.begin(), clonedCurrentItem.end(), originalIndex, clonedIndex);
		if (clonedCurrentItem != currentItem)
		{
			auto it = std::find_if(clonedCombination.begin(), clonedCombination.end(), Utils::compare_itemset(clonedCurrentItem));
			if (it == clonedCombination.end())
			{
				clonedCombination.push_back(clonedCurrentItem);
				// recurse on new combination
				buildClonedCombinason(clonedCurrentItem, clonedCombination, originalClonedIndexes);
			}
		}
	}
}

void TreeNode::computeLists(std::vector<Utils::Itemset>& graph_mt)
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
			std::string tmp = Utils::itemsetToString(currentItem);
			Logger::log("--> minimalTraversal list, add : ", Utils::itemsetToString(currentItem), ", size : ", std::to_string(graph_mt.size()), "\n");

			// if this itemset contains an original, add the same minimal transverals list with the clone
			if (this->useCloneOptimization)
			{
				std::vector<std::pair<unsigned int, unsigned int>> originalClonedIndexes;
				if (this->binaryRepresentation->containsOriginals(currentItem, originalClonedIndexes))
				{
					std::vector<Utils::Itemset> clonedCombination;
					buildClonedCombinason(currentItem, clonedCombination, originalClonedIndexes);

					for (auto it = clonedCombination.begin(); it != clonedCombination.end(); it++)
					{
						graph_mt.push_back(*it);
						Logger::log("--> minimalTraversal list (clone), add : ", Utils::itemsetToString(*it), ", size : ", std::to_string(graph_mt.size()), "\n");
					}
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
				//Logger::log("--> maxClique list : ", Utils::itemsetListToString(maxClique), "\n");
			}
			else
			{
				// here, we can combine with previous element / make a union on 2 elements
				Utils::Itemset combinedItem = Utils::combineItemset(previousItem, currentItem);
				// compute disjonctif support of the concatenation
				unsigned int disjSup = this->binaryRepresentation->computeDisjonctifSupport(combinedItem);
				//Logger::log("disjonctive support for element ", Utils::itemsetToString(combinedItem), " : ", std::to_string(disjSup), "\n");

				if (disjSup != this->binaryRepresentation->getObjectCount())
				{
					previousItem = combinedItem;
					maxClique.push_back(currentItem);
					//Logger::log("--> maxClique list : ", Utils::itemsetListToString(maxClique), "\n");
				}
				else
				{
					toExplore.push_back(currentItem);
					//Logger::log("--> toExplore list : ", Utils::itemsetListToString(toExplore), "\n");
				}
			}
		}
	});
}

void TreeNode::computeMinimalTransversals(std::vector<Utils::Itemset>& graph_mt)
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
		//#pragma omp parallel for
		for (int i = 0; i < lastIndexToTest; i++)
		{
			std::vector<Utils::Itemset> newToTraverse;
			Utils::Itemset toCombinedLeft = combinedItemsetList[i];			
			// if this itemset is a clone, do not compute the minimal transverals
			std::string tmp = Utils::itemsetToString(toCombinedLeft);
			if (!this->binaryRepresentation->containsAClone(toCombinedLeft))
			{
				// combine each element between [0, lastIndexToTest] with the entire combined itemset list
				for (unsigned int j = i + 1; j < combinedItemsetList.size(); j++)
				{
					assert(j < combinedItemsetList.size());
					Utils::Itemset toCombinedRight = combinedItemsetList[j];
					Utils::Itemset combinedItemset = Utils::combineItemset(toCombinedLeft, toCombinedRight);

					if (!this->binaryRepresentation->containsAClone(combinedItemset))
					{
						// test if combined itemset is essential
						if (binaryRepresentation->isEssential(combinedItemset))
							newToTraverse.push_back(combinedItemset);
						else
						{
							Logger::log("", Utils::itemsetToString(combinedItemset), " is not essential", "\n");
						}
					}
					else
					{
						Logger::log("", Utils::itemsetToString(combinedItemset), " contains a clone, do not compute mt", "\n");
					}
				}

				Logger::log("new toTraverse list", Utils::itemsetListToString(newToTraverse), "\n");
				Logger::log("----------------------------------------------------------", "\n");

				// create a new child node for this newToTraverse list
				std::shared_ptr<TreeNode> node = std::make_shared<TreeNode>(this->useCloneOptimization, newToTraverse, this->binaryRepresentation);
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

