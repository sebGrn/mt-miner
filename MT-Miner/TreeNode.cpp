#include "TreeNode.h"
#include "Logger.h"
#include "Profiler.h"

std::atomic_ullong TreeNode::nbTotalChildren(0);
// to avoid interleaved outputs
std::mutex TreeNode::output_guard;
// synchro stuff
std::deque<std::future<std::vector<std::shared_ptr<Itemset>>>> TreeNode::task_queue;
std::mutex TreeNode::task_guard;
std::condition_variable TreeNode::task_signal;
int TreeNode::pending_task_count(0);
std::shared_ptr<BinaryRepresentation> TreeNode::binaryRepresentation = std::make_shared<BinaryRepresentation>();

TreeNode::TreeNode(bool useCloneOptimization)
{
	this->useCloneOptimization = useCloneOptimization;
}

TreeNode::~TreeNode()
{
}

void TreeNode::recurseOnClonedItemset(const std::shared_ptr<Itemset>& currentItemset, unsigned int i, std::vector<std::shared_ptr<Itemset>>& graph_mt)
{
	if (i < currentItemset->getItemCount())
	{
		std::shared_ptr<Item> item = currentItemset->getItem(i);

		// test if current item contains an original for all its items
		if (item->isAnOriginal())
		{
			for (unsigned int j = 0, cloneCount = item->getCloneCount(); j < cloneCount; j++)
			{
				// get clone index for current itemset
				std::shared_ptr<Item> clone = item->getClone(j);

				// call copy constructor to make a new copy of the itemset
				std::shared_ptr<Itemset> clonedItemset = std::make_shared<Itemset>(currentItemset.get());
				clonedItemset->replaceItem(i, clone);

				graph_mt.push_back(clonedItemset);

				for(unsigned int j = i; j < clonedItemset->getItemCount(); j++)
					recurseOnClonedItemset(clonedItemset, j, graph_mt);
			}
		}
	}
}

void TreeNode::updateListsFromToTraverse(const std::vector<std::shared_ptr<Itemset>>& toTraverse,
										       std::vector<std::shared_ptr<Itemset>>& maxClique, 											
											   std::vector<std::shared_ptr<Itemset>>& toExplore,
											   std::vector<std::shared_ptr<Itemset>>& graph_mt)
{
	maxClique.clear();
	toExplore.clear();

	// results of cumulated combined items / must be declared outside of the loop
	std::shared_ptr<Itemset> previousItem;
	// loop on toTraverse list and build maxClique and toExplore lists
	for (auto it = toTraverse.begin(); it != toTraverse.end(); it++)
	{
		//Itemset& currentItem = const_cast<Itemset&>(*it);
		std::shared_ptr<Itemset> currentItemset = (*it);
		unsigned int disjSup = this->binaryRepresentation->computeDisjonctifSupport(currentItemset);
		if (disjSup == this->binaryRepresentation->getObjectCount())
		{
			// we have a minimal transversal
			graph_mt.push_back(currentItemset);

			if (this->useCloneOptimization)
			{
				for (unsigned int i = 0; i < currentItemset->getItemCount(); i++)
					recurseOnClonedItemset(currentItemset, i, graph_mt);
			}
		}
		else
		{
			if ((currentItemset == (*toTraverse.begin())) && currentItemset->itemset.size() == 1)
			{
				// must be the 1st element with only one element
				previousItem = currentItemset;
				maxClique.push_back(currentItemset);
			}
			else
			{
				// we can combine with previous element / make a union on 2 elements
				std::shared_ptr<Itemset> combinedItem;
				if(previousItem)
					combinedItem = Itemset::combineItemset(previousItem, currentItemset);
				else 
					combinedItem = currentItemset;
				unsigned int disjSup = this->binaryRepresentation->computeDisjonctifSupport(combinedItem);
				if (disjSup != this->binaryRepresentation->getObjectCount())
				{
					previousItem = combinedItem;
					maxClique.push_back(currentItemset);
				}
				else
				{
					toExplore.push_back(currentItemset);
				}
			}
		}
	}
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------- //

std::vector<std::shared_ptr<Itemset>> TreeNode::computeMinimalTransversals_task(const std::vector<std::shared_ptr<Itemset>>& toTraverse)
{
	// ## START TASK ##

	// test trivial case
	if (toTraverse.empty())
		return std::vector<std::shared_ptr<Itemset>>();

	// contains list of itemsets that will be combined to the candidates
	std::vector<std::shared_ptr<Itemset>> maxClique;
	// contains list of itemsets that are candidates
	std::vector<std::shared_ptr<Itemset>> toExplore;
	// contains the final minimal transverals for this node
	std::vector<std::shared_ptr<Itemset>> graph_mt;
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
		std::vector<std::shared_ptr<Itemset>> combinedItemsetList = toExplore;
		combinedItemsetList.insert(combinedItemsetList.end(), maxClique.begin(), maxClique.end());

		// loop on candidates from toExplore list only
		for (unsigned int i = 0; i < lastIndexToTest; i++)
		{
			// build newTraverse list
			std::vector<std::shared_ptr<Itemset>> newToTraverse;
			std::shared_ptr<Itemset> toCombinedLeft = combinedItemsetList[i];
			// combine each element between [0, lastIndexToTest] with the entire combined itemset list
			for (unsigned int j = i + 1; j < combinedItemsetList.size(); j++)
			{
				assert(j < combinedItemsetList.size());
				std::shared_ptr<Itemset> toCombinedRight = combinedItemsetList[j];
				std::shared_ptr<Itemset> combinedItemset = Itemset::combineItemset(toCombinedLeft, toCombinedRight);

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

std::vector<std::shared_ptr<Itemset>> TreeNode::computeMinimalTransversals(std::vector<std::shared_ptr<Itemset>>& toTraverse)
{
	// ## START system ##

	std::vector<std::shared_ptr<Itemset>> final_mt;
	// emit initial task
	auto task = std::async(std::launch::deferred, &TreeNode::computeMinimalTransversals_task, this, std::move(toTraverse));

	// ## SPAWN task ##
	{
		const std::lock_guard<std::mutex> lock(task_guard);
		task_queue.emplace_back(std::move(task));
		++pending_task_count;
	}

	// launch processing units
	std::list<std::future<std::vector<std::shared_ptr<Itemset>>>> units;
	const unsigned int thead_multiplicator = 1;
	for (auto n = std::thread::hardware_concurrency() * thead_multiplicator; --n;)
	{
		units.emplace_back(std::async(std::launch::async, [n]()
		{
			// ## LAUNCH task ##
			std::vector<std::shared_ptr<Itemset>> result_mt;

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
						std::vector<std::shared_ptr<Itemset>> mt = task.get();
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
		std::vector<std::shared_ptr<Itemset>> result = unit.get();
		{
			const std::lock_guard<std::mutex> lock(output_guard);
			std::copy(result.begin(), result.end(), std::back_inserter(final_mt));
		}
	}
	return final_mt;
}
