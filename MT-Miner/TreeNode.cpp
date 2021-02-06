#include "TreeNode.h"
#include "Logger.h"

std::atomic_ullong TreeNode::nbTasks(0);
std::atomic_ullong TreeNode::nbTotalMt(0);
std::atomic_ullong TreeNode::minimalMt(9999);

// to avoid interleaved outputs
std::mutex TreeNode::output_guard;
// synchro stuff
std::deque<std::future<void>> TreeNode::task_queue;
std::mutex TreeNode::task_guard;
std::condition_variable TreeNode::task_signal;

std::mutex TreeNode::memory_guard;
//std::condition_variable_any TreeNode::memory_signal;

std::condition_variable TreeNode::memory_signal;
std::atomic_uint TreeNode::pending_memory_task_count(0);

std::atomic_uint TreeNode::previous_pending_task_count(0);
std::atomic_uint TreeNode::pending_task_count(0);
std::atomic_uint TreeNode::max_pending_task_count(1000);
std::atomic_bool TreeNode::pending_task_checker(true);

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
		unsigned int disjSup = crtItemset->getDisjunctifSupport();
		if (disjSup == objectCount)
		{
			// lock thread and add minimal transverse
			{
				const std::lock_guard<std::mutex> guard(task_guard);
				this->minimalTransverse.push_back(crtItemset);
			}
			//{
			//	const std::lock_guard<std::mutex> guard(memory_guard);
			//	lock_memory_signal = false;
			//	std::cout << "MT added " << crtItemset->toString() << std::endl;
			//}

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

			//memory_signal.notify_one();
		}
		else
		{
			// combine itemset one by one from toTraverse list as combine itemset
			// if disjunctive support for combined itemset is equal to object count --> add the itemset to toExplore list
			//	if not --> add the itemset to maxClique list

			// if current itemset is the 1st one, store it into a previous itemset variable and use it later for computing combined itemsets
			if ((crtItemset == *toTraverse.begin()) && crtItemset->getItemCount() == 1)
			{
				// must be the 1st element with only one element
				Itemset::copyRightIntoLeft(cumulatedItemset, crtItemset);
				maxClique.emplace_back(crtItemset);
			}
			else
			{
				// compute disjunctif support
				unsigned int disjSup = Itemset::computeDisjunctifSupport(cumulatedItemset, crtItemset);
				// test support and add itemset in maxClique or toExplore list
				if (disjSup != objectCount)
				{
					Itemset::combineRightIntoLeft(cumulatedItemset, crtItemset);
					maxClique.emplace_back(crtItemset);
				}
				else
				{
					toExplore.emplace_back(crtItemset);
				}
			}
		}
	});
}

void TreeNode::addTasksForNextCandidates(std::vector<std::shared_ptr<Itemset>>&& toTraverse)
{
	assert(!toTraverse.empty());
	// emit task
	nbTasks++;

	// !!! utilisation d'un "fichier mappé" pour écrire / lire les liste d'itemset dans un fichier (mapped file)
	//std::file_mapping
	// write itemset into file
	//std::string file = std::to_string(nbTasks);
	//file += ".bin";
	//{
	//	std::cout << "writing into " << file << std::endl;
	//	std::ofstream output(file, std::ios::binary);
	//	for_each(newToTraverse.begin(), newToTraverse.end(), [&output](auto itemset) {
	//		itemset->writeToBinaryFile(output);
	//	});
	//	newToTraverse.clear();
	//}

	auto subtask = std::async(std::launch::deferred, &TreeNode::computeMinimalTransversals_task, this, std::move(toTraverse));

	// ## SPAWN TASK ##
	{
		// !!! ne pas ajouter la tâche dans le tableau systématiquement
		// !!! donner une taille max à task_queue avec un atomic
		// !!! définir un ID pour cq tâche, pour faire en sorte qu'une tâche parente ne soit pas bloquée par ses tâches filles
		// !!! ajouter memory_signal de type condition_variable_any
		// !!! utiliser http://www.cplusplus.com/reference/condition_variable/condition_variable_any/wait/
		// !!! memory_signal.wait(lock, fct)
		// !!! utiliser nbTasks pour tester le nb de tâches pour savoir si on en ajoute
		// !!! laisser toujours une tâche en cours, si la tâche est une feuille de l'arbre, on la bloque pas, on bloque ses frères

		// lock to add task into task_queue
		std::unique_lock<std::mutex> lock(task_guard);
		//if (i != 0 && pending_task_count > 2)
		//{
		//	std::cout << "lock this thread for task " << taskId << ", dont add the task" << std::endl;
		//	memory_signal.wait(lock);
		//}

		//std::cout << pending_task_count << std::endl;
		//std::cout << "lock this thread for task " << taskId << " ? " << std::endl;
		//memory_signal.wait(lock, [] {	return (pending_task_count < 1); });

		//std::cout << "add this task for another thread " << taskId << ", add the task" << std::endl;
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

	//std::cout << "toTraverse size " << toTraverse.size() << std::endl;
	//SIZE_T used0 = Utils::printUsedMemoryForCrtProcess();
	//{
	//	const std::lock_guard<std::mutex> lock(task_guard);
	//	std::ifstream input(file, std::ios::binary);
	//	std::string str;
	//	std::string delim = ",";
	//	std::getline(input, str, ';');
	//	do {
	//		//std::cout << str << std::endl;
	//		auto start = 0U;
	//		auto end = str.find(delim);
	//		auto size = str.size();
	//		while (start < size) 
	//		{
	//			StaticBitset b(str.substr(start, end - start));
	//			//std::cout << b.to_string() << std::endl;
	//			start = end + delim.length();
	//			end = str.find(delim, start);
	//		}
	//			
	//	} while (std::getline(input, str, ';'));
	//	input.close();		
	//}

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
		// contains list of itemsets that will be combined to the candidates, the largest space in which is not possible to find minimal transversals
		std::deque<std::shared_ptr<Itemset>> maxClique;
		// contains list of itemsets that are candidates
		std::deque<std::shared_ptr<Itemset>> toExplore;

		// !!! to reserve maxClique and fit/pack (set capacity à la taille correcte, voir les fonctions)
		// !!! to reserve toExplore and fit/pack
		
		// push elements from toTraverse into maxClique, toExplore or minimal transverse
		this->updateListsFromToTraverse(std::move(toTraverse), std::move(maxClique), std::move(toExplore));

		// task is terminated
		// combine element and go for next task 
		//if(pending_task_count > 0)
		//	--pending_task_count; 
		//memory_signal.notify_one();
		

		// !!! virer maxClique et toExplore
		// !!! garder toTraverse et le trier

		// we don't need toTraverse anymore, remove references		
		toTraverse.clear();

		if (toExplore.empty())
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
				
				if (toCombinedLeft->containsAClone())
				{
					toCombinedLeft.reset();
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
							// combine toCombinedRight into toCombinedLeft
							std::shared_ptr<Itemset> newItemset = std::make_shared<Itemset>(toCombinedLeft);
							newItemset->combineItemset(toCombinedRight);
							if (newItemset->computeIsEssential())
							{
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
					std::unique_lock<std::mutex> lock(memory_guard);
					//std::cout << "\ntask " << nbTasks << ", i " << i << std::endl;
					//std::cout << "stored tasks in the list, waiting to be managed : " << pending_task_count << " - blocked tasks, waiting to be unlock : " << pending_memory_task_count << std::endl;
					// pending_task_count : stored tasks in the list, waiting to be managed, memory consuming
					// pending_memory_task_count : blocked tasks, waiting to be unlock, memory is locked
					//if (pending_task_count > max_pending_task_count && pending_memory_task_count < 0)
					if (pending_task_count > max_pending_task_count)
					{
						/// ! ajout d'un thread qui vérifie que pending_task_count n'est pas toujours identique

						pending_memory_task_count++;
						//std::cout << "### lock task" << std::endl;
						//memory_signal.wait(lock , [] {	return !lock_memory_signal; });
						memory_signal.wait(lock);
						//std::cout << "task unlocked" << std::endl;						
					}

					//std::cout << "process task" << std::endl;
					lock.unlock();

					addTasksForNextCandidates(std::move(newToTraverse));
				}
			}
			toExplore.clear();
		}
	}

	// terminate task
	//const std::lock_guard<std::mutex> lock(task_guard);
	if (!--pending_task_count)
	{
		// ## EMIT SHUTDOWN SIGNAL ##
		// awake all idle units for auto-shutdown
		task_signal.notify_all();
		//memory_signal.notify_one();
	}
	// ## EMIT COMPLETE TASK ##

}

void TreeNode::launchPendingTasksChecking()
{
}

std::vector<std::shared_ptr<Itemset>> TreeNode::computeMinimalTransversals(std::vector<std::shared_ptr<Itemset>>&& toTraverse)
{
	// ## START system ##
	//std::string file = std::to_string(nbTasks);
	//file += ".bin";
	//{
	//	std::cout << "writing into " << file << std::endl;
	//	std::ofstream output(file, std::ios::out | std::ios::binary);
	//	for_each(toTraverse.begin(), toTraverse.end(), [&output](auto itemset) {
	//		itemset->writeToBinaryFile(output);
	//		output << ";";
	//		});
	//	output.close();
	//	toTraverse.clear();
	//}
	
	// lambda function called during parsing every 30 seconds
	auto ftr = std::async(std::launch::async, []() {

		const int msToWait = 5;
		auto beginTime = std::chrono::system_clock::now();
		while (pending_task_checker)
		{
			//auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - beginTime).count();
			//if (duration > msToWait)
			{
				//std::unique_lock<std::mutex> lock(memory_guard);
				//std::cout << pending_task_count << " tasks stored in the list, waiting to be managed - " 
				//	      << pending_memory_task_count << " blocked tasks, waiting to be unlock - "
				//		  << max_pending_task_count << " max pending tasks" << std::endl;
				// pending_task_count : stored tasks in the list, waiting to be managed, memory consuming
				// pending_memory_task_count : blocked tasks, waiting to be unlock, memory is locked				
				
				if (pending_task_count >= max_pending_task_count)
				{
					max_pending_task_count++;

					memory_signal.notify_one();
					if(pending_memory_task_count)
						pending_memory_task_count--;
				}
				else 
				{
					if (max_pending_task_count >= 1000)
					{
						max_pending_task_count--;
					}
				}

				//unsigned int crtPendingCount = pending_task_count;
				//previous_pending_task_count = crtPendingCount;

				//std::cout << "process task" << std::endl;
				//lock.unlock();
				//beginTime = std::chrono::system_clock::now();
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
