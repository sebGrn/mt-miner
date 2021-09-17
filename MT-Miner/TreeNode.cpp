#include "TreeNode.h"

#define TRACE

#define MAX_PENDING_TASKS_START 1000
#define MAX_MINIMAL_TRAVERSE_SIZE 9999

std::atomic_ullong TreeNode::nbTotalMt(0);
std::atomic_ullong TreeNode::minimalMt(MAX_MINIMAL_TRAVERSE_SIZE);

std::atomic_bool TreeNode::only_minimal(true);
std::atomic_uint TreeNode::threshold(1);

// to avoid interleaved outputs
std::mutex TreeNode::output_guard;
// tasks queue to be process by thread
std::deque<std::future<void>> TreeNode::task_queue;
// synchro stuff for process units and tasks
std::mutex TreeNode::task_guard;
std::condition_variable TreeNode::task_signal;
std::mutex TreeNode::trace_guard;
// synchro stuff for tasks blocking
std::mutex TreeNode::memory_guard;
std::condition_variable TreeNode::memory_signal;
std::atomic_uint TreeNode::pending_memory_task_count(0);

std::atomic_uint TreeNode::pending_task_count(0);
std::atomic_uint TreeNode::max_pending_task_count(MAX_PENDING_TASKS_START);
std::atomic_bool TreeNode::pending_task_checker(true);

std::atomic_uint TreeNode::cpt(0);

std::shared_ptr<BinaryRepresentation> TreeNode::binaryRepresentation = std::make_shared<BinaryRepresentation>();

TreeNode::TreeNode(bool useCloneOptimization, bool only_minimal, float threshold)
{
	this->useCloneOptimization = useCloneOptimization;
	TreeNode::only_minimal = only_minimal;
	TreeNode::threshold = static_cast<unsigned int>(threshold * 100.0f);
}

TreeNode::~TreeNode()
{
}

void TreeNode::recurseOnClonedItemset(std::shared_ptr<Itemset> itemset, unsigned int iItem)
{
	assert(iItem < itemset->getItemCount());

	Item* item = Itemset::getItem(itemset, iItem);

	// test if current item contains an original for all its items
	if (item->isAnOriginal())
	{
		// item is an original
		// create a new itemset by replacing original with its clone and update graph mt list
		// then recurse on new itemset
		for (unsigned int j = 0, cloneCount = item->getCloneCount(); j < cloneCount; j++)
		{
			// get clone index for current itemset
			Item* clone = item->getClone(j);

			// make a copy of currentItemset and replace ith item by clone item
			std::shared_ptr<Itemset> clonedItemset = itemset->createAndReplaceItem(iItem, clone);
			{
				// lock thread and add minimal transverse			
				const std::lock_guard<std::mutex> minimalTransverse_guard(task_guard);
				this->minimalTransverse.push_back(clonedItemset);
#ifdef TRACE
				{
					const std::lock_guard<std::mutex> guard(trace_guard);
					std::cout << "new minimal traverse found from clone " << clonedItemset->toString() << "kk" << std::endl;
				}
#endif
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

bool TreeNode::isMinimalTrasverse(const std::shared_ptr<Itemset>& itemset) const
{
	// store object count for optimization
	unsigned int objectCount = this->binaryRepresentation->getObjectCount();

	// Compute disjunctive support for each itemset of toTraverse list
	//	if disjunctive support is equal to object count --> add the itemset to graphMt list (then process its clones)
	unsigned int support = itemset->getSupport();

	// check if itemset is a minimal transverse depending on disjunctive or consjonctive (with dual) method
	bool isMinimalTransverse = false;
	if (Itemset::itemsetType == Itemset::ItemsetType::DISJUNCTIVE)
		isMinimalTransverse = ((100 * support) >= (objectCount * TreeNode::threshold));
	else
		isMinimalTransverse = ((100 * support) <= (objectCount * (100 - TreeNode::threshold)));

	return isMinimalTransverse;
}

void TreeNode::updateMinimalTraverseList(const std::shared_ptr<Itemset>& crtItemset)
{
	// lock thread and add minimal transverse
	if (!only_minimal || minimalMt >= MAX_MINIMAL_TRAVERSE_SIZE || crtItemset->getItemCount() <= minimalMt)
	{
#ifdef ISESSENTIAL_ON_TOEXPLORE
		bool isEssential = Itemset::computeIsEssential(crtItemset, true);
		if (isEssential)
#endif
		{
			{
				const std::lock_guard<std::mutex> guard(task_guard);
				this->minimalTransverse.push_back(crtItemset);
#ifdef TRACE
				{
					const std::lock_guard<std::mutex> guard(trace_guard);
					std::cout << "new minimal traverse found " << crtItemset->toString() << std::endl;
				}
#endif
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
	}
}

bool TreeNode::isCandidateForMaxClique(const Itemset& cumulatedItemset, const std::shared_ptr<Itemset>& crtItemset) const
{
	//removeCandidate = false;
	// store object count for optimization
	unsigned int objectCount = this->binaryRepresentation->getObjectCount();

	// test support and add itemset in maxClique or toExplore list
	unsigned int support = Itemset::computeSupport(cumulatedItemset, crtItemset);

	// si support 715 == support 71 ou support 75
	//if (support == cumulatedItemset.getSupport() || support == crtItemset->getSupport())
	//{
	//	removeCandidate = true;
	//	return false;
	//}

	bool isMaxClique = false;
	if (Itemset::itemsetType == Itemset::ItemsetType::DISJUNCTIVE)
		isMaxClique = (support != objectCount);
	else
		isMaxClique = (support != 0);

#ifdef TRACE
	{
		const std::lock_guard<std::mutex> guard(trace_guard);
		if (isMaxClique)
			std::cout << "add item to maxClique list " << crtItemset->toString() << std::endl;
	}
#endif
	
	return isMaxClique;
}

void TreeNode::generateCandidates(std::vector<std::shared_ptr<Itemset>>&& toTraverse, std::vector<std::shared_ptr<Itemset>>&& toExplore, unsigned int& toExplore_MaxClique_Index)
{	
	// store object count for optimization
	unsigned int objectCount = this->binaryRepresentation->getObjectCount();
	
	// results of cumulated combined items / must be declared outside of the loop
	Itemset cumulatedItemset;

	// toExplore list will contain all toExplore itemset on [0, toExplore_MaxClique_Index] and maxClique itemset on [toExplore_MaxClique_Index, toExplore.size]
	toExplore_MaxClique_Index = 0;
	toExplore.reserve(toTraverse.size());

	// loop on toTraverse list and sort all itemset between minimal traverse, toExplore or maxClique candidates
	for (unsigned int i = 0; i < toTraverse.size(); i++)
	{
		auto crtItemset = toTraverse[i];

		// test if current itemset is a minimal traverse
		if (isMinimalTrasverse(crtItemset))
		{
			updateMinimalTraverseList(crtItemset);
		}
		else
		{
			if (i == 0)
			{
#ifdef TRACE
				{
					const std::lock_guard<std::mutex> guard(trace_guard);
					std::cout << "add item to maxClique list " << crtItemset->toString() << std::endl;
				}
#endif
				// crtItemset as maxClique
				toExplore.push_back(crtItemset);
				// combine current itemset into cumulated itemset
				Itemset* itemsetPtr = crtItemset.get();
				cumulatedItemset.combineItemset(itemsetPtr);
			}
			else
			{
/*
				// if (support 715 == support 71) ou (support 715 == support 75), then 715 will not be a candidate
				unsigned int support = Itemset::computeSupport(cumulatedItemset, crtItemset);
				if (support == cumulatedItemset.getSupport() || support == crtItemset->getSupport())
				{
#ifdef TRACE					
					{
						const std::lock_guard<std::mutex> guard(trace_guard);
						std::cout << crtItemset->toString() << " combined with " << cumulatedItemset.toString() << " will never be a candidate" << std::endl;
					}
#endif
				}
*/
				if(isCandidateForMaxClique(cumulatedItemset, crtItemset))
				{
					// add as max clique
					Itemset* itemsetPtr = crtItemset.get();
					cumulatedItemset.combineItemset(itemsetPtr);
					toExplore.push_back(crtItemset);
				}
				else
				{
#ifdef ISESSENTIAL_ON_TOEXPLORE
					bool isEssential = Itemset::computeIsEssential(crtItemset);
					if (isEssential)
#endif
					{
#ifdef TRACE
						{
							const std::lock_guard<std::mutex> guard(trace_guard);
							std::cout << "add item to toExplore list " << crtItemset->toString() << std::endl;
						}
#endif
						// add itemset as to explore
						toExplore.insert(toExplore.begin() + toExplore_MaxClique_Index, crtItemset);
						toExplore_MaxClique_Index++;
					}
				}
			}
		}
	}




/*
	for (unsigned int i = 0; i < toTraverse.size(); i++)
	{
		auto crtItemset = toTraverse[i];

		// test if current itemset is a minimal traverse
		if (isMinimalTrasverse(crtItemset))
		{
			updateMinimalTraverseList(crtItemset);
		}
		else
		{
			// test if current itemset is a maxClique candidate
			bool removeCandidate = false;
			if ((i == 0 && crtItemset->getItemCount() == 1) || isCandidateForMaxClique(cumulatedItemset, crtItemset, removeCandidate))
			{
				if (removeCandidate)
				{
#ifdef TRACE
					// do not add this candidate into maxClique or toExplore
					{
						const std::lock_guard<std::mutex> guard(trace_guard);
						std::cout << "this itemset is not a candidate in toExplore or maxClique : "  << cumulatedItemset.toString() << " U " << crtItemset->toString() << std::endl;
					}
#endif
					continue;
				}

				// add current itemset as maxClique, ie push the itemset at the end of toExplore 
				Itemset* itemsetPtr = crtItemset.get();
				cumulatedItemset.combineItemset(itemsetPtr);
				toExplore.push_back(crtItemset);
#ifdef TRACE
				{
					const std::lock_guard<std::mutex> guard(trace_guard);
					std::cout << "add item to maxClique list " << crtItemset->toString() << std::endl;
				}
#endif
			}
			else
			{
				// add current itemset as candidate as toExplore
#ifdef ISESSENTIAL_ON_TOEXPLORE
				bool isEssential = Itemset::computeIsEssential(crtItemset);
				if (isEssential)
#endif
				{
					toExplore.insert(toExplore.begin() + toExplore_MaxClique_Index, crtItemset);
					toExplore_MaxClique_Index++;
				}
#ifdef TRACE
				{
					const std::lock_guard<std::mutex> guard(trace_guard);
					std::cout << "add item to toExplore list " << crtItemset->toString() << std::endl;
				}
#endif
			}
		}
	}
#ifdef TRACE
	{
		const std::lock_guard<std::mutex> guard(trace_guard);
		std::cout << "sortItemsetFromToTraverse, toExplore size : " << toExplore.size() << ", maxClique_Index : " << toExplore.size() - toExplore_MaxClique_Index << std::endl;
	}
#endif
*/
}

void TreeNode::addTaskIntoQueue(std::vector<std::shared_ptr<Itemset>>&& toTraverse)
{
	assert(!toTraverse.empty());

	// sort itemset
	std::vector<std::shared_ptr<Itemset>> toExplore;
	unsigned int toExplore_MaxClique_Index = 0;
	this->generateCandidates(std::move(toTraverse), std::move(toExplore), toExplore_MaxClique_Index);
	toTraverse.clear();

	// emit task
	auto subtask = std::async(std::launch::deferred, &TreeNode::computeMinimalTransversals_task, this, std::move(toExplore), toExplore_MaxClique_Index);

	// ## SPAWN TASK ##
	{
		// !!! ne pas ajouter la tâche dans le tableau systématiquement
		// !!! donner une taille max à task_queue avec un atomic
		// !!! définir un ID pour cq tâche, pour faire en sorte qu'une tâche parente ne soit pas bloquée par ses tâches filles
		// !!! ajouter memory_signal de type condition_variable_any
		// !!! utiliser http://www.cplusplus.com/reference/condition_variable/condition_variable_any/wait/
		// !!! memory_signal.wait(lock, fct)
		// !!! laisser toujours une tâche en cours, si la tâche est une feuille de l'arbre, on la bloque pas, on bloque ses frères

		// lock to add task into task_queue
		//std::cout << "create a new task" << std::endl;
		std::unique_lock<std::mutex> lock(task_guard);
		task_queue.emplace_back(std::move(subtask));
		++pending_task_count;
		lock.unlock();
	}

	// be sure at least one unit is awaken
	task_signal.notify_one();

	// modify delay from 1 to 100 to see idle behaviour
	//std::this_thread::sleep_for(std::chrono::milliseconds(1));
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------- //

void TreeNode::computeMinimalTransversals_task(std::vector<std::shared_ptr<Itemset>>&& toExplore, unsigned int toExplore_MaxClique_Index)
{
	// ## START TASK ##
	//std::cout << "\nNew task with itemset list : ";
	//for_each(toTraverse.begin(), toTraverse.end(), [&](const std::shared_ptr<Itemset> elt) { std::cout << elt->toString(), "\n"; });
	//std::cout << std::endl;

	// test trivial case
	if (toExplore.empty() || toExplore_MaxClique_Index == 0)
	{
		const std::lock_guard<std::mutex> lock(task_guard);
		//std::cout << "toExplore is empty, unlock a task, pending memory task : " << pending_memory_task_count << std::endl;
		if (pending_memory_task_count)
		{
			memory_signal.notify_one();
			pending_memory_task_count--;
		}
		else
		{
			memory_signal.notify_all();
		}
	}
	else
	{
		// build new toTraverse list and explore next branch
		// combine each element between [0, toExplore_MaxClique_Index] with the entire combined itemset list
		for (unsigned int i = toExplore_MaxClique_Index; i--; )
		{
			std::shared_ptr<Itemset> toCombinedLeft = toExplore.front();
			toExplore.erase(toExplore.begin());

			if (only_minimal && toCombinedLeft->getItemCount() > minimalMt)
			{
				// we cut the branches here
				// this itemset is bigger than the minimal size of found minimal traverse
				// we dont need to explore this one
				break;
			}
				
			// build newTraverse list, reserve with max possible size
			std::vector<std::shared_ptr<Itemset>> newToTraverse;
			newToTraverse.reserve(toExplore.size());
			try
			{
				// loop on next candidate itemset
				for_each(toExplore.begin(), toExplore.end(), [&newToTraverse, &toCombinedLeft](std::shared_ptr<Itemset>& toCombinedRight) {

					if (!toCombinedRight->containsAClone())
					{
						if(Itemset::isEssentialRapid(toCombinedLeft, toCombinedRight))
						{
#ifndef ISESSENTIAL_ON_TOEXPLORE
							if (Itemset::computeIsEssential(toCombinedLeft, toCombinedRight))
#endif
							{
								/// combination of toCombinedRight & toCombinedRight is essential
								/// we have to test if it will be usefull

								/// 71 75 ==> 715
								/// 71 76 ==> 716

								/// --> new task --> 715

								/// TEST IF 715 IS USEFULL BEFORE CREATING NEW TASK
								/// CREER UNE TACHE POUR LES ELEMENTS DE TO EXPLORE


								// combine toCombinedRight into toCombinedLeft
								std::shared_ptr<Itemset> newItemset = std::make_shared<Itemset>(toCombinedLeft);
								newItemset->combineItemset(toCombinedRight.get());

								//std::vector<std::shared_ptr<Itemset>> toExplore;
								//unsigned int toExplore_MaxClique_Index = 0;
								//this->sortItemsetFromToTraverse(newItemset, std::move(toExplore), toExplore_MaxClique_Index);

								//toTraverse.clear();
								// this is a candidate for next iteration
								newToTraverse.push_back(newItemset);

#ifdef _DEBUG
								{
									std::cout << "create new itemset for " << newItemset->toString() << std::endl;
								}
#endif // _DEBUG


							}
						}
					}
				});	
			}
			catch (std::exception& e)
			{
				std::cout << "during computeMinimalTransversals_task " << e.what() << std::endl;
			}

			// we can free toCombined
			toCombinedLeft.reset();

			// call process in the loop
			if (!newToTraverse.empty())
			{
				std::unique_lock<std::mutex> lock(memory_guard);
				//std::cout << "stored tasks in the list, waiting to be managed : " << pending_task_count << " - blocked tasks, waiting to be unlock : " << pending_memory_task_count << std::endl;
				
				// check if we have too much tasks waiting to be managed by the process units in the task queue
				// if yes, we stop the thread with a lock (condition variable), this thread will be unlock when a tasks will be finished by the process unit
				if (pending_task_count > max_pending_task_count)
				{
					// do not add the taks into the task queue
					// wait for other tasks to finish
					pending_memory_task_count++;						
					memory_signal.wait(lock);
				}

				// memory signal has been notified, we can add the task to task queue
				lock.unlock();

				cpt++;

				// pending task
				addTaskIntoQueue(std::move(newToTraverse));
			}
		}
		toExplore.clear();
	}
	
	// terminate task
	if (!--pending_task_count)
	{
		// ## EMIT SHUTDOWN SIGNAL ##
		// awake all idle units for auto-shutdown
		task_signal.notify_all();
	}
	// ## EMIT COMPLETE TASK ##
}

std::vector<std::shared_ptr<Itemset>> TreeNode::computeMinimalTransversals(std::vector<std::shared_ptr<Itemset>>&& toTraverse)
{
	// ## START system ##
	
	// lambda function called during minimal transverse computing
	auto ftr = std::async(std::launch::async, []() {

		while (pending_task_checker)
		{
			if (pending_task_count >= max_pending_task_count)
			{
				// change max task queue to avoid blocking
				max_pending_task_count++;

				memory_signal.notify_one();
				if(pending_memory_task_count)
					pending_memory_task_count--;
			}
			else 
			{
				if (max_pending_task_count >= MAX_PENDING_TASKS_START)
				{
					max_pending_task_count--;
				}
			}
		}
	});

	// toExplore will contains a copy of toTraverse itemsets excepts minimal transverse items
	// itemsets from [0 to toExplore_MaxClique_Index] are to explore
	// itemsets from [toExplore_MaxClique_Index to toExplore size] are max clique
	std::vector<std::shared_ptr<Itemset>> toExplore;
	unsigned int toExplore_MaxClique_Index = 0;
	this->generateCandidates(std::move(toTraverse), std::move(toExplore), toExplore_MaxClique_Index);
	toTraverse.clear();

	// emit initial task
	auto task = std::async(std::launch::deferred, &TreeNode::computeMinimalTransversals_task, this, std::move(toExplore), toExplore_MaxClique_Index);

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
			std::unique_lock<std::mutex> lock(task_guard);
			while (true)
			{
				if (!task_queue.empty())
				{
					// pick a task
					auto task = std::move(task_queue.front());
					task_queue.pop_front();

					// !!! notifie qu'une tâche a été dépilée --> on peut en rajouter une
					// !!! analyser la taille de la task_queue, si < seuil alors memory_task.notify_one()
					// !!! utiliser un compteur d'"impacts de tâche" qui compte la mémoire de cq tâche en fonction du nombre d'itemset

					lock.unlock(); // unlock while processing task
					{
						// process task
						task.wait();
					}
					lock.lock(); // reacquire lock
				
					//memory_signal.notify_one();
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
		}));
	}

	// wait for shutdown
	for (auto& unit : units)
	{
		unit.wait();
	}

	this->pending_task_checker = false;
	ftr.get();	

	return this->minimalTransverse;
}
