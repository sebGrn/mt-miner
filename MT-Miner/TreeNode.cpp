#include "TreeNode.h"
#include "Logger.h"
#include "Profiler.h"
#include "JsonTree.h"

std::atomic_int TreeNode::nbRunningThread(0);
std::atomic_ullong TreeNode::nbTotalChildren(0);
std::atomic_int TreeNode::processorCount(std::thread::hardware_concurrency());

TreeNode::TreeNode(bool useCloneOptimization, const std::shared_ptr<BinaryRepresentation>& binaryRepresentation)
{
	this->binaryRepresentation = binaryRepresentation;
	this->useCloneOptimization = useCloneOptimization;
	this->useMultitheadOptimization = true;
}

TreeNode::~TreeNode()
{
}

void TreeNode::buildClonedCombination(const Itemset& currentItem, std::vector<Itemset>& clonedCombination, const std::vector<std::pair<unsigned int, unsigned int>>& originalClonedIndexes)
{
	for (auto it = originalClonedIndexes.begin(); it != originalClonedIndexes.end(); it++)
	{
		unsigned int originalIndex = it->first;
		unsigned int clonedIndex = it->second;
		Itemset clonedCurrentItem = currentItem;
		replace(clonedCurrentItem.begin(), clonedCurrentItem.end(), originalIndex, clonedIndex);
		if (clonedCurrentItem != currentItem)
		{
			auto it = std::find_if(clonedCombination.begin(), clonedCombination.end(), Utils::compare_itemset(clonedCurrentItem));
			if (it == clonedCombination.end())
			{
				clonedCombination.push_back(clonedCurrentItem);
				// recurse on new combination
				buildClonedCombination(clonedCurrentItem, clonedCombination, originalClonedIndexes);
			}
		}
	}
}

void TreeNode::updateListsFromToTraverse(const ItemsetList& toTraverse, ItemsetList& maxClique, ItemsetList& toExplore, ItemsetList& graph_mt)
{
	maxClique.clear();
	toExplore.clear();

	// results of cumulated combined items / must be declared outside of the loop
	Itemset previousItem;
	// loop on toTraverse list and build maxClique and toExplore lists
	//for_each(toTraverse.begin(), toTraverse.end(), [&](const Itemset& currentItem) {
	for (auto it = toTraverse.begin(); it != toTraverse.end(); it++)
	{
		Itemset currentItem = *it;
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
					ItemsetList clonedCombination;
					buildClonedCombination(currentItem, clonedCombination, originalClonedIndexes);
					std::copy(clonedCombination.begin(), clonedCombination.end(), std::back_inserter(graph_mt));
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
				Itemset combinedItem = Utils::combineItemset(previousItem, currentItem);
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

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------- //

std::vector<Itemset> TreeNode::computeMinimalTransversals_iterative(const std::vector<Itemset>& toTraverse)
{
	typedef std::vector<Itemset> ItemsetList;

	// test trivial case
	if (toTraverse.empty())
		return ItemsetList();

	// structure to queue and synchronize toExplore and maxClique lists
	struct TailList
	{
		std::vector<ItemsetList> toExploreTailList;
		std::vector<ItemsetList> maxCliqueTailList;
	};
	TailList iterativeTailList;

	// contains the final minimal transverals for this node
	ItemsetList graph_mt;
	{
		// contains list of itemsets that are candidates
		ItemsetList toExplore;
		// contains list of itemsets that will be combined to the candidates
		ItemsetList maxClique;
		// update lists from toTraverse
		this->updateListsFromToTraverse(toTraverse, maxClique, toExplore, graph_mt);

		//Logger::log("toExplore list", ItemsetListToString(toExplore), "\n");
		//Logger::log("maxClique list", ItemsetListToString(maxClique), "\n");

		iterativeTailList.toExploreTailList.push_back(toExplore);
		iterativeTailList.maxCliqueTailList.push_back(maxClique);
	}

	auto timer1 = std::chrono::system_clock::now();
	//auto timer2 = std::chrono::system_clock::now();
	//auto timer3 = std::chrono::system_clock::now();
	//auto duration2 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - timer2).count();
	//auto duration3 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - timer3).count();

	unsigned int nbIter = 0;

	std::vector<long long> durationList1;
	std::vector<long long> durationList2;
	std::vector<long long> durationList3;

	while (!iterativeTailList.toExploreTailList.empty())
	{
		//auto timer2 = std::chrono::system_clock::now();
		//auto timer3 = std::chrono::system_clock::now();

		std::this_thread::sleep_for(std::chrono::milliseconds(1));

		ItemsetList toExplore = iterativeTailList.toExploreTailList.back();
		iterativeTailList.toExploreTailList.pop_back();

		ItemsetList maxClique = iterativeTailList.maxCliqueTailList.back();
		iterativeTailList.maxCliqueTailList.pop_back();

		unsigned int lastIndexToTest = static_cast<unsigned int>(toExplore.size());
		// combine toExplore (left part) with maxClique list (right part) into a combined list
		ItemsetList combinedItemsetList = toExplore;
		combinedItemsetList.insert(combinedItemsetList.end(), maxClique.begin(), maxClique.end());

		//durationList2.push_back(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - timer2).count());

		
		//std::this_thread::sleep_for(std::chrono::milliseconds(1));

		// loop on candidates from toExplore list only
		for (unsigned int i = 0; i < lastIndexToTest; i++)
		{
			// build newTraverse list
			ItemsetList newToTraverse;
			Itemset toCombinedLeft = combinedItemsetList[i];
			// combine each element between [0, lastIndexToTest] with the entire combined itemset list
			for (unsigned int j = i + 1; j < combinedItemsetList.size(); j++)
			{
				assert(j < combinedItemsetList.size());
				Itemset toCombinedRight = combinedItemsetList[j];
				Itemset && combinedItemset = Utils::combineItemset(toCombinedLeft, toCombinedRight);

				// check if combined item is containing a clone (if true, do not compute the minimal transverals) and if combined itemset is essential
				if (!this->binaryRepresentation->containsAClone(combinedItemset) && binaryRepresentation->isEssential(combinedItemset))
					newToTraverse.push_back(combinedItemset);
			}

			if (!newToTraverse.empty())
			{
				// compute minimal transversals for the branch
				this->updateListsFromToTraverse(newToTraverse, maxClique, toExplore, graph_mt);

				//std::this_thread::sleep_for(std::chrono::milliseconds(100));

				if (!toExplore.empty())
				{
					iterativeTailList.toExploreTailList.push_back(toExplore);
					iterativeTailList.maxCliqueTailList.push_back(maxClique);
				}
			}
		}		
		//durationList3.push_back(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - timer3).count());
	
		nbIter++;
	}

	//int duration1 = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - timer1).count();
	//int duration2 = std::accumulate(durationList2.begin(), durationList2.end(), 0);
	//int duration3 = std::accumulate(durationList3.begin(), durationList3.end(), 0);
	//double avg = duration3 / (double)durationList3.size();

	//Logger::log(RED, "duration1 ", duration1, " ms\n", RESET);
	//Logger::log(RED, "duration2 ", duration2, " ms\n", RESET);
	//Logger::log(RED, "duration3 ", duration3, " ms\n", RESET);
	//Logger::log(RED, "avg ", avg, " ms\n", RESET);
	Logger::log(RED, "nbIter ", nbIter, "\n", RESET);
	
	return graph_mt;
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------- //

void TreeNode::exploreNextBranch(const ItemsetList& maxClique, const ItemsetList& toExplore, ItemsetList& graph_mt)
{
	// store toExploreList max index
	unsigned int lastIndexToTest = static_cast<unsigned int>(toExplore.size());
	// combine toExplore (left part) with maxClique list (right part) into a combined list
	ItemsetList combinedItemsetList = toExplore;
	combinedItemsetList.insert(combinedItemsetList.end(), maxClique.begin(), maxClique.end());

	// loop on candidates from toExplore list only
	for (unsigned int i = 0; i < lastIndexToTest; i++)
	{
		// build newTraverse list
		ItemsetList newToTraverse;
		Itemset toCombinedLeft = combinedItemsetList[i];
		// combine each element between [0, lastIndexToTest] with the entire combined itemset list
		for (unsigned int j = i + 1; j < combinedItemsetList.size(); j++)
		{
			assert(j < combinedItemsetList.size());
			Itemset toCombinedRight = combinedItemsetList[j];
			Itemset combinedItemset = Utils::combineItemset(toCombinedLeft, toCombinedRight);

			// check if combined item is containing a clone (if true, do not compute the minimal transverals) and if combined itemset is essential
			if (!this->binaryRepresentation->containsAClone(combinedItemset) && binaryRepresentation->isEssential(combinedItemset))
				newToTraverse.push_back(combinedItemset);
		}

		if (!newToTraverse.empty())
		{
			// create a new child node for this newToTraverse list and add the node as a child
			std::shared_ptr<TreeNode> node = std::make_shared<TreeNode>(this->useCloneOptimization, this->binaryRepresentation);
			this->children.push_back(node);
			nbTotalChildren++;

			// recurse
			if (this->useMultitheadOptimization && nbRunningThread < processorCount)
			{
				// create thread for the top nodes only
				nbRunningThread++;
				//std::cout << BLUE << "launch thead " << nbRunningThread << RESET << std::endl;
				futures.push_back(std::async(&TreeNode::computeMinimalTransversals_recursive, node, std::move(newToTraverse)));
			}
			else
			{
				// do not create a thread for each node, compute minimal transversals for the branch
				std::vector<Itemset>&& graph_mt_child = node->computeMinimalTransversals_recursive(std::move(newToTraverse));
				std::copy(graph_mt_child.begin(), graph_mt_child.end(), std::back_inserter(graph_mt));
			}
		}
	}
}

std::vector<Itemset> TreeNode::computeMinimalTransversals_recursive(const std::vector<Itemset>& toTraverse)
{
	// test trivial case
	if (toTraverse.empty())
		return ItemsetList();

	// contains list of itemsets that will be combined to the candidates
	ItemsetList maxClique;
	// contains list of itemsets that are candidates
	ItemsetList toExplore;
	// contains the final minimal transverals for this node
	ItemsetList graph_mt;
	// update lists from toTraverse
	this->updateListsFromToTraverse(toTraverse, maxClique, toExplore, graph_mt);

	//Logger::log("toExplore list", ItemsetListToString(toExplore), "\n");
	//Logger::log("maxClique list", ItemsetListToString(maxClique), "\n");

	// add json node for js visualisation
	//JsonTree::addJsonNode(toExplore);

	// build new toTraverse list and explore next branch
	if (!toExplore.empty())
		this->exploreNextBranch(maxClique, toExplore, graph_mt);

	// manage futures
	try
	{
		while (!futures.empty())
		{
			auto ftr = std::move(futures.back());
			futures.pop_back();
			// wait here all tasks to finish (barrier), corresponding to all the children tasks executed in parallel
			ItemsetList&& child_mt = ftr.get();
			std::copy(child_mt.begin(), child_mt.end(), std::back_inserter(graph_mt));

			nbRunningThread--;
		}

		//std::for_each(futures.begin(), futures.end(), [&graph_mt](std::future<ItemsetList>& future) {
		//	// wait here all tasks to finish (barrier), corresponding to all the children tasks executed in parallel
		//	ItemsetList child_mt = future.get();
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

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------- //