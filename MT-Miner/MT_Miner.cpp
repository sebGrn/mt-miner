#include "MT_Miner.h"
#include "utils.h"

MT_Miner::MT_Miner()
{
	this->itemCount = 0;
	this->objectCount = 0;
	this->verbose = false;
}

MT_Miner::~MT_Miner()
{

}

void MT_Miner::init(unsigned int itemCount, unsigned int objectCount, const std::vector<std::vector<unsigned int>>& hypergraph)
{
	this->itemCount = itemCount;
	this->objectCount = objectCount;
	assert(hypergraph.size() == this->objectCount);

	// build formal context
	std::vector<std::vector<bool>> formalContext;

	for (unsigned int i = 0; i < hypergraph.size(); i++)
	{
		std::vector<bool> bitset(this->itemCount);
		transform(bitset.begin(), bitset.end(), bitset.begin(), [](bool b) { return false; });

		std::vector<unsigned int> line = hypergraph[i];
		for (unsigned int j = 0; j < line.size(); j++)
		{
			assert(line[j] >= 1);
			unsigned int index = line[j] - 1;
			assert(index < bitset.size());
			bitset[index] = true;
		}
		formalContext.push_back(bitset);
	}

	// build binary representation from formal context
	for (unsigned int j = 0; j < this->itemCount; j++)	// 8 on test.txt
	{
		std::vector<bool> bitset(this->objectCount);
		for (unsigned int i = 0; i < this->objectCount; i++)	// 6 on test.txt
		{
			bitset[i] = formalContext[i][j];
		}
		this->binaryRep[j + 1] = bitset;
	}
}

unsigned int MT_Miner::computeDisjonctifSupport(const std::string& pattern)
{
	std::vector<unsigned int> itemsOfpattern = splitToVectorOfInt(pattern, ' ');
	std::vector<unsigned int> SumOfN_1Items;
	for (int i = 0; i < objectCount; i++)
		SumOfN_1Items.push_back(0);

	for (int i = 0; i < itemsOfpattern.size(); i++)
	{
		for (int j = 0; j < objectCount; j++)
		{
			SumOfN_1Items[j] = (SumOfN_1Items[j] != 0) | binaryRep[itemsOfpattern[i]][j];
		}
	}
	unsigned int disSupp = 0;
	for (int i = 0; i < objectCount; i++)
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

		if (currentElt == *toTraverse.begin() && currentElt.size() == 1)
		{
			// add solo element
			unsigned int disjSup = computeDisjonctifSupport(currentElt);
			if (disjSup != objectCount)
			{
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
				// here we add element into toExplore list or into minimumTrasversals list ?
				toExplore.push_back(currentElt);
				if (verbose)
				{
					std::cout << "toExplore list : ";
					printStringVectorList(toExplore);
					std::cout << std::endl;
				}
			}
		}
		else
		{
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
}