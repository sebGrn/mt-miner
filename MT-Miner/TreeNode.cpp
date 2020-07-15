#include "TreeNode.h"
#include "Logger.h"
#include "Profiler.h"
#include "JsonTree.h"

std::atomic_ullong TreeNode::nbTotalChildren(0);
// to avoid interleaved outputs
std::mutex TreeNode::output_guard;
// synchro stuff
std::deque<std::future<std::vector<Itemset>>> TreeNode::task_queue;
std::mutex TreeNode::task_guard;
std::condition_variable TreeNode::task_signal;
int TreeNode::pending_task_count(0);
//std::shared_ptr<BinaryRepresentation<CustomULBitset>> TreeNode::binaryRepresentation = std::make_shared<BinaryRepresentation<CustomULBitset>>();
std::shared_ptr<BinaryRepresentation<bitset_type>> TreeNode::binaryRepresentation = std::make_shared<BinaryRepresentation<bitset_type>>();

TreeNode::TreeNode(bool useCloneOptimization)
{
	this->useCloneOptimization = useCloneOptimization;
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

		// replace originalIndex into clonedIndex into clonedCurrentItem.itemset_list list
		std::replace(clonedCurrentItem.itemset_list.begin(), clonedCurrentItem.itemset_list.end(), originalIndex, clonedIndex);

		// tester le support de l'itemset pour savoir si le bitset est un clone avant de tester s'ils sont égaux
		if (clonedCurrentItem.itemset_list != currentItem.itemset_list)
		{
			auto it = std::find_if(clonedCombination.begin(), clonedCombination.end(), compare_itemset(clonedCurrentItem));
			if (it == clonedCombination.end())
			{
				// add cloned item into clonedCombination list					
				clonedCombination.push_back(clonedCurrentItem);
				// recurse on new combination with cloned item as current item
				buildClonedCombination(clonedCurrentItem, clonedCombination, originalClonedIndexes);
			}
		}
	}
}

void TreeNode::updateListsFromToTraverse(const std::vector<Itemset>& toTraverse, std::vector<Itemset>& maxClique, std::vector<Itemset>& toExplore, std::vector<Itemset>& graph_mt)
{
	maxClique.clear();
	toExplore.clear();

	// results of cumulated combined items / must be declared outside of the loop
	Itemset previousItem;
	// loop on toTraverse list and build maxClique and toExplore lists
	for (auto it = toTraverse.begin(); it != toTraverse.end(); it++)
	{
		Itemset& currentItem = const_cast<Itemset&>(*it);
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
					std::vector<Itemset> clonedCombination;
					buildClonedCombination(currentItem, clonedCombination, originalClonedIndexes);
					// copy from clonedCombination list into graph_mt list
					std::copy(clonedCombination.begin(), clonedCombination.end(), std::back_inserter(graph_mt));
				}
			}
		}
		else
		{
			if (currentItem.itemset_list == toTraverse.begin()->itemset_list && currentItem.itemset_list.size() == 1)
			{
				// must be the 1st element with only one element
				previousItem = currentItem;
				maxClique.push_back(currentItem);
			}
			else
			{
				// we can combine with previous element / make a union on 2 elements
				Itemset combinedItem = Itemset::combineItemset(previousItem, currentItem);
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

std::vector<Itemset> TreeNode::computeMinimalTransversals_task(const std::vector<Itemset>& toTraverse)
{
	// ## START TASK ##

	// test trivial case
	if (toTraverse.empty())
		return std::vector<Itemset>();

	// contains list of itemsets that will be combined to the candidates
	std::vector<Itemset> maxClique;
	// contains list of itemsets that are candidates
	std::vector<Itemset> toExplore;
	// contains the final minimal transverals for this node
	std::vector<Itemset> graph_mt;
	// update lists from toTraverse
	this->updateListsFromToTraverse(toTraverse, maxClique, toExplore, graph_mt);

	//Logger::log("toExplore list", ItemsetListToString(toExplore), "\n");
	//Logger::log("maxClique list", ItemsetListToString(maxClique), "\n");
	// add json node for js visualisation
	//JsonTree::addJsonNode(toExplore);

	// build new toTraverse list and explore next branch
	if (!toExplore.empty())
	{
		// store toExploreList max index
		unsigned int lastIndexToTest = static_cast<unsigned int>(toExplore.size());
		// combine toExplore (left part) with maxClique list (right part) into a combined list
		std::vector<Itemset>& combinedItemsetList = toExplore;
		combinedItemsetList.insert(combinedItemsetList.end(), maxClique.begin(), maxClique.end());

		// loop on candidates from toExplore list only
		for (unsigned int i = 0; i < lastIndexToTest; i++)
		{
			// build newTraverse list
			std::vector<Itemset> newToTraverse;
			Itemset& toCombinedLeft = combinedItemsetList[i];
			// combine each element between [0, lastIndexToTest] with the entire combined itemset list
			for (unsigned int j = i + 1; j < combinedItemsetList.size(); j++)
			{
				assert(j < combinedItemsetList.size());
				Itemset toCombinedRight = combinedItemsetList[j];
				Itemset combinedItemset = Itemset::combineItemset(toCombinedLeft, toCombinedRight);

				// check if combined item is containing a clone (if true, do not compute the minimal transverals) and if combined itemset is essential
				if (!this->binaryRepresentation->containsAClone(combinedItemset) && binaryRepresentation->isEssential(combinedItemset))
					newToTraverse.push_back(combinedItemset);
			}

			if (!newToTraverse.empty())
			{
				// emit recursive task
				nbTotalChildren++;
				// call on the same node, it works because no class members are used except atomics
				auto subtask = std::async(std::launch::deferred, &TreeNode::computeMinimalTransversals_task, this, std::move(newToTraverse));
				
				// ## SPAWN TASK ##
				{
					const std::lock_guard<std::mutex> lock(task_guard);
					task_queue.emplace_back(std::move(subtask));
					++pending_task_count;
				}
				task_signal.notify_one(); // be sure at least one unit is awaken

				// modify delay from 1 to 100 to see idle behaviour
				//std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}
		}
	}

	// terminate task
	const std::lock_guard<std::mutex> lock(task_guard);
	if (!--pending_task_count)
	{
		// ## EMIT SHUTDOWN SIGNAL ##
		// awake all idle units for auto-shutdown
		task_signal.notify_all();
	}
	// ## EMIT COMPLETE TASK ##

	return graph_mt;
}

std::vector<Itemset> TreeNode::computeMinimalTransversals(std::vector<Itemset>& toTraverse)
{
	// ## START system ##

	std::vector<Itemset> final_mt;
	// emit initial task
	auto task = std::async(std::launch::deferred, &TreeNode::computeMinimalTransversals_task, this, std::move(toTraverse));

	// ## SPAWN task ##
	{
		const std::lock_guard<std::mutex> lock(task_guard);
		task_queue.emplace_back(std::move(task));
		++pending_task_count;
	}

	// launch processing units
	std::list<std::future<std::vector<Itemset>>> units;
	const unsigned int thead_multiplicator = 4;
	for (auto n = std::thread::hardware_concurrency() * thead_multiplicator; --n;)
	{
		units.emplace_back(std::async(std::launch::async, [n]()
		{
			// ## LAUNCH task ##
			std::vector<Itemset> result_mt;

			std::unique_lock<std::mutex> lock(task_guard);
			while (true)
			{
				if (!task_queue.empty())
				{
					// pick a task
					auto task = std::move(task_queue.front());
					task_queue.pop_front();
						
					lock.unlock(); // unlock while processing task
					{
						// process task
						std::vector<Itemset> mt = task.get();
						std::copy(mt.begin(), mt.end(), std::back_inserter(result_mt));
					}
					lock.lock(); // reacquire lock
				}
				else if (!pending_task_count)
					break;
				else
				{
					// ## PAUSE / IDLE ##
					task_signal.wait(lock);
					// ## AWAKE ##
				}
			}
			
			// ## TERMINATE ##
			return result_mt;
		}));
	}

	// wait for shutdown
	for (auto& unit : units)
	{
		std::vector<Itemset> result = unit.get();
		{
			const std::lock_guard<std::mutex> lock(output_guard);
			std::copy(result.begin(), result.end(), std::back_inserter(final_mt));
		}
	}
	return final_mt;
}
