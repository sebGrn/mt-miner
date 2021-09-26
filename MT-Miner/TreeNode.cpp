#include "TreeNode.h"

//#define TRACE

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

std::atomic_uint TreeNode::pending_task_count(0);

std::atomic_uint TreeNode::nbTaskCreated(0);

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
					std::cout << "--> new minimal traverse found from clone " << clonedItemset->toString() << std::endl;
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

	bool isMinimalTransverse = ((100 * support) >= (objectCount * TreeNode::threshold));

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
#else
		unsigned int indexToAdd = crtItemset->getLastItemAttributeIndex();
		bool isEssential = Itemset::computeIsEssential(crtItemset, indexToAdd);
		if (isEssential)

#endif
		{
			{
				const std::lock_guard<std::mutex> guard(task_guard);
				this->minimalTransverse.push_back(crtItemset);
#ifdef TRACE
				{
					const std::lock_guard<std::mutex> guard(trace_guard);
					std::cout << "--> new minimal traverse found " << crtItemset->toString() << std::endl;
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
	// store object count for optimization
	unsigned int objectCount = this->binaryRepresentation->getObjectCount();

	if (cumulatedItemset.getSupport() + crtItemset->getSupport() == objectCount)
		return false;

	// test support and add itemset in maxClique or toExplore list
	unsigned int support = Itemset::computeSupport(cumulatedItemset, crtItemset);

	if (support != objectCount)
		return true;
	else
		return false;
}

void TreeNode::generateCandidates(std::deque<std::shared_ptr<Itemset>>&& toTraverse, std::deque<std::shared_ptr<Itemset>>&& toExplore, std::vector<unsigned int>&& maxClique)
{	
	// results of cumulated combined items / must be declared outside of the loop
	Itemset cumulatedItemset;

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
			if ((i == 0) || isCandidateForMaxClique(cumulatedItemset, crtItemset))
			{
				// add crtItemset as maxClique
				unsigned int rightAttributeIndex = crtItemset->getLastItemAttributeIndex();
				maxClique.push_back(rightAttributeIndex);
				// combine current itemset into cumulated itemset for next round
				cumulatedItemset.combine(rightAttributeIndex);
#ifdef TRACE
				{
					const std::lock_guard<std::mutex> guard(trace_guard);
					std::cout << "add item to maxClique list " << crtItemset->toString() << std::endl;
				}
#endif
			}
			else
			{
#ifdef ISESSENTIAL_ON_TOEXPLORE
				bool isEssential = Itemset::computeIsEssential(crtItemset);
				if (isEssential)
#endif
				{
					// add itemset as to explore
					toExplore.push_back(crtItemset);
#ifdef TRACE
					{
						const std::lock_guard<std::mutex> guard(trace_guard);
						std::cout << "add item to toExplore list " << crtItemset->toString() << std::endl;
				}
#endif
			}
			}
		}
	}
}

void TreeNode::addTaskIntoQueue(std::deque<std::shared_ptr<Itemset>>&& toTraverse)
{
	assert(!toTraverse.empty());

	// sort itemset
	std::deque<std::shared_ptr<Itemset>> toExplore;
	std::vector<unsigned int> maxClique;
	this->generateCandidates(std::move(toTraverse), std::move(toExplore), std::move(maxClique));
	toTraverse.clear();

	// emit task
	auto subtask = std::async(std::launch::deferred, &TreeNode::computeMinimalTransversals_task, this, std::move(toExplore), std::move(maxClique));

	// ## SPAWN TASK ##
	{
		// !!! ne pas ajouter la t�che dans le tableau syst�matiquement
		// !!! donner une taille max � task_queue avec un atomic
		// !!! d�finir un ID pour cq t�che, pour faire en sorte qu'une t�che parente ne soit pas bloqu�e par ses t�ches filles
		// !!! ajouter memory_signal de type condition_variable_any
		// !!! utiliser http://www.cplusplus.com/reference/condition_variable/condition_variable_any/wait/
		// !!! memory_signal.wait(lock, fct)
		// !!! laisser toujours une t�che en cours, si la t�che est une feuille de l'arbre, on la bloque pas, on bloque ses fr�res

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

void TreeNode::computeMinimalTransversals_task(std::deque<std::shared_ptr<Itemset>>&& toExplore, std::vector<unsigned int>&& maxClique)
{
	// ## START TASK ##
	//std::cout << "\nNew task with itemset list : ";
	//for_each(toTraverse.begin(), toTraverse.end(), [&](const std::shared_ptr<Itemset> elt) { std::cout << elt->toString(), "\n"; });
	//std::cout << std::endl;

	// test trivial case
	{
		// build new toTraverse list and explore next branch
		// combine each element between [0, toExplore_MaxClique_Index] with the entire combined itemset list
		while(toExplore.size())
		{
			std::shared_ptr<Itemset> toCombinedLeft = toExplore.front();
			toExplore.pop_front();

			if (only_minimal && toCombinedLeft->getItemCount() > minimalMt)
			{
				// we cut the branches here
				// this itemset is bigger than the minimal size of found minimal traverse
				// we dont need to explore this one
				break;
			}

			// build newTraverse list, reserve with max possible size
			std::deque<std::shared_ptr<Itemset>> newToTraverse;
			try
			{
				// first loop on toExplore itemsets
				for_each(toExplore.begin(), toExplore.end(), [&newToTraverse, &toCombinedLeft](std::shared_ptr<Itemset>& toCombinedRight) {

					if (!toCombinedRight->containsAClone())
					{
						unsigned int indexItemToAdd = toCombinedRight->getLastItemAttributeIndex();
						if(Itemset::isEssentialRapid(toCombinedLeft, indexItemToAdd))
						{
#ifndef ISESSENTIAL_ON_TOEXPLORE
							if (Itemset::computeIsEssential(toCombinedLeft, indexItemToAdd))
#endif
							{
								// combine toCombinedRight into toCombinedLeft
								std::shared_ptr<Itemset> newItemset = std::make_shared<Itemset>(toCombinedLeft);
								newItemset->combine(indexItemToAdd);

								// this is a candidate for next iteration
								newToTraverse.push_back(newItemset);
/*#ifdef _DEBUG
								{
									const std::lock_guard<std::mutex> guard(trace_guard);
									std::cout << "create new itemset for " << newItemset->toString() << std::endl;
								}
#endif*/
							}
						}
					}
				});

				// then loop on maxClique itemsets
				for_each(maxClique.begin(), maxClique.end(), [&newToTraverse, &toCombinedLeft](unsigned int attributeIndex) {

					std::shared_ptr<Item> item = BinaryRepresentation::getItemFromKey(attributeIndex);
					if (!item->isAClone())
					{
						if (Itemset::isEssentialRapid(toCombinedLeft, attributeIndex))
						{
							// combine toCombinedRight into toCombinedLeft
							std::shared_ptr<Itemset> newItemset = std::make_shared<Itemset>(toCombinedLeft);
							newItemset->combine(attributeIndex);

							// this is a candidate for next iteration
							newToTraverse.push_back(newItemset);
/*#ifdef _DEBUG
							{
								const std::lock_guard<std::mutex> guard(trace_guard);
								std::cout << "create new itemset for " << newItemset->toString() << std::endl;
								int k = 0;
							}
#endif */

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
				// pending task
				addTaskIntoQueue(std::move(newToTraverse));
				// inc task count
				nbTaskCreated++;
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

std::deque<std::shared_ptr<Itemset>> TreeNode::computeMinimalTransversals(std::deque<std::shared_ptr<Itemset>>&& toTraverse)
{
	// ## START system ##
	
	// toExplore will contains a copy of toTraverse itemsets excepts minimal transverse items
	// itemsets from [0 to toExplore_MaxClique_Index] are to explore
	// itemsets from [toExplore_MaxClique_Index to toExplore size] are max clique
	std::deque<std::shared_ptr<Itemset>> toExplore;
	std::vector<unsigned int> maxClique;
	this->generateCandidates(std::move(toTraverse), std::move(toExplore), std::move(maxClique));
	toTraverse.clear();

	// emit initial task
	auto task = std::async(std::launch::deferred, &TreeNode::computeMinimalTransversals_task, this, std::move(toExplore), std::move(maxClique));

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

					// !!! notifie qu'une t�che a �t� d�pil�e --> on peut en rajouter une
					// !!! analyser la taille de la task_queue, si < seuil alors memory_task.notify_one()
					// !!! utiliser un compteur d'"impacts de t�che" qui compte la m�moire de cq t�che en fonction du nombre d'itemset

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
	

	return this->minimalTransverse;
}
