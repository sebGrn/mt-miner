#include "TreeNode.h"
#include "Logger.h"

std::atomic_int TreeNode::nbRunningThread = 0;
std::atomic_int TreeNode::nbTotalChildren = 0;

TreeNode::TreeNode(bool useCloneOptimization, const std::shared_ptr<BinaryRepresentation>& binaryRepresentation)
{
	this->binaryRepresentation = binaryRepresentation;
	this->useCloneOptimization = useCloneOptimization;
	//this->toTraverse = toTraverse;
	recursionLevel = 0;

	//this->thread = std::make_unique<std::thread>(&TreeNode::computeMinimalTransversals, this, std::ref(graph_mt));
	//this->thread->join();

	//threadList.push_back(std::thread(&TreeNode::computeMinimalTransversals, this, std::ref(graph_mt)));
}

TreeNode::~TreeNode()
{
	//this->thread->join();
	//for (std::unique_ptr<std::thread> thread : this->childrenThread)
	//{
	//	thread->join();
	//}

	// Now wait for all the worker thread to finish i.e.
	// Call join() function on each of the std::thread object
	//std::cout << "wait for all the worker thread to finish" << std::endl;
	//std::for_each(children.begin(), children.end(), std::mem_fn(&std::thread::join));
}

void TreeNode::joinThead()
{
	for (auto it = this->children.begin(); it != this->children.end(); it++)
	{
		(*it)->joinThead();
	}
	std::for_each(this->threadList.begin(), this->threadList.end(), std::mem_fn(&std::thread::join));
}

void TreeNode::buildClonedCombinaison(const Utils::Itemset& currentItem, std::vector<Utils::Itemset>& clonedCombination, const std::vector<std::pair<unsigned int, unsigned int>>& originalClonedIndexes)
{
	for (auto it = originalClonedIndexes.begin(); it != originalClonedIndexes.end(); it++)
	{
		unsigned int originalIndex = it->first;
		unsigned int clonedIndex = it->second;
		Utils::Itemset clonedCurrentItem = currentItem;
		replace(clonedCurrentItem.begin(), clonedCurrentItem.end(), originalIndex, clonedIndex);
		if (clonedCurrentItem != currentItem)
		{
			auto it = std::find_if(clonedCombination.begin(), clonedCombination.end(), Utils::compare_itemset(clonedCurrentItem));
			if (it == clonedCombination.end())
			{
				clonedCombination.push_back(clonedCurrentItem);
				// recurse on new combination
				buildClonedCombinaison(clonedCurrentItem, clonedCombination, originalClonedIndexes);
			}
		}
	}
}

void TreeNode::computeListsFromToTraverse(const std::vector<Utils::Itemset>& toTraverse, std::vector<Utils::Itemset>& maxClique, std::vector<Utils::Itemset>& toExplore, std::vector<Utils::Itemset>& graph_mt)
{
	maxClique.clear();
	toExplore.clear();

	// results of cumulated combined items / must be declared outside of the loop
	Utils::Itemset previousItem;
	// loop on toTraverse list and build maxClique and toExplore lists
	for_each(toTraverse.begin(), toTraverse.end(), [&](const Utils::Itemset& currentItem) {
	
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
					std::vector<Utils::Itemset> clonedCombination;
					buildClonedCombinaison(currentItem, clonedCombination, originalClonedIndexes);

					for (auto it = clonedCombination.begin(); it != clonedCombination.end(); it++)
					{
						graph_mt.push_back(*it);
					}
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
				Utils::Itemset combinedItem = Utils::combineItemset(previousItem, currentItem);
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
	});
}

//void TreeNode::computeMinimalTransversalsCb(int nb, std::vector<Utils::Itemset>& graph_mt)
//{
//	//std::cout << "Worker Thread " << std::this_thread::get_id() << " is Executing" << std::endl;
//	if (nb < 0)
//		return;
//
//
//	m1.lock();
//
//	graph_mt.push_back(Utils::Itemset(1, nb));
//	//std::cout << "graph_mt size " << graph_mt.size() << std::endl;
//
//	computeMinimalTransversalsCb(--nb, graph_mt);
//
//	m1.unlock();
//	
//	//
//	////std::lock_guard<std::mutex> guard1(myMutex);
//	//{
//	//	nbRunningThread++;
//	//	std::cout << "computeMinimalTransversals in thread " << TreeNode::nbRunningThread << std::endl;
//	//}
//
//	////if (TreeNode::nbRunningThread <= 10)
//	//{
//	//	for (int i = 0; i < 1; i++)
//	//	{
//	//		std::shared_ptr<TreeNode> node = std::make_shared<TreeNode>(this->useCloneOptimization, toTraverse, this->binaryRepresentation, graph_mt);
//	//		this->children.push_back(node);
//
//	//		//graph_mt.push_back(Utils::Itemset(1, nbRunningThread));
//	//		this->threadList.push_back(std::thread(&TreeNode::computeMinimalTransversalsCb, node, std::ref(graph_mt)));
//	//	}
//	//}
//}

//std::recursive_mutex m1;

void TreeNode::computeMinimalTransversals(const std::vector<Utils::Itemset>& toTraverse, std::vector<Utils::Itemset>& graph_mt)
{
	//nbRunningThread++;
	//this->toTraverse.clear();
	//this->toTraverse.push_back(Utils::Itemset(1, nbRunningThread));
	//std::cout << "computeMinimalTransversals in thread " << TreeNode::nbRunningThread << std::endl;
	
	/*this->threadList.push_back(std::thread(&TreeNode::computeMinimalTransversalsCb, this, 1000, std::ref(graph_mt)));
	this->threadList.push_back(std::thread(&TreeNode::computeMinimalTransversalsCb, this, 1000, std::ref(graph_mt)));
	this->threadList.push_back(std::thread(&TreeNode::computeMinimalTransversalsCb, this, 1000, std::ref(graph_mt)));

	std::for_each(this->threadList.begin(), this->threadList.end(), std::mem_fn(&std::thread::join));*/



	/*if (nbRunningThread <= 2)
	{
		//std::vector<std::thread> threadList;
		for (int i = 0; i < 2; i++)
		{
			std::shared_ptr<TreeNode> node = std::make_shared<TreeNode>(this->useCloneOptimization, toTraverse, this->binaryRepresentation, graph_mt);
			this->children.push_back(node);
			
			//graph_mt.push_back(Utils::Itemset(1, nbRunningThread));
			this->threadList.push_back(std::thread(&TreeNode::computeMinimalTransversals, node, std::ref(graph_mt)));
		}
	}*/		
	
	// compute toExplore : contains list of itemsets that are candidates
	// compute maxClique : contains list of itemsets that will be combined to the candidates
	// update graph_mt : contains the final minimal transverals
	
	if (toTraverse.empty())
		return;

	//m1.lock();

	std::vector<Utils::Itemset> maxClique;
	std::vector<Utils::Itemset> toExplore;
	this->computeListsFromToTraverse(toTraverse, maxClique, toExplore, graph_mt);

	if (!toExplore.empty())
	{
		//Logger::log("toExplore list", Utils::itemsetListToString(toExplore), " - recursion level ", recursionLevel, "\n");
		//Logger::log("maxClique list", Utils::itemsetListToString(maxClique), "\n");

		// store toExploreList max index
		unsigned int lastIndexToTest = toExplore.size();
		// combine toExplore (left part) with maxClique list (right part) into a combined list
		std::vector<Utils::Itemset> combinedItemsetList = toExplore;
		combinedItemsetList.insert(combinedItemsetList.end(), maxClique.begin(), maxClique.end());

		// loop on candidates from toExplore list only
		//#pragma omp parallel for
		for (int i = 0; i < lastIndexToTest; i++)
		{
			// build newTraverse list
			std::vector<Utils::Itemset> newToTraverse;
			Utils::Itemset toCombinedLeft = combinedItemsetList[i];
			// combine each element between [0, lastIndexToTest] with the entire combined itemset list
			for (unsigned int j = i + 1; j < combinedItemsetList.size(); j++)
			{
				assert(j < combinedItemsetList.size());
				Utils::Itemset toCombinedRight = combinedItemsetList[j];
				Utils::Itemset combinedItemset = Utils::combineItemset(toCombinedLeft, toCombinedRight);

				// check if combined item is containing a clone (if true, do not compute the minimal transverals) and if combined itemset is essential
				if (!this->binaryRepresentation->containsAClone(combinedItemset) && binaryRepresentation->isEssential(combinedItemset))
					newToTraverse.push_back(combinedItemset);
			}

			// create a new child node for this newToTraverse list
			std::shared_ptr<TreeNode> node = std::make_shared<TreeNode>(this->useCloneOptimization, this->binaryRepresentation);
			// add this node as a child
			this->children.push_back(node);
			node->recursionLevel = this->recursionLevel + 1;

			// recurse
			/*if (this->recursionLevel == 0)
			{
				// create thread for 1st branch 
				threadList.push_back(std::thread(&TreeNode::computeMinimalTransversals, node, std::ref(newToTraverse), std::ref(graph_mt)));
			}
			else
			{
				// compute minimal transversals for the branch
				node->computeMinimalTransversals(newToTraverse, graph_mt);
			}*/
			node->computeMinimalTransversals(newToTraverse, graph_mt);
		}
	}
	//m1.unlock();
}