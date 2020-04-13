#include "MT_Miner.h"
#include "utils.h"

MT_Miner::MT_Miner(bool verbose)
{
	//this->itemCount = 0;
	//this->objectCount = 0;
	this->verbose = verbose;
}

MT_Miner::~MT_Miner()
{

}

void MT_Miner::init(const std::shared_ptr<HyperGraph>& hypergraph)
{
	//this->itemCount = hypergraph->getItemCount();
	//this->objectCount = hypergraph->getObjectCount();
	
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
	binaryRepresentation.reset();
	binaryRepresentation = std::make_shared<BinaryRepresentation>(formalContext);

	// build clone
	//for(unsigned int i = 0; i < )
	//checkClone(unsigned int index);

	// remove clone columns
}


std::vector<Utils::Itemset> MT_Miner::computeMinimalTransversals(const std::vector<Utils::Itemset>& toTraverse) const
{
	std::vector<Utils::Itemset> graph_mt;
	std::shared_ptr<GraphNode> node = std::make_shared<GraphNode>(toTraverse, binaryRepresentation);
	node->computeMinimalTransversals(graph_mt);
	return graph_mt;
	//return node->getGraphMt();

	/*// minimal transversal return list
	std::vector<Utils::Itemset> mt;

	std::vector<Utils::Itemset> maxClique;
	std::vector<Utils::Itemset> toExplore;
	
	// results of cumulated combined items
	// must be declared outside of the loop
	Utils::Itemset previousItem;

	// build maxClique, toExplore lists
	for_each(toTraverse.begin(), toTraverse.end(), [&](const Utils::Itemset& currentItem) {

		unsigned int disjSup = binaryRepresentation->computeDisjonctifSupport(currentItem);
		if (disjSup == objectCount)
		{
			mt.push_back(currentItem);
			if (verbose)
				std::cout << "--> minimalTraversal list : " << Utils::itemsetListToString(mt) << std::endl;
		}
		else
		{
			if (currentItem == *toTraverse.begin() && currentItem.size() == 1)
			{
				// must be the 1st element with only one element
				previousItem = currentItem;
				maxClique.push_back(currentItem);
				if (verbose)
					std::cout << "--> maxClique list : " << Utils::itemsetListToString(maxClique) << std::endl;
			}
			else
			{
				// here, we can combine with previous element
				unsigned int disjSup = binaryRepresentation->computeDisjonctifSupport(currentItem);
				if (disjSup == objectCount)
				{
					mt.push_back(currentItem);
					if (verbose)
						std::cout << "--> minimalTraversal list : " << Utils::itemsetListToString(mt) << std::endl;
				}
				else
				{
					// add combinaison of previous + current
					Utils::Itemset lastElt = previousItem;
					// make a union on 2 elements
					Utils::Itemset combinedItem = Utils::combineItemset(lastElt, currentItem);
					// compute disjonctif support of the concatenation
					unsigned int disjSup = binaryRepresentation->computeDisjonctifSupport(combinedItem);
					if (verbose)
						std::cout << "disjonctive support for element \"" << Utils::itemsetToString(combinedItem) << "\" : " << disjSup << std::endl;
					if (disjSup != objectCount)
					{
						previousItem = combinedItem;
						maxClique.push_back(currentItem);
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

						toExplore.push_back(currentItem);
						if (verbose)
							std::cout << "--> toExplore list : " << Utils::itemsetListToString(toExplore) << std::endl;
					}
				}
			}
		}
	});

	if (toExplore.empty())
	{
		if (verbose)
			std::cout << "toExplore list is empty, end of the branch" << std::endl;
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
			//if (isClone(i))
			//{

			//}

			std::vector<Utils::Itemset> newToTraverse;
			Utils::Itemset toCombinedLeft = combinedList[i];

			for (unsigned int j = i + 1; j < combinedList.size(); j++)
			{
				assert(j < combinedList.size());
				Utils::Itemset toCombinedRight = combinedList[j];
				Utils::Itemset combinedString = Utils::combineItemset(toCombinedLeft, toCombinedRight);

				// check clone
				//for (unsigned int i = 0; i < newToTraverse.size(); i++)
				//{
				//	Utils::Itemset item = newToTraverse[i];

				//}

				if (binaryRepresentation->isEssential(combinedString))
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

			// compute minimal transversals for the branch
			std::vector<Utils::Itemset> mtBranch = computeMinimalTransversals(newToTraverse);

			// concatenating parent mt from the branch with current mt list
			mt.insert(mt.end(), mtBranch.begin(), mtBranch.end());
		}
	}
	return mt;*/
}

