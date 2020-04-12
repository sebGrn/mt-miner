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

	/*
	avant de commencer l'exploration des TM tu cherches dans le fichier les colonnes clones
	qui ont le meme support et qui couvrent exactement les memes objets
	si tu fais le ET logique du support de la colonne avec son clone tu obtiens le meme vecteur
	donc c'est comme ça que tu identifies ces clones
	une fois tu les as

	dans le fichier

	et tu gardes un clone  par groupe
	tu vois comme l'exemple Hyp1

	tu gardes soit 9 soit 10 dans ton contexte
	apres tu n'as pas a explorer la branche de 10
	tu vas prendre les MT obtenus de la branche 9
	et tu remplaces 9 par 10
	*/

	// build binary representation from formal context
	binaryRepresentation.reset(nullptr);
	binaryRepresentation = std::make_unique<BinaryRepresentation>(formalContext);

	// build clone
	//for(unsigned int i = 0; i < )
	//checkClone(unsigned int index);

	// remove clone columns
}

bool MT_Miner::checkOneItem(int itemBar, const Utils::Itemset& itemsOfpattern) const
{
	Bitset SumOfN_1Items(this->objectCount);

	for_each(itemsOfpattern.begin(), itemsOfpattern.end(), [&](unsigned int elt) {
		if (elt != itemBar)
		{
#ifdef _DEBUG
			for (int j = 0; j < this->objectCount; j++)
			{
				SumOfN_1Items[j] = SumOfN_1Items[j] || this->binaryRepresentation->getElement(elt)[j];
			}
#else
			SumOfN_1Items = SumOfN_1Items | this->binaryRepresentation->getElement(elt);
#endif
		}
	});

	Bitset bitset = this->binaryRepresentation->getElement(itemBar);
	for (int i = 0; i < this->objectCount; i++)
	{
		if (SumOfN_1Items[i] == false && bitset[i] == true)
			return true;
	}
	return false;
}

// return true if element is essential
bool MT_Miner::isEssential(const Utils::Itemset& itemsOfPattern) const
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

unsigned int MT_Miner::computeDisjonctifSupport(const Utils::Itemset& pattern) const
{
	Bitset SumOfN_1Items(this->objectCount);

	for (int i = 0; i < pattern.size(); i++)
	{
		unsigned int columnKey = pattern[i];

		
#ifdef _DEBUG
		for (int j = 0; j < this->objectCount; j++)
		{
			SumOfN_1Items[j] = SumOfN_1Items[j] || this->binaryRepresentation->getElement(columnKey)[j];
		}
#else
		SumOfN_1Items = SumOfN_1Items | this->binaryRepresentation->getElement(columnKey);
#endif			
	}
	unsigned int disSupp = 0;
	for (int i = 0; i < this->objectCount; i++)
	{
		if (SumOfN_1Items[i] == 1)
			disSupp++;
	}
	return disSupp;
}

Utils::Itemset MT_Miner::combineItemset(const Utils::Itemset& str1, const Utils::Itemset& str2) const
{
	// "1" + "2" => "12"
	// "71" + "72" => "712"
	Utils::Itemset left = str1;
	Utils::Itemset right = str2;
	std::vector<Utils::Itemset> combinedListElt;
	for_each(str1.begin(), str1.end(), [&](unsigned int i) {
		auto it = std::find_if(right.begin(), right.end(), Utils::compare_int(i));
		if (it != right.end())
		{
			// remove elt
			right.erase(it);
		}
		});
	// merge 2 lists into intList1
	left.insert(left.end(), right.begin(), right.end());
	// transform int list into string list seperated by ' '
	Utils::Itemset combinedElt;
	for_each(left.begin(), left.end(), [&](unsigned int i) {
		combinedElt.push_back(i);
		});
	return combinedElt;
}

std::vector<Utils::Itemset> MT_Miner::computeMinimalTransversals(std::vector<Utils::Itemset>& toTraverse) const
{
	// minimal transversal return list
	std::vector<Utils::Itemset> mt;

	std::vector<Utils::Itemset> maxClique;
	std::vector<Utils::Itemset> toExplore;
	
	Utils::Itemset previousElt;

	// build maxClique, toExplore lists
	for_each(toTraverse.begin(), toTraverse.end(), [&](const Utils::Itemset& currentElt) {

		unsigned int disjSup = computeDisjonctifSupport(currentElt);
		if (disjSup == objectCount)
		{
			mt.push_back(currentElt);
			if (verbose)
				std::cout << "--> minimalTraversal list : " << Utils::itemsetListToString(mt) << std::endl;
		}
		else
		{
			if (currentElt == *toTraverse.begin() && currentElt.size() == 1)
			{
				// must be the 1st element with only one element
				previousElt = currentElt;
				maxClique.push_back(currentElt);
				if (verbose)
					std::cout << "--> maxClique list : " << Utils::itemsetListToString(maxClique) << std::endl;
			}
			else
			{
				// here, we can combine with previous element
				unsigned int disjSup = computeDisjonctifSupport(currentElt);
				if (disjSup == objectCount)
				{
					mt.push_back(currentElt);
					if (verbose)
						std::cout << "--> minimalTraversal list : " << Utils::itemsetListToString(mt) << std::endl;
				}
				else
				{
					// add combinaison of previous + current
					Utils::Itemset lastElt = previousElt;
					// make a union on 2 elements
					Utils::Itemset combinedElement = combineItemset(lastElt, currentElt);					
					// compute disjonctif support of the concatenation
					unsigned int disjSup = computeDisjonctifSupport(combinedElement);
					if (verbose)
						std::cout << "disjonctive support for element \"" << Utils::itemsetToString(combinedElement) << "\" : " << disjSup << std::endl;
					if (disjSup != objectCount)
					{
						previousElt = combinedElement;
						maxClique.push_back(currentElt);
						if (verbose)
							std::cout << "--> maxClique list : " << Utils::itemsetListToString(maxClique) << std::endl;
					}
					else
					{
						// check if columnKey has a clone
						// if it has a clone, use the clone value computation instead of computing the support
						//{
						//	for (unsigned int i = 0; i < toExplore.size(); i++)
						//	{

						//	}
						//}

						toExplore.push_back(currentElt);
						if (verbose)
							std::cout << "--> toExplore list : " << Utils::itemsetListToString(toExplore) << std::endl;
					}
				}
			}
		}
	});

	// explore then branch and recurse
	if (toExplore.empty())
	{
		if (verbose)
			std::cout << "toExplore list is empty, end of the branch" << std::endl;
		return mt;

	}
	else
	{
		if (verbose)
		{
			std::cout << "----------------------------------------------------------" << std::endl;
			std::cout << "toExplore list" << Utils::itemsetListToString(toExplore) << std::endl;
			std::cout << "maxClique list" << Utils::itemsetListToString(maxClique) << std::endl;
		}

		// store toExploreList max index
		unsigned int lastIndexToTest = toExplore.size();
		// combine toExplore (left part) with maxClique list (right part) into a combined list
		std::vector<Utils::Itemset> combinedList = toExplore;
		combinedList.insert(combinedList.end(), maxClique.begin(), maxClique.end());

		for (unsigned int i = 0; i < lastIndexToTest; i++)
		{
			std::vector<Utils::Itemset> newToTraverse;
			Utils::Itemset toCombinedLeft = combinedList[i];

			for (unsigned int j = i + 1; j < combinedList.size(); j++)
			{
				assert(j < combinedList.size());
				Utils::Itemset toCombinedRight = combinedList[j];
				Utils::Itemset combinedString = combineItemset(toCombinedLeft, toCombinedRight);

				// check clone
				/*for (unsigned int i = 0; i < newToTraverse.size(); i++)
				{
					Utils::Itemset item = newToTraverse[i];

				}*/


				if (isEssential(combinedString))
					newToTraverse.push_back(combinedString);
				else
				{
					if (verbose)
						std::cout << Utils::itemsetToString(combinedString) << " is not essential" << std::endl;
				}
			}

			if (verbose)
			{
				std::cout << "new toTraverse list" << Utils::itemsetListToString(newToTraverse) << std::endl;
				std::cout << "----------------------------------------------------------" << std::endl;
			}

			std::vector<Utils::Itemset> mtBranch = computeMinimalTransversals(newToTraverse);
			mt.insert(mt.end(), mtBranch.begin(), mtBranch.end());
		}
		return mt;
	}
}

