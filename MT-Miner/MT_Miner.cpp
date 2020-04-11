#include "MT_Miner.h"
#include "utils.h"

MT_Miner::MT_Miner(bool verbose)
{
	this->itemCount = 0;
	this->objectCount = 0;
	this->verbose = verbose;
}

MT_Miner::~MT_Miner()
{

}


void MT_Miner::init(const std::shared_ptr<HyperGraph>& hypergraph)
{
	this->itemCount = hypergraph->getItemCount();
	this->objectCount = hypergraph->getObjectCount();
	
	// build formal context from hypergraph
	FormalContext formalContext(hypergraph);

	// build binary representation from formal context
	binaryRepresentation.reset(nullptr);
	binaryRepresentation = std::make_unique<BinaryRepresentation>(formalContext);
}

bool MT_Miner::checkOneItem(int itemBar, const Itemset& itemsOfpattern)
{
	Bitset SumOfN_1Items(this->objectCount);

	for (int i = 0; i < itemsOfpattern.size(); i++)
	{
		if (itemsOfpattern[i] != itemBar)
		{
			SumOfN_1Items = SumOfN_1Items | this->binaryRepresentation->getElement(itemsOfpattern[i]);
		}
	}

	Bitset bitset = this->binaryRepresentation->getElement(itemBar);
	for (int i = 0; i < this->objectCount; i++)
	{
		if (SumOfN_1Items[i] == false && bitset[i] == true)
			return true;
	}
	return false;
}

// return true if element is essential
bool MT_Miner::isEssential(const Itemset& itemsOfPattern)
{
	if (itemsOfPattern.size() == 1)
		return true;

	for (int i = 0; i < itemsOfPattern.size(); i++)
	{
		if (!checkOneItem(itemsOfPattern[i], itemsOfPattern))
			return false;
	}
	return true;
}

unsigned int MT_Miner::computeDisjonctifSupport(const Itemset& pattern)
{
	Bitset SumOfN_1Items(this->objectCount);

	for (int i = 0; i < pattern.size(); i++)
	{
		SumOfN_1Items = SumOfN_1Items | this->binaryRepresentation->getElement(pattern[i]);
	}
	unsigned int disSupp = 0;
	for (int i = 0; i < this->objectCount; i++)
	{
		if (SumOfN_1Items[i] == 1)
			disSupp++;
	}
	return disSupp;
}

void MT_Miner::computeMinimalTransversals(std::vector<Itemset>& toTraverse, std::vector<Itemset>& mt)
{
	std::vector<Itemset> maxClique;
	std::vector<Itemset> toExplore;
	Itemset previousElt;

	for_each(toTraverse.begin(), toTraverse.end(), [&](const Itemset& currentElt) {

		unsigned int disjSup = computeDisjonctifSupport(currentElt);
		if (disjSup == objectCount)
		{
			mt.push_back(currentElt);
			if (verbose)
				std::cout << "--> minimalTraversal list : " << itemsetListToString(mt) << std::endl;
		}
		else
		{
			if (currentElt == *toTraverse.begin() && currentElt.size() == 1)
			{
				// must be the 1st element with only one element
				previousElt = currentElt;
				maxClique.push_back(currentElt);
				if (verbose)
					std::cout << "--> maxClique list : " << itemsetListToString(maxClique) << std::endl;
			}
			else
			{
				// here, we can combine with previous element
				unsigned int disjSup = computeDisjonctifSupport(currentElt);
				if (disjSup == objectCount)
				{
					mt.push_back(currentElt);
					if (verbose)
						std::cout << "--> minimalTraversal list : " << itemsetListToString(mt) << std::endl;
				}
				else
				{
					// add combinaison of previous + current
					Itemset lastElt = previousElt;
					// make a union on 2 elements
					Itemset combinedElement = combineItemset(lastElt, currentElt);
					// compute disjonctif support of the concatenation
					unsigned int disjSup = computeDisjonctifSupport(combinedElement);
					if (verbose)
						std::cout << "disjonctive support for element \"" << itemsetToString(combinedElement) << "\" : " << disjSup << std::endl;
					if (disjSup != objectCount)
					{
						previousElt = combinedElement;
						maxClique.push_back(currentElt);
						if (verbose)
							std::cout << "--> maxClique list : " << itemsetListToString(maxClique) << std::endl;
					}
					else
					{
						toExplore.push_back(currentElt);
						if (verbose)
							std::cout << "--> toExplore list : " << itemsetListToString(toExplore) << std::endl;
					}
				}
			}
		}
	});

	// explore then branch
	if (!toExplore.empty())
	{
		if (verbose)
		{
			std::cout << "----------------------------------------------------------" << std::endl;
			std::cout << "toExplore list" << itemsetListToString(toExplore) << std::endl;
			std::cout << "maxClique list" << itemsetListToString(maxClique) << std::endl;
		}

		// store toExploreList max index
		unsigned int lastIndexToTest = toExplore.size();
		// combine toExplore (left part) with maxClique list (right part) into a combined list
		std::vector<Itemset> combinedList = toExplore;
		combinedList.insert(combinedList.end(), maxClique.begin(), maxClique.end());

		for (unsigned int i = 0; i < lastIndexToTest; i++)
		{
			std::vector<Itemset> newToTraverse;
			Itemset toCombinedLeft = combinedList[i];

			for (unsigned int j = i + 1; j < combinedList.size(); j++)
			{
				assert(j < combinedList.size());
				Itemset toCombinedRight = combinedList[j];
				Itemset combinedString = combineItemset(toCombinedLeft, toCombinedRight);
				newToTraverse.push_back(combinedString);
			}

			std::vector<unsigned int> indexToRemove;
			for (unsigned int i = 0; i < newToTraverse.size(); i++)
			{
				if (!isEssential(newToTraverse[i]))
				{
					indexToRemove.push_back(i);
					if (verbose)
						std::cout << "remove element " << itemsetToString(newToTraverse[i]) << " from next iteration, not essential" << std::endl;
				}
			}
			for (unsigned int i = indexToRemove.size(); i--; )
			{
				newToTraverse.erase(newToTraverse.begin() + indexToRemove[i]);
			}

			if (verbose)
			{
				std::cout << "new toTraverse list" << itemsetListToString(newToTraverse) << std::endl;
				std::cout << "----------------------------------------------------------" << std::endl;
			}

			computeMinimalTransversals(newToTraverse, mt);
		}
	}
	else
	{
		if (verbose)
			std::cout << "toExplore list is empty, end of the branch" << std::endl;
	}
}