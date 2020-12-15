#include "TreeNode.h"
#include "Logger.h"

std::atomic_ullong TreeNode::nbTotalChildren(0);
std::atomic_ullong TreeNode::nbTotalMt(0);
std::atomic_ullong TreeNode::minimalMt(9999);

// to avoid interleaved outputs
std::mutex TreeNode::output_guard;
// synchro stuff
std::deque<std::future<void>> TreeNode::task_queue;
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

void TreeNode::recurseOnClonedItemset(std::shared_ptr<Itemset> itemset, unsigned int iItem)
{
	assert(iItem < itemset->getItemCount());

	std::shared_ptr<Item> item = itemset->getItem(iItem);

	// test if current item contains an original for all its items
	if (item->isAnOriginal())
	{
		// item is an original
		// create a new itemset by replacing original with its clone and update graph mt list
		// then recurse on new itemset
		for (unsigned int j = 0, cloneCount = item->getCloneCount(); j < cloneCount; j++)
		{
			// get clone index for current itemset
			std::shared_ptr<Item> clone = item->getClone(j);

			// make a copy of currentItemset and replace ith item by clone item
			std::shared_ptr<Itemset> clonedItemset = itemset->createAndReplaceItem(iItem, clone);

			// lock thread and add minimal transverse
			{
				const std::lock_guard<std::mutex> minimalTransverse_guard(task_guard);
				this->minimalTransverse.push_back(clonedItemset);
			}

			// update info
			nbTotalMt++;
			if (clonedItemset->getItemCount() < minimalMt)
				minimalMt = clonedItemset->getItemCount();

			// recurse on new cloned itemset to replace kth original by 
			for (unsigned int k = iItem, n = clonedItemset->getItemCount(); k < n; k++)
				recurseOnClonedItemset(clonedItemset, k);
		}
	}
}

void TreeNode::updateListsFromToTraverse(const std::vector<std::shared_ptr<Itemset>>& toTraverse, std::vector<std::shared_ptr<Itemset>>& maxClique, std::vector<std::shared_ptr<Itemset>>& toExplore)
{
	assert(maxClique.empty());
	assert(toExplore.empty());

	// store object count for optimization
	unsigned int objectCount = this->binaryRepresentation->getObjectCount();
	
	// results of cumulated combined items / must be declared outside of the loop
	Itemset cumulatedItemset;
	std::shared_ptr<Itemset> crtItemset;

	// loop on toTraverse list and build maxClique and toExplore lists
	for (auto it = toTraverse.begin(); it != toTraverse.end(); it++)
	{
		crtItemset = (*it);

		// Compute disjunctive support for each itemset of toTraverse list
		//	if disjunctive support is equal to object count --> add the itemset to graphMt list (then process its clones)
		unsigned int disjSup = crtItemset->getDisjunctifSupport();
		if (disjSup == objectCount)
		{
			// lock thread and add minimal transverse
			{
				const std::lock_guard<std::mutex> minimalTransverse_guard(task_guard);
				this->minimalTransverse.push_back(crtItemset);
			}

			// update info
			nbTotalMt++;
			if (crtItemset->getItemCount() < minimalMt)
				minimalMt = crtItemset->getItemCount();

			// manage clones
			if (this->useCloneOptimization)
			{
				for (unsigned int i = 0, n = crtItemset->getItemCount(); i < n; i++)
					this->recurseOnClonedItemset(crtItemset, i);
			}
		}
		else
		{
			// combine itemset one by one from toTraverse list as combine itemset
			// if disjunctive support for combined itemset is equal to object count --> add the itemset to toExplore list
			//	if not --> add the itemset to maxClique list

			// if current itemset is the 1st one, store it into a previous itemset variable and use it later for computing combined itemsets
			if ((it == toTraverse.begin()) && crtItemset->getItemCount() == 1)
			{
				// must be the 1st element with only one element
				//previousItemset = (*currentItemset_it);
				Itemset::copyRightIntoLeft(cumulatedItemset, crtItemset);
				maxClique.push_back(crtItemset);
			}
			else
			{
				// compute disjunctif support
				unsigned int disjSup = Itemset::computeDisjunctifSupport(cumulatedItemset, crtItemset);
				// test support and add itemset in maxClique or toExplore list
				if (disjSup != objectCount)
				{
					Itemset::combineRightIntoLeft(cumulatedItemset, crtItemset);
					maxClique.push_back(crtItemset);
				}
				else
				{
					toExplore.push_back(crtItemset);
				}
			}
		}
	}
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------- //

void TreeNode::computeMinimalTransversals_task(const std::vector<std::shared_ptr<Itemset>>& toTraverse)
{
	// ## START TASK ##

	//std::cout << "toTraverse size " << toTraverse.size() << std::endl;
	//SIZE_T used0 = Utils::printUsedMemoryForCrtProcess();

	// test trivial case
	if (!toTraverse.empty())
	{
		// contains list of itemsets that will be combined to the candidates, the largest space in which is not possible to find minimal transversals
		std::vector<std::shared_ptr<Itemset>> maxClique;
		// contains list of itemsets that are candidates
		std::vector<std::shared_ptr<Itemset>> toExplore;
		// update lists from toTraverse, move itemset from toTraverse to maxClique, toExplore or graph_mt
		this->updateListsFromToTraverse(toTraverse, maxClique, toExplore);
	

		// build new toTraverse list and explore next branch
		if (!toExplore.empty())
		{
			// store toExploreList max index
			unsigned int lastIndexToTest = static_cast<unsigned int>(toExplore.size());
			// move toExplore (left part) with maxClique list (right part) into a toExplore list
			toExplore.insert(toExplore.end(), maxClique.begin(), maxClique.end());
			// maxClique can be empty
			maxClique.clear();

			// combine each element between [0, lastIndexToTest] with the entire combined itemset list
			// loop on candidate itemset from initial toExplore list
			for (unsigned int i = 0; i < lastIndexToTest; i++)
			{
				std::shared_ptr<Itemset> toCombinedLeft = toExplore.front();
				toExplore.erase(toExplore.begin());

				// build newTraverse list
				std::vector<std::shared_ptr<Itemset>> newToTraverse;

				// loop on next candidate itemset
				for (unsigned int j = 0, n = toExplore.size(); j < n; j++)
				{
					assert(j < toExplore.size());
					std::shared_ptr<Itemset> toCombinedRight = toExplore[j];

					if (!(toCombinedLeft->containsAClone() || toCombinedRight->containsAClone()))
					{
						std::shared_ptr<Itemset> newItemset;
						try
						{
							// combine toCombinedRight into toCombinedLeft
							newItemset = std::make_shared<Itemset>(toCombinedLeft);							
						}
						catch (std::exception& e)
						{
							std::cout << "during computeMinimalTransversals_task " << e.what() << std::endl;
						}
						if (newItemset)
						{
							newItemset->combineItemset(toCombinedRight);
							if (newItemset->computeIsEssential())
							{
								// this is a candidate, copy toCombinedLeft into newItemset
								newToTraverse.push_back(newItemset);
							}
							else
							{
								//newItemset.reset();
							}
						}
					}
				}
				
				//toCombinedLeft.reset();
				
				// call process in the loop
				if (!newToTraverse.empty())
				{
					// emit task
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
					//std::this_thread::sleep_for(std::chrono::milliseconds(1000));

					// new list has been managed by the thread, clear it
					//for (auto it = newToTraverse.begin(); it != newToTraverse.end(); it++) { it->reset(); }
					//newToTraverse.clear();
				}
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

}

bool TreeNode::computeMinimalTransversals(std::vector<std::shared_ptr<Itemset>>& final_mt, std::vector<std::shared_ptr<Itemset>>& toTraverse)
{
	// ## START system ##

	// emit initial task
	auto task = std::async(std::launch::deferred, &TreeNode::computeMinimalTransversals_task, this, std::move(toTraverse));

	// ## SPAWN task ##
	{
		const std::lock_guard<std::mutex> lock(task_guard);
		task_queue.emplace_back(std::move(task));
		++pending_task_count;
	}

	// launch processing units
	std::list<std::future<void>> units;
	const unsigned int thead_multiplicator = 1;
	for (auto n = std::thread::hardware_concurrency() * thead_multiplicator; --n;)
	{
		units.emplace_back(std::async(std::launch::async, [n]()
		{
			// ## LAUNCH task ##
			//std::vector<std::shared_ptr<Itemset>> result_mt;

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
						//std::vector<std::shared_ptr<Itemset>>&& mt = task.get();
						//std::copy(mt.begin(), mt.end(), std::back_inserter(result_mt));
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
			//return result_mt;
		}));
	}

	// wait for shutdown
	for (auto& unit : units)
	{
		unit.wait();
		//std::vector<std::shared_ptr<Itemset>> result = unit.get();
		//{
		//	const std::lock_guard<std::mutex> lock(output_guard);
		//	std::copy(result.begin(), result.end(), std::back_inserter(final_mt));
		//}
	}

	final_mt = this->minimalTransverse;
	return true;
}
