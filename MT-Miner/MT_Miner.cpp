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

bool MT_Miner::checkOneItem(int itemBar, const std::vector<unsigned int>& itemsOfpattern)
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
bool MT_Miner::isEss(const std::string& pattern)
{
	std::vector<unsigned int> itemsOfPattern = splitToVectorOfInt(pattern, ' ');
	if (itemsOfPattern.size() == 1)
		return true;

	for (int i = 0; i < itemsOfPattern.size(); i++)
	{
		if (!checkOneItem(itemsOfPattern[i], itemsOfPattern))
			return false;
	}
	return true;
}

unsigned int MT_Miner::computeDisjonctifSupport(const std::string& pattern)
{
	std::vector<unsigned int> itemsOfpattern = splitToVectorOfInt(pattern, ' ');
	Bitset SumOfN_1Items(this->objectCount);

	for (int i = 0; i < itemsOfpattern.size(); i++)
	{
		SumOfN_1Items = SumOfN_1Items | this->binaryRepresentation->getElement(itemsOfpattern[i]);
	}
	unsigned int disSupp = 0;
	for (int i = 0; i < this->objectCount; i++)
	{
		if (SumOfN_1Items[i] == 1)
			disSupp++;
	}
	return disSupp;
}

void MT_Miner::computeMinimalTransversals(std::vector<std::string>& toTraverse, std::vector<std::string>& mt)
{
	std::vector<std::string> maxClique;
	std::vector<std::string> toExplore;
	std::string previousElt;

	for_each(toTraverse.begin(), toTraverse.end(), [&](const std::string& currentElt) {

		unsigned int disjSup = computeDisjonctifSupport(currentElt);
		if (disjSup == objectCount)
		{
			mt.push_back(currentElt);
			if (verbose)
			{
				std::cout << "minimalTraversal list : ";
				printStringVectorList(mt);
				std::cout << std::endl;
			}
		}
		else
		{
			if (currentElt == *toTraverse.begin() && currentElt.size() == 1)
			{
				// must be the 1st element with only one element
				previousElt = currentElt;
				maxClique.push_back(currentElt);
				if (verbose)
				{
					std::cout << "maxClique list : ";
					printStringVectorList(maxClique);
					std::cout << std::endl;
				}
			}
			else
			{
				// here, we can combine with previous element
				unsigned int disjSup = computeDisjonctifSupport(currentElt);
				if (disjSup == objectCount)
				{
					mt.push_back(currentElt);
					if (verbose)
					{
						std::cout << "minimalTraversal list : ";
						printStringVectorList(mt);
						std::cout << std::endl;
					}
				}
				else
				{
					// add combinaison of previous + current
					std::string lastElt = previousElt;
					// make a union on 2 elements
					std::string combinedElement = combineIntoString(lastElt, currentElt);
					// compute disjonctif support of the concatenation
					unsigned int disjSup = computeDisjonctifSupport(combinedElement);
					if (verbose)
						std::cout << "disjonctive support for element \"" << combinedElement << "\" : " << disjSup << std::endl;
					if (disjSup != objectCount)
					{
						previousElt = combinedElement;
						maxClique.push_back(currentElt);
						if (verbose)
						{
							std::cout << "maxClique list : ";
							printStringVectorList(maxClique);
							std::cout << std::endl;
						}
					}
					else
					{
						toExplore.push_back(currentElt);
						if (verbose)
						{
							std::cout << "toExplore list : ";
							printStringVectorList(toExplore);
							std::cout << std::endl;
						}
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

			std::cout << "toExplore list" << std::endl;
			printStringVectorList(toExplore);
			std::cout << std::endl;

			std::cout << "maxClique list" << std::endl;
			printStringVectorList(maxClique);
			std::cout << std::endl;
		}

		// store toExploreList max index
		unsigned int lastIndexToTest = toExplore.size();
		// combine toExplore (left part) with maxClique list (right part) into a combined list
		std::vector<std::string> combinedList = toExplore;
		combinedList.insert(combinedList.end(), maxClique.begin(), maxClique.end());

		for (unsigned int i = 0; i < lastIndexToTest; i++)
		{
			std::vector<std::string> newToTraverse;
			std::string toCombinedLeft = combinedList[i];

			for (unsigned int j = i + 1; j < combinedList.size(); j++)
			{
				assert(j < combinedList.size());
				std::string toCombinedRight = combinedList[j];
				std::string combinedString = combineIntoString(toCombinedLeft, toCombinedRight);
				newToTraverse.push_back(combinedString);
			}

			std::vector<unsigned int> indexToRemove;
			for (unsigned int i = 0; i < newToTraverse.size(); i++)
			{
				if (!isEss(newToTraverse[i]))
				{
					indexToRemove.push_back(i);
					if (verbose)
					{
						std::cout << "remove element {" << newToTraverse[i] << "} from next iteration, not essential" << std::endl;
					}
				}
			}
			for (unsigned int i = indexToRemove.size(); i--; )
			{
				newToTraverse.erase(newToTraverse.begin() + indexToRemove[i]);
			}

			if (verbose)
			{
				std::cout << "new toTraverse list" << std::endl;
				printStringVectorList(newToTraverse);
				std::cout << std::endl;

				std::cout << "----------------------------------------------------------" << std::endl;
			}

			computeMinimalTransversals(newToTraverse, mt);
		}
	}
	else
	{
		if (verbose)
		{
			std::cout << "toExplore list is empty, end of the branch" << std::endl;
		}
	}
}