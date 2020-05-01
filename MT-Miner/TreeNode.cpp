#include "TreeNode.h"
#include "Logger.h"

std::atomic_int TreeNode::nbRunningThread = 0;
std::atomic_int TreeNode::nbTotalChildren = 0;
std::atomic_int TreeNode::processorCount = std::thread::hardware_concurrency();

TreeNode::TreeNode(bool useCloneOptimization, const std::shared_ptr<BinaryRepresentation>& binaryRepresentation)
{
	this->binaryRepresentation = binaryRepresentation;
	this->useCloneOptimization = useCloneOptimization;
}

TreeNode::~TreeNode()
{
}

void TreeNode::buildClonedCombinaison(const Utils::Itemset& currentItem, std::vector<Utils::Itemset>& clonedCombination, const std::vector<std::pair<unsigned int, unsigned int>>& originalClonedIndexes)
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
				buildClonedCombinaison(clonedCurrentItem, clonedCombination, originalClonedIndexes);
			}
		}
	}
}

void TreeNode::computeListsFromToTraverse(const std::vector<Utils::Itemset>& toTraverse, std::vector<Utils::Itemset>& maxClique, std::vector<Utils::Itemset>& toExplore, std::vector<Utils::Itemset>& graph_mt)
{
	maxClique.clear();
	toExplore.clear();

	// results of cumulated combined items / must be declared outside of the loop
	Utils::Itemset previousItem;
	// loop on toTraverse list and build maxClique and toExplore lists
	//for_each(toTraverse.begin(), toTraverse.end(), [&](const Utils::Itemset& currentItem) {
	for (auto it = toTraverse.begin(); it != toTraverse.end(); it++)
	{
		Utils::Itemset currentItem = *it;
		unsigned int disjSup = this->binaryRepresentation->computeDisjonctifSupport(currentItem);
		if (disjSup == this->binaryRepresentation->getObjectCount())
		{
			// we have a minimal transversal
			graph_mt.push_back(currentItem);

			if (this->useCloneOptimization)
			{
				// if this itemset contains an original, add the same minimal transverals list for all the clones
				std::vector<std::pair<unsigned int, unsigned int>> originalClonedIndexes;
				if (this->binaryRepresentation->containsOriginals(currentItem, originalClonedIndexes))
				{
					std::vector<Utils::Itemset> clonedCombination;
					buildClonedCombinaison(currentItem, clonedCombination, originalClonedIndexes);

					for (auto it = clonedCombination.begin(); it != clonedCombination.end(); it++)
					{
						graph_mt.push_back(*it);
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
			}
			else
			{
				// we can combine with previous element / make a union on 2 elements
				Utils::Itemset combinedItem = Utils::combineItemset(previousItem, currentItem);
				unsigned int disjSup = this->binaryRepresentation->computeDisjonctifSupport(combinedItem);
				if (disjSup != this->binaryRepresentation->getObjectCount())
				{
					previousItem = combinedItem;
					maxClique.push_back(currentItem);
				}
				else
				{
					toExplore.push_back(currentItem);
				}
			}
		}
	}
}

std::vector<Utils::Itemset> TreeNode::computeMinimalTransversals(const std::vector<Utils::Itemset> & toTraverse)
{
	// compute toExplore : contains list of itemsets that are candidates
	// compute maxClique : contains list of itemsets that will be combined to the candidates
	// update graph_mt : contains the final minimal transverals
	
	if (toTraverse.empty())
		return std::vector<Utils::Itemset>();

	std::vector<Utils::Itemset> maxClique;
	std::vector<Utils::Itemset> toExplore;
	std::vector<Utils::Itemset> graph_mt;
	this->computeListsFromToTraverse(toTraverse, maxClique, toExplore, graph_mt);

	if (!toExplore.empty())
	{
		//Logger::log("toExplore list", Utils::itemsetListToString(toExplore), " - recursion level ", recursionLevel, "\n");
		//Logger::log("maxClique list", Utils::itemsetListToString(maxClique), "\n");

		// store toExploreList max index
		unsigned int lastIndexToTest = toExplore.size();
		// combine toExplore (left part) with maxClique list (right part) into a combined list
		std::vector<Utils::Itemset> combinedItemsetList = toExplore;
		combinedItemsetList.insert(combinedItemsetList.end(), maxClique.begin(), maxClique.end());

		// loop on candidates from toExplore list only
		//#pragma omp parallel for
		for (int i = 0; i < lastIndexToTest; i++)
		{
			// build newTraverse list
			std::vector<Utils::Itemset> newToTraverse;
			Utils::Itemset toCombinedLeft = combinedItemsetList[i];
			// combine each element between [0, lastIndexToTest] with the entire combined itemset list
			for (unsigned int j = i + 1; j < combinedItemsetList.size(); j++)
			{
				assert(j < combinedItemsetList.size());
				Utils::Itemset toCombinedRight = combinedItemsetList[j];
				Utils::Itemset combinedItemset = Utils::combineItemset(toCombinedLeft, toCombinedRight);

				// check if combined item is containing a clone (if true, do not compute the minimal transverals) and if combined itemset is essential
				if (!this->binaryRepresentation->containsAClone(combinedItemset) && binaryRepresentation->isEssential(combinedItemset))
					newToTraverse.push_back(combinedItemset);
			}

			// create a new child node for this newToTraverse list and add the node as a child
			std::shared_ptr<TreeNode> node = std::make_shared<TreeNode>(this->useCloneOptimization, this->binaryRepresentation);
			this->children.push_back(node);
			nbTotalChildren++;

			// recurse
			if (nbRunningThread < processorCount)
			{
				// create thread for 1st branch 
				nbRunningThread++;
				//std::cout << "launch thead " << nbRunningThread << std::endl;
				futures.push_back(std::async(&TreeNode::computeMinimalTransversals, node, std::move(newToTraverse)));
			}
			else
			{
				// compute minimal transversals for the branch
				std::vector<Utils::Itemset>&& graph_mt_child = node->computeMinimalTransversals(std::move(newToTraverse));
				std::copy(graph_mt_child.begin(), graph_mt_child.end(), std::back_inserter(graph_mt));
			}
		}
	}

	try
	{
		while (!futures.empty())
		{
			auto ftr = std::move(futures.back());
			futures.pop_back();
			// wait here all tasks to finish (barrier), corresponding to all the children tasks executed in parallel
			std::vector<Utils::Itemset>&& child_mt = ftr.get();
			std::copy(child_mt.begin(), child_mt.end(), std::back_inserter(graph_mt));
		}

		//std::for_each(futures.begin(), futures.end(), [&graph_mt](std::future<std::vector<Utils::Itemset>>& future) {
		//	// wait here all tasks to finish (barrier), corresponding to all the children tasks executed in parallel
		//	std::vector<Utils::Itemset> child_mt = future.get();
		//	std::copy(child_mt.begin(), child_mt.end(), std::back_inserter(graph_mt));
		//});
	}
	catch (std::system_error& e)
	{
		std::cout << "system error " << e.code().message() << std::endl;
	}
	catch (std::exception& e)
	{
		std::cout << "exception " << e.what() << std::endl;
	}
	catch (...)
	{
		std::cout << "unknown exception" << std::endl;
	}
	return graph_mt;
}