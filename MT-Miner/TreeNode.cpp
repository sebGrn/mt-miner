#include "TreeNode.h"
#include "Logger.h"

std::atomic_ullong TreeNode::nbTotalChildren(0);
std::atomic_ullong TreeNode::nbTotalMt(0);
std::atomic_ullong TreeNode::minimalMt(9999);
std::atomic_uint TreeNode::nbThread(0);
std::atomic_uint TreeNode::cptSharedItemsets(0);

std::atomic_bool TreeNode::thread_nb1(false);
std::atomic_bool TreeNode::thread_nb2(false);
std::atomic_bool TreeNode::thread_nb3(false);
std::atomic_bool TreeNode::thread_nb4(false);

std::atomic_uint TreeNode::shared_toTraverse_index_start(0);
std::atomic_uint TreeNode::shared_toTraverse_index_end(0);

// to avoid interleaved outputs
std::mutex TreeNode::output_guard;
std::mutex TreeNode::print_guard;
std::mutex TreeNode::shared_toTraverseItemset_guard;
std::mutex TreeNode::shared_minimalTransverse_guard;
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
	//std::unique_lock<std::mutex> tt_lock(shared_minimalTransverse_guard);
	//{
	//	for (auto it = this->shared_toTraverse.begin(); it != this->shared_toTraverse.end(); it++)
	//	{
	//		if (*it)
	//		{
	//			delete* it;
	//			(*it) = nullptr;
	//		}
	//	}
	//	this->shared_toTraverse.clear();
	//}
	//tt_lock.unlock();

	//std::unique_lock<std::mutex> mt_lock(shared_minimalTransverse_guard);
	//{
	//	for (auto it = this->shared_minimalTransverse.begin(); it != this->shared_minimalTransverse.end(); it++)
	//	{
	//		if (*it)
	//		{
	//			delete* it;
	//			(*it) = nullptr;
	//		}
	//	}
	//	shared_minimalTransverse.clear();
	//}
	//mt_lock.unlock();
}

void TreeNode::recurseOnClonedItemset(Itemset* itemset, unsigned int iItem)
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
			Itemset* clonedItemset = itemset->createAndReplaceItem(iItem, clone);

			{
				const std::lock_guard<std::mutex> mt_lock(shared_minimalTransverse_guard);
				shared_minimalTransverse.push_back(clonedItemset);
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

/*void TreeNode::updateListsDependingOnProcessUnit(std::vector<Itemset*>& threadList, std::vector<Itemset*>& maxClique, std::vector<Itemset*>& toExplore)
{
	// store object count for optimization
	unsigned int objectCount = this->binaryRepresentation->getObjectCount();

	// results of cumulated combined items / must be declared outside of the loop
	Itemset cumulatedItemset;
	Itemset* crtItemset = nullptr;

	// loop on toTraverse list and build maxClique and toExplore lists
	//std::unique_lock<std::mutex> lock(shared_toTraverseItemset_guard);
	for (auto it = threadList.begin(); it != threadList.end(); it++)
	{
		crtItemset = (*it);
		//std::cout << crtItemset->toString() << std::endl;

		// Compute disjunctive support for each itemset of toTraverse list
		//	if disjunctive support is equal to object count --> add the itemset to graphMt list (then process its clones)
		unsigned int disjSup = crtItemset->getDisjunctifSupport();
		if (disjSup == objectCount)
		{
			//std::cout << "add MT" << std::endl;
			// we have a minimal transversal
			{
				const std::lock_guard<std::mutex> mt_lock(shared_minimalTransverse_guard);
				this->shared_minimalTransverse.push_back(crtItemset);
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
			if ((it == threadList.begin()) && crtItemset->getItemCount() == 1)
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
}*/

//void TreeNode::updateListsFromToTraverse(const std::vector<Itemset*>& toTraverse, std::vector<Itemset*>& maxClique, std::vector<Itemset*>& toExplore, std::vector<Itemset*>& graph_mt)
void TreeNode::updateListsFromToTraverse(std::vector<Itemset*>& maxClique, std::vector<Itemset*>& toExplore, unsigned int startIndex, unsigned int endIndex)
{
	assert(maxClique.empty());
	assert(toExplore.empty());

	//if(GetCurrentProcessorNumber() == thread_nb1)
	//	updateListsDependingOnProcessUnit(shared_toTraverse_itemset_thread1, maxClique, toExplore);
	//else if (GetCurrentProcessorNumber() == thread_nb2)
	//	updateListsDependingOnProcessUnit(shared_toTraverse_itemset_thread2, maxClique, toExplore);
	//else if (GetCurrentProcessorNumber() == thread_nb3)
	//	updateListsDependingOnProcessUnit(shared_toTraverse_itemset_thread3, maxClique, toExplore);
	//else if (GetCurrentProcessorNumber() == thread_nb4)
	//	updateListsDependingOnProcessUnit(shared_toTraverse_itemset_thread4, maxClique, toExplore);

	// store object count for optimization
	unsigned int objectCount = this->binaryRepresentation->getObjectCount();
	// results of cumulated combined items / must be declared outside of the loop
	Itemset cumulatedItemset;
	Itemset* crtItemset = nullptr;

	// loop on toTraverse list and build maxClique and toExplore lists
	std::unique_lock<std::mutex> lock(shared_toTraverseItemset_guard);
	{
		for (unsigned int i = startIndex; i < endIndex; i++)
		{
			crtItemset = this->shared_toTraverse[i];
			//std::cout << crtItemset->toString() << std::endl;

			// Compute disjunctive support for each itemset of toTraverse list
			//	if disjunctive support is equal to object count --> add the itemset to graphMt list (then process its clones)
			unsigned int disjSup = crtItemset->getDisjunctifSupport();
			if (disjSup == objectCount)
			{
				//std::cout << "add MT" << std::endl;
				// we have a minimal transversal
				{
					const std::lock_guard<std::mutex> mt_lock(shared_minimalTransverse_guard);
					this->shared_minimalTransverse.push_back(crtItemset);
				}
				// update info
				nbTotalMt++;
				if (crtItemset->getItemCount() < minimalMt)
					minimalMt = crtItemset->getItemCount();

				// manage clones
				if (this->useCloneOptimization)
				{
					for (unsigned int j = 0, n = crtItemset->getItemCount(); j < n; j++)
						this->recurseOnClonedItemset(crtItemset, j);
				}
			}
			else
			{
				// combine itemset one by one from toTraverse list as combine itemset
				// if disjunctive support for combined itemset is equal to object count --> add the itemset to toExplore list
				//	if not --> add the itemset to maxClique list
				// if current itemset is the 1st one, store it into a previous itemset variable and use it later for computing combined itemsets
				if ((i == startIndex) && crtItemset->getItemCount() == 1)
				{
					// must be the 1st element with only one element
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
	lock.unlock();
}

void TreeNode::computeMinimalTransversals_task_test(unsigned int startIndex, unsigned int endIndex)
{
	//setThreadIdentifier();
	shared_toTraverse_index_start = startIndex;
	shared_toTraverse_index_end = endIndex;

	// simulate work in progress
	// contains list of itemsets that will be combined to the candidates, the largest space in which is not possible to find minimal transversals
	std::vector<Itemset*> maxClique;
	// contains list of itemsets that are candidates
	std::vector<Itemset*> toExplore;
	// update lists from toTraverse, move itemset from toTraverse to maxClique, toExplore or graph_mt
	this->updateListsFromToTraverse(maxClique, toExplore, startIndex, endIndex);

	//std::unique_lock<std::mutex> lock(shared_toTraverseItemset_guard);
	//const std::lock_guard<std::mutex> lock(shared_toTraverseItemset_guard);
	//this->shared_toTraverse_itemset.clear();
	//lock.unlock();

	// build new toTraverse list and explore next branch
	if (!toExplore.empty())
	{
		// store toExploreList max index
		unsigned int lastIndexToTest = static_cast<unsigned int>(toExplore.size());
		// move toExplore (left part) with maxClique list (right part) into a toExplore list
		toExplore.insert(toExplore.end(), maxClique.begin(), maxClique.end());
		// maxClique can be empty
		maxClique.clear();

		unsigned int currentStartIndex = shared_toTraverse_index_end;

		// combine each element between [0, lastIndexToTest] with the entire combined itemset list
		// loop on candidate itemset from initial toExplore list
		for (unsigned int i = 0; i < lastIndexToTest; i++)
		{
			Itemset* toCombinedLeft = toExplore.front();
			toExplore.erase(toExplore.begin());

			unsigned int newStartIndex = currentStartIndex;

			// loop on next candidate itemset
			for (unsigned int j = 0; j < toExplore.size(); j++)
			{
				assert(j < toExplore.size());
				Itemset* toCombinedRight = toExplore[j];

				if (!(toCombinedLeft->containsAClone() || toCombinedRight->containsAClone()))
				{
					std::unique_lock<std::mutex> lock(shared_toTraverseItemset_guard);
					{
						Itemset* newItemset = nullptr;
						try
						{
							// DO NOT CREATE NEW ITEM, MODIFY ITEM IN SHARED MEMORY LIST
							// combine toCombinedRight into toCombinedLeft
							newItemset = this->shared_toTraverse[currentStartIndex];
							if (!newItemset)
							{
								newItemset = new Itemset(toCombinedLeft);
							}
							else
							{
								newItemset->removeAllItems();
								newItemset->combineItemset(toCombinedLeft);
							}
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
								//std::unique_lock<std::mutex> lock(shared_toTraverseItemset_guard);
								//const std::lock_guard<std::mutex> lock(shared_toTraverseItemset_guard);
								// this is a candidate, copy toCombinedLeft into newItemset
								{
									//std::unique_lock<std::mutex> lock(shared_toTraverseItemset_guard);
									this->shared_toTraverse[currentStartIndex] = newItemset;
									currentStartIndex++;
								}
								//newToTraverse.push_back(newItemset);
								//std::cout << "add to shared to traverse list " << newItemset->toString() << std::endl;
								//lock.unlock();

								//if (GetCurrentProcessorNumber() == thread_nb1)
								//	shared_toTraverse_itemset_thread1.push_back(newItemset);
								//else if (GetCurrentProcessorNumber() == thread_nb2)
								//	shared_toTraverse_itemset_thread2.push_back(newItemset);
								//else if (GetCurrentProcessorNumber() == thread_nb3)
								//	shared_toTraverse_itemset_thread3.push_back(newItemset);
								//else if (GetCurrentProcessorNumber() == thread_nb4)
								//	shared_toTraverse_itemset_thread4.push_back(newItemset);
							}
							//else
							//{
							//	delete newItemset;
							//	newItemset = nullptr;
							//}
						}
					}
					lock.unlock();
				}				
			}

			//delete toCombinedLeft;
			//toCombinedLeft = nullptr;


			//if (GetCurrentProcessorNumber() == thread_nb1 && (shared_toTraverse_itemset_thread1.empty()))
			//	return;
			//if (GetCurrentProcessorNumber() == thread_nb2 && (shared_toTraverse_itemset_thread2.empty()))
			//	return;
			//if (GetCurrentProcessorNumber() == thread_nb3 && (shared_toTraverse_itemset_thread3.empty()))
			//	return;
			//if (GetCurrentProcessorNumber() == thread_nb4 && (shared_toTraverse_itemset_thread4.empty()))
			//	return;
			
			if(currentStartIndex != newStartIndex)
			{
				//std::unique_lock<std::mutex> lock(shared_toTraverseItemset_guard);
				//const std::lock_guard<std::mutex> lock(shared_toTraverseItemset_guard);			
					// call process in the loop
				// emit task
				nbTotalChildren++;
				// call on the same node, it works because no class members are used except atomics
				shared_toTraverse_index_end = currentStartIndex;
				auto subtask = std::async(std::launch::deferred, &TreeNode::computeMinimalTransversals_task_test, this, newStartIndex, currentStartIndex);
				// ## SPAWN TASK ##
				{
					const std::lock_guard<std::mutex> lock(task_guard);
					task_queue.emplace_back(std::move(subtask));
					++pending_task_count;
				}
				task_signal.notify_one(); // be sure at least one unit is awaken

				// modify delay from 1 to 100 to see idle behaviour
				//std::this_thread::sleep_for(std::chrono::milliseconds(1000));
			}
		}


		//for (unsigned int m = 10; m--;) 
		//{
		//	newItemset = new Itemset();
			//newToTraverse.push_back(std::move(newItemset));
		//}
		//if (TreeNode::cptSharedItemsets < 100)
		//{
		//	TreeNode::cptSharedItemsets++;
		//}
		//else
		//{
		//	TreeNode::cptSharedItemsets = 0;
		//}

		//const std::lock_guard<std::mutex> lock(shared_itemset_guard);
		//if(shared_toTraverse_itemset[TreeNode::cptSharedItemsets])
		//	delete shared_toTraverse_itemset[TreeNode::cptSharedItemsets];
		//shared_toTraverse_itemset[TreeNode::cptSharedItemsets] = new Itemset();


		//this->itemsets_per_process[0] = newToTraverse;

		//std::this_thread::sleep_for(std::chrono::milliseconds(100));
		
		//if(newItemset)
		//	delete newItemset;

		//for (auto it = newToTraverse.begin(); it != newToTraverse.end(); it++) { (*it).reset();  }
		//newToTraverse.clear();
		
		//{
		//	//const std::lock_guard<std::mutex> lock(shared_itemsets_guard);
		//	//std::cout << shared_itemsets.size() << std::endl;
		//	//std::cout << TreeNode::cptSharedItemsets << std::endl;
		//}
		//{
		//	const std::lock_guard<std::mutex> lock(print_guard);
		//	Utils::printUsedMemoryForCrtProcess();
		//}
		/*
		// call on the same node, it works because no class members are used except atomics
		auto subtask = std::async(std::launch::deferred, &TreeNode::computeMinimalTransversals_task_test, this);

		// ## SPAWN TASK ##
		{
			const std::lock_guard<std::mutex> lock(task_guard);
			task_queue.emplace_back(std::move(subtask));
			++pending_task_count;
		}
		task_signal.notify_one(); // be sure at least one unit is awaken*/
	}

	// terminate task
	const std::lock_guard<std::mutex> lock_task(task_guard);
	if (!--pending_task_count)
	{
		// ## EMIT SHUTDOWN SIGNAL ##
		// awake all idle units for auto-shutdown
		task_signal.notify_all();
	}
	// ## EMIT COMPLETE TASK ##

	//for_each(graph_mt.begin(), graph_mt.end(), [&](const std::shared_ptr<Itemset>& elt) { std::cout << elt->toString() << "\n"; });

	//return shared_minimalTransverse;
}

void TreeNode::computeMinimalTransversals(std::vector<Itemset*>& final_mt, std::vector<Itemset*>& toTraverse)
{
	// ## START system ##	

	// get nb thread and fix it
	const unsigned int thead_multiplicator = 1;
	nbThread = std::thread::hardware_concurrency() * thead_multiplicator;
	const unsigned int nb_thread = 4;// std::thread::hardware_concurrency()* thead_multiplicator;
	std::cout << "nb thread " << nb_thread << std::endl;

	// 
	const unsigned int sharedListSize = 1000;
	//this->shared_toTraverse.clear();
	//this->shared_toTraverse.reserve(sharedListSize);
	//this->shared_toTraverse.resize(sharedListSize);
	//std::fill(this->shared_toTraverse.begin(), this->shared_toTraverse.end(), nullptr);
	//assert(toTraverse.size() < sharedListSize);
	//for (unsigned int i = 0; i != toTraverse.size(); i++)
	//	this->shared_toTraverse[i] = toTraverse[i];
	

	// copy totraverse to all shared lists
	//shared_toTraverse_itemset_thread1 = toTraverse;
	//shared_toTraverse_itemset_thread2 = toTraverse;
	//shared_toTraverse_itemset_thread3 = toTraverse;
	//shared_toTraverse_itemset_thread4 = toTraverse;

	//{
	//	std::unique_lock<std::mutex> lock(shared_toTraverseItemset_guard);
	//	for (auto it = toTraverse.begin(); it != toTraverse.end(); it++)
	//		shared_toTraverse_itemset.push_back(*it);
	//	lock.unlock();
	//}

	//this->shared_toTraverse_itemset.reserve(100);
	//for (unsigned int i = 100; i--; )
	//	this->shared_toTraverse_itemset[i] = nullptr;

	//std::vector<std::shared_ptr<Itemset>> itemsets;
	// emit initial task
	//auto task = std::async(std::launch::deferred, &TreeNode::computeMinimalTransversals_task, this, std::move(toTraverse));
	auto task = std::async(std::launch::deferred, &TreeNode::computeMinimalTransversals_task_test, this, 0, static_cast<unsigned int>(toTraverse.size()));

	// ## SPAWN task ##
	{
		const std::lock_guard<std::mutex> lock(task_guard);
		task_queue.emplace_back(std::move(task));
		++pending_task_count;
	}

	// launch processing units
	std::list<std::future<void>> units;
	for (auto n = nb_thread; --n;)
	{
		units.emplace_back(std::async(std::launch::async, [n]()
		{
			// ## LAUNCH task ##
			//std::vector<Itemset*> result_mt;
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
						//std::vector<Itemset*> mt = task.get();
						//if (mt.size() > result_mt.size())
						//	result_mt = mt;
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
		/*std::vector<Itemset*>&& result = unit.get();
		{
			const std::lock_guard<std::mutex> lock(output_guard);
			std::copy(result.begin(), result.end(), std::back_inserter(final_mt));
		}*/
	}
	{
		const std::lock_guard<std::mutex> mt_lock(shared_minimalTransverse_guard);
		final_mt = shared_minimalTransverse;
	}
}
