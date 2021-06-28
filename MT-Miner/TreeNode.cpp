#include "TreeNode.h"

//#define TRACE

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
// synchro stuff for tasks blocking
std::mutex TreeNode::memory_guard;
std::condition_variable TreeNode::memory_signal;
std::atomic_uint TreeNode::pending_memory_task_count(0);

std::atomic_uint TreeNode::pending_task_count(0);
std::atomic_uint TreeNode::max_pending_task_count(MAX_PENDING_TASKS_START);
std::atomic_bool TreeNode::pending_task_checker(true);

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

	Item* item = itemset->getItem(iItem);

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
				std::cout << "new minimal traverse found from clone " << clonedItemset->toString() << "kk" << std::endl;
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

void TreeNode::updateListsFromToTraverse(std::vector<std::shared_ptr<Itemset>>&& toTraverse, std::deque<std::shared_ptr<Itemset>>&& maxClique, std::deque<std::shared_ptr<Itemset>>&& toExplore)
{
	assert(maxClique.empty());
	assert(toExplore.empty());

	// store object count for optimization
	unsigned int objectCount = this->binaryRepresentation->getObjectCount();
	
	// results of cumulated combined items / must be declared outside of the loop
	Itemset cumulatedItemset;	

	// loop on toTraverse list and build maxClique and toExplore lists
	for_each(toTraverse.begin(), toTraverse.end(), [&](auto crtItemset) {

		// Compute disjunctive support for each itemset of toTraverse list
		//	if disjunctive support is equal to object count --> add the itemset to graphMt list (then process its clones)
		unsigned int support = crtItemset->getSupport();		

		// check if itemset is a minimal transverse depending on disjunctive or consjonctive (with dual) method
		bool isMinimalTransverse = false;
		if (Itemset::itemsetType == Itemset::ItemsetType::DISJUNCTIVE)
			//isMinimalTransverse = (support == objectCount);
			isMinimalTransverse = ((100 * support) >= (objectCount * TreeNode::threshold));
		else
			//isMinimalTransverse = (support == 0);
			isMinimalTransverse = ((100 * support) <= (objectCount * (100 - TreeNode::threshold)));
		
		if (isMinimalTransverse)
		{
#ifdef ISESSENTIAL_ON_TOEXPLORE
			bool isEssential = Itemset::computeIsEssential(crtItemset);
			if (isEssential)
#endif
			{
				// lock thread and add minimal transverse
				if (!only_minimal || minimalMt >= MAX_MINIMAL_TRAVERSE_SIZE || crtItemset->getItemCount() <= minimalMt)
				{
					{
						const std::lock_guard<std::mutex> guard(task_guard);
						this->minimalTransverse.push_back(crtItemset);
#ifdef TRACE
						std::cout << "new minimal traverse found " << crtItemset->toString() << std::endl;
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
		else
		{
			// combine itemset one by one from toTraverse list as combine itemset
			// if disjunctive support for combined itemset is equal to object count --> add the itemset to toExplore list
			//	if not --> add the itemset to maxClique list

			// if current itemset is the 1st one, store it into a previous itemset variable and use it later for computing combined itemsets
			if ((crtItemset == *toTraverse.begin()) && crtItemset->getItemCount() == 1)
			//if(!cumulatedItemset.getItemCount())
			{
				// must be the 1st element with only one element
				//Itemset::copyRightIntoLeft(cumulatedItemset, crtItemset);
				Itemset* itemsetPtr = crtItemset.get();
				cumulatedItemset.combineItemset(itemsetPtr);
				maxClique.emplace_back(crtItemset);
#ifdef TRACE
				std::cout << "add item to maxClique list " << crtItemset->toString() << std::endl;
#endif
			}
			else
			{
				// test support and add itemset in maxClique or toExplore list
				unsigned int support = Itemset::computeSupport(cumulatedItemset, crtItemset);

				bool isMaxClique = false;
				if (Itemset::itemsetType == Itemset::ItemsetType::DISJUNCTIVE)
					isMaxClique = (support != objectCount);
				else
					isMaxClique = (support != 0);

				if (isMaxClique)
				{
					//Itemset::combineRightIntoLeft(cumulatedItemset, crtItemset);
					Itemset* itemsetPtr = crtItemset.get();
					cumulatedItemset.combineItemset(itemsetPtr);
					maxClique.emplace_back(crtItemset);
#ifdef TRACE
					std::cout << "add item to maxClique list " << crtItemset->toString() << std::endl;
#endif
				}
				else
				{
#ifdef ISESSENTIAL_ON_TOEXPLORE
					bool isEssential = Itemset::computeIsEssential(crtItemset);
					if (isEssential)
#endif
					{
						toExplore.emplace_back(crtItemset);
					}
#ifdef TRACE
					std::cout << "add item to toExplore list " << crtItemset->toString() << std::endl;
#endif
				}
			}
		}
	});
}

void TreeNode::addTaskIntoQueue(std::vector<std::shared_ptr<Itemset>>&& toTraverse)
{
	assert(!toTraverse.empty());

	// emit task
	auto subtask = std::async(std::launch::deferred, &TreeNode::computeMinimalTransversals_task, this, std::move(toTraverse));

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

void TreeNode::computeMinimalTransversals_task(std::vector<std::shared_ptr<Itemset>>&& toTraverse)
{
	// ## START TASK ##
	//std::cout << "\nNew task with itemset list : ";
	//for_each(toTraverse.begin(), toTraverse.end(), [&](const std::shared_ptr<Itemset> elt) { std::cout << elt->toString(), "\n"; });
	//std::cout << std::endl;

	// test trivial case
	if (toTraverse.empty())
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
		//std::cout << "computeMinimalTransversals_task " << toTraverse.size() << std::endl;

		// contains list of itemsets that will be combined to the candidates, the largest space in which is not possible to find minimal transversals
		std::deque<std::shared_ptr<Itemset>> maxClique;
		// contains list of itemsets that are candidates
		std::deque<std::shared_ptr<Itemset>> toExplore;

		// !!! to reserve maxClique and fit/pack (set capacity à la taille correcte, voir les fonctions)
		// !!! to reserve toExplore and fit/pack
		
		// push elements from toTraverse into maxClique, toExplore or minimal transverse
		// !!! virer maxClique et toExplore
		// !!! garder toTraverse et le trier
		this->updateListsFromToTraverse(std::move(toTraverse), std::move(maxClique), std::move(toExplore));

		// we don't need toTraverse anymore, remove references		
		toTraverse.clear();

#ifdef TRACE
		std::cout << "updateListsFromToTraverse, toExplore size : " << toExplore.size() << ", maxclique size : " << maxClique.size() << std::endl;
		std::cout << "toExplore size : ";
		for_each(toExplore.begin(), toExplore.end(), [&](const std::shared_ptr<Itemset> elt) { std::cout << elt->toString(); });
		std::cout << std::endl;
#endif

		if (toExplore.empty())
		{
			//const std::lock_guard<std::mutex> lock(task_guard);
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
				
			// store toExploreList max index
			unsigned int lastIndexToTest = static_cast<unsigned int>(toExplore.size());
			// move toExplore (left part) with maxClique list (right part) into a toExplore list
			toExplore.insert(toExplore.end(), maxClique.begin(), maxClique.end());
			// we don't need maxClique anymore, remove references		
			maxClique.clear();

			// combine each element between [0, lastIndexToTest] with the entire combined itemset list
			// loop on candidate itemset from initial toExplore list
			for (unsigned int i = 0; i < lastIndexToTest; i++)
			{
				std::shared_ptr<Itemset> toCombinedLeft = toExplore.front();
				toExplore.pop_front();

				if (only_minimal && toCombinedLeft->getItemCount() > minimalMt)
				{
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
					for_each(toExplore.begin(), toExplore.end(), [&newToTraverse, &toCombinedLeft](auto toCombinedRight) {

						if (!toCombinedRight->containsAClone())
						{
#ifndef ISESSENTIAL_ON_TOEXPLORE
							bool isEssential = Itemset::computeIsEssential(toCombinedLeft, toCombinedRight);
							if (isEssential)
#endif
							{
								// combine toCombinedRight into toCombinedLeft							
								std::shared_ptr<Itemset> newItemset = std::make_shared<Itemset>(toCombinedLeft);
								newItemset->combineItemset(toCombinedRight.get());
								//std::cout << "this combined itemset is essential, added to new toTraverse list" << std::endl;
								// this is a candidate for next iteration
								newToTraverse.push_back(newItemset);
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
					//std::cout << "newToTraverse " << newToTraverse.size() << std::endl;

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

					// pending task
					addTaskIntoQueue(std::move(newToTraverse));
				}
			}
			toExplore.clear();
		}
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
