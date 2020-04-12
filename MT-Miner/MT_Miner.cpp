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
	une fois tu les a

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
}

bool MT_Miner::checkOneItem(int itemBar, const Itemset& itemsOfpattern) const
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
bool MT_Miner::isEssential(const Itemset& itemsOfPattern) const
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

unsigned int MT_Miner::computeDisjonctifSupport(const Itemset& pattern) const
{
	Bitset SumOfN_1Items(this->objectCount);

	for (int i = 0; i < pattern.size(); i++)
	{
#ifdef _DEBUG
		for (int j = 0; j < this->objectCount; j++)
		{
			SumOfN_1Items[j] = SumOfN_1Items[j] || this->binaryRepresentation->getElement(pattern[i])[j];
		}
#else
		SumOfN_1Items = SumOfN_1Items | this->binaryRepresentation->getElement(pattern[i]);
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

MT_Miner::Itemset MT_Miner::combineItemset(const Itemset& str1, const Itemset& str2) const
{
	// "1" + "2" => "12"
	// "71" + "72" => "712"
	Itemset left = str1;
	Itemset right = str2;
	std::vector<Itemset> combinedListElt;
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
	Itemset combinedElt;
	for_each(left.begin(), left.end(), [&](unsigned int i) {
		combinedElt.push_back(i);
		});
	return combinedElt;
}

void MT_Miner::computeMinimalTransversals(std::vector<Itemset>& toTraverse, std::vector<Itemset>& mt) const
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
					Itemset lastElt = previousElt;
					// make a union on 2 elements
					Itemset combinedElement = combineItemset(lastElt, currentElt);
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
						toExplore.push_back(currentElt);
						if (verbose)
							std::cout << "--> toExplore list : " << Utils::itemsetListToString(toExplore) << std::endl;
					}
				}
			}
		}
	});

	// explore then branch and recurse
	if (!toExplore.empty())
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
						std::cout << "remove element " << Utils::itemsetToString(newToTraverse[i]) << " from next iteration, not essential" << std::endl;
				}
			}
			for (unsigned int i = indexToRemove.size(); i--; )
			{
				newToTraverse.erase(newToTraverse.begin() + indexToRemove[i]);
			}

			if (verbose)
			{
				std::cout << "new toTraverse list" << Utils::itemsetListToString(newToTraverse) << std::endl;
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

