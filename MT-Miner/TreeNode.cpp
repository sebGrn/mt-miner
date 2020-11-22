#include "TreeNode.h"
#include "Logger.h"

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

void TreeNode::updateListsFromToTraverse(const std::vector<std::shared_ptr<Itemset>>& toTraverse,
										       std::vector<std::shared_ptr<Itemset>>& maxClique, 											
											   std::vector<std::shared_ptr<Itemset>>& toExplore,
											   std::vector<std::shared_ptr<Itemset>>& graph_mt)
{
	maxClique.clear();
	toExplore.clear();

	// store object count for optimization
	unsigned int objectCount = this->binaryRepresentation->getObjectCount();
	
	// results of cumulated combined items / must be declared outside of the loop
	std::shared_ptr<Itemset> previousItemset;

	// loop on toTraverse list and build maxClique and toExplore lists
	for (auto currentItemset_it = toTraverse.begin(); currentItemset_it != toTraverse.end(); currentItemset_it++)
	{
		// Compute disjunctive support for each itemset of toTraverse list
		//	if disjunctive support is equal to object count --> add the itemset to graphMt list (then process its clones)
		unsigned int disjSup = (*currentItemset_it)->getDisjunctifSupport();
		if (disjSup == objectCount)
		{
			// we have a minimal transversal
			graph_mt.push_back((*currentItemset_it));

			if (this->useCloneOptimization)
			{
				for (unsigned int i = 0, n = (*currentItemset_it)->getItemCount(); i < n; i++)
					(*currentItemset_it)->recurseOnClonedItemset(i, graph_mt);
			}
		} 
		else
		{
			// combine itemset one by one from toTraverse list as combine itemset
			// if disjunctive support for combined itemset is equal to object count --> add the itemset to toExplore list
			//	if not --> add the itemset to maxClique list

			// if current itemset is the 1st one, store it into a previous itemset variable and use it later for computing combined itemsets
			if (((*currentItemset_it) == (*toTraverse.begin())) && (*currentItemset_it)->getItemCount() == 1)
			{
				// must be the 1st element with only one element
				previousItemset = (*currentItemset_it);
				maxClique.push_back((*currentItemset_it));
			}
			else
			{
				// we can combine with previous element / make a union on 2 elements
				std::shared_ptr<Itemset> combinedItemset;
				if (previousItemset)
				{
					try
					{
						combinedItemset = std::make_shared<Itemset>(*previousItemset);
						combinedItemset->combineItemset((*currentItemset_it));
					}
					catch (std::exception& e)
					{
						std::cout << "combined to get lists " << e.what() << std::endl;
					}
				}
				else
				{
					combinedItemset = (*currentItemset_it);
				}

				if (combinedItemset)
				{
					unsigned int disjSup = combinedItemset->getDisjunctifSupport();
					if (disjSup != objectCount)
					{
						previousItemset = combinedItemset;
						maxClique.push_back((*currentItemset_it));
					}
					else
					{
						toExplore.push_back((*currentItemset_it));
					}
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

	// contains list of itemsets that will be combined to the candidates, the largest space in which is not possible to find minimal transversals
	std::vector<std::shared_ptr<Itemset>> maxClique;
	// contains list of itemsets that are candidates
	std::vector<std::shared_ptr<Itemset>> toExplore;
	// contains the final minimal transverals for this node
	std::vector<std::shared_ptr<Itemset>> graph_mt;
	// update lists from toTraverse
	this->updateListsFromToTraverse(toTraverse, maxClique, toExplore, graph_mt);
	
	//std::this_thread::sleep_for(std::chrono::milliseconds(100));

	//Logger::log("toExplore list", ItemsetListToString(toExplore), "\n");
	//Logger::log("maxClique list", ItemsetListToString(maxClique), "\n");
	// add json node for js visualisation
	//JsonTree::addJsonNode(toExplore);

	// build new toTraverse list and explore next branch
	if (!toExplore.empty())
	{
		// store toExploreList max index
		unsigned int lastIndexToTest = static_cast<unsigned int>(toExplore.size());
		// combine toExplore (left part) with maxClique list (right part) into a toExplore list
		toExplore.insert(toExplore.end(), maxClique.begin(), maxClique.end());

		// build newTraverse list
		std::vector<std::shared_ptr<Itemset>> newToTraverse;

		// combine each element between [0, lastIndexToTest] with the entire combined itemset list
		// loop on candidate itemset from initial toExplore list
		for (unsigned int i = 0; i < lastIndexToTest; i++)
		{
			//auto toCombinedLeft = toExplore[i];  
			auto toCombinedLeft = toExplore.front();
			toExplore.erase(toExplore.begin());

			// loop on next candidate itemset
			for (unsigned int j = 0; j < toExplore.size(); j++)
			{
				assert(j < toExplore.size());
				auto toCombinedRight = toExplore[j];
				
				try
				{
					// combine toCombinedRight into toCombinedLeft
					auto newItemset = std::make_shared<Itemset>(*toCombinedLeft);
					newItemset->combineItemset(toCombinedRight);

					if (!newItemset->containsAClone() && newItemset->computeIsEssential())
					{
						// this is a candidate, copy toCombinedLeft into newItemset
						//auto newItemset = std::make_shared<Itemset>(*toCombinedLeft);
						newToTraverse.push_back(newItemset);
					}
				}
				catch (std::exception& e)
				{
					std::cout << "combined to get newToTraverse " << e.what() << std::endl;
				}
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

				// new list has been managed by the thread, clear it
				newToTraverse.clear();
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

	//for_each(graph_mt.begin(), graph_mt.end(), [&](const std::shared_ptr<Itemset>& elt) { std::cout << elt->toString() << "\n"; });

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
	//for (auto n = 1; --n;)
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
						task.wait();
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
		unit.wait();
		std::vector<std::shared_ptr<Itemset>> result = unit.get();
		{
			const std::lock_guard<std::mutex> lock(output_guard);
			std::copy(result.begin(), result.end(), std::back_inserter(final_mt));
		}
	}
	return final_mt;
}
