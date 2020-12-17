#include "MT_Miner.h"
#include "utils.h"

std::atomic_bool MT_Miner::stop(false);

MT_Miner::MT_Miner(bool useCloneOptimization)
{
	this->useCloneOptimization = useCloneOptimization;		
}

MT_Miner::~MT_Miner()
{
}

void MT_Miner::createBinaryRepresentation(const HyperGraph& hypergraph)
{
	// build formal context from hypergraph
	FormalContext formalContext(hypergraph);
	//formalContext.serialize("format_context.csv");

	// build binary representation from formal context
	BinaryRepresentation::buildFromFormalContext(formalContext);
	//BinaryRepresentation<bitset_type>::serialize("binary_rep.csv");

	if (this->useCloneOptimization)
	{
		// avant de commencer l'exploration des TM tu cherches dans le fichier les colonnes clones
		// qui ont le meme support et qui couvrent exactement les memes objets
		// si tu fais le ET logique du support de la colonne avec son clone tu obtiens le meme vecteur
		// donc c'est comme ça que tu identifies ces clones une fois tu les as
		//
		// et tu gardes un clone  par groupe tu vois comme l'exemple Hyp1
		//
		// tu gardes soit 9 soit 10 dans ton contexte, apres tu n'as pas a explorer la branche de 10
		// tu vas prendre les MT obtenus de la branche 9 et tu remplaces 9 par 10

		// explore binary representation to check if we have clone
		// if we have, memorize the indexes of the original and the cloned
		// if the cloned bitset index is into a toExplore list, dont compute the mt for the clone but use those from the original
		//Logger::log("computing clones\n");
		unsigned int cloneListSize = BinaryRepresentation::buildCloneList();
		Logger::log(cloneListSize, " clones found\n");

		if (cloneListSize == 0)
			this->useCloneOptimization = false;
	}
}

void MT_Miner::computeInitalToTraverseList(std::vector<std::shared_ptr<Itemset>>& toTraverse) const
{
	assert(toTraverse.empty());
	for (unsigned int i = 1; i <= BinaryRepresentation::getItemCount(); i++)
	{
		std::shared_ptr<Item> item = BinaryRepresentation::getItemFromKey(i);
		// dont push clones into initial trasverse list
		if (!item->isAClone())
		{
			// get itemset from binary representation and store them into lists
			// these itemsets will be used in the program
			// we dont need binary representation anymore here
			std::shared_ptr<Itemset> itemset(new Itemset(item));
			toTraverse.push_back(itemset);
		}
	}
}

void MT_Miner::computeMinimalTransversals(std::vector<std::shared_ptr<Itemset>>& mt)
{
	auto beginTime = std::chrono::system_clock::now();
	//SIZE_T used0 = Utils::printUsedMemoryForCrtProcess();
	
	// initialise itemset
	std::vector<std::shared_ptr<Itemset>> toTraverse;
	computeInitalToTraverseList(toTraverse);
	
	// create a graph, then compute minimal transversal from the binary representation
	TreeNode rootNode(this->useCloneOptimization);
	
	// lambda function called during parsing every 30 seconds
	auto ftr = std::async(std::launch::async, [&rootNode]() {
		const int secondsToWait = 30;
		int n = 1;
		auto beginTime = std::chrono::system_clock::now();
		while (!stop)
		{
			auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - beginTime).count();
			if(duration > secondsToWait * 1000)
			{
				std::cout << CYAN << "computing minimal transversals in progress : " << secondsToWait * n << " seconds, "
					<< rootNode.nbTasks << " tasks created, "
					<< rootNode.nbTotalMt << " minimal transverses found, " 
					<< "minimal size of minimal transverse is " << rootNode.minimalMt
					<< RESET << std::endl;
				n++;
				beginTime = std::chrono::system_clock::now();
			}
		}
	});
	

	// compute all minimal transversal from the root node
	mt = rootNode.computeMinimalTransversals(std::move(toTraverse));
	
	// stop the thread and detach it (dont not wait next n seconds)
	this->stop = true;
	ftr.get();

	std::cout << CYAN << rootNode.nbTasks << " tasks created, "
		<< rootNode.nbTotalMt << " minimal transverses found, "
		<< "minimal size of minimal transverse is " << rootNode.minimalMt
		<< RESET << std::endl;

	// print timer
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - beginTime).count();
	auto duration2 = duration / 1000.0;
	Logger::log("Found ", mt.size(), " minimal transverses in ", duration2, " s\n");

	// sort transversals itemset
	//graph_mt = sortVectorOfItemset(graph_mt);

	// print minimal transversals
	Logger::log("\nminimal transversals count : ", mt.size(), "\n");
	if (mt.size() > 6)
	{
		for_each(mt.begin(), mt.begin() + 5, [&](const std::shared_ptr<Itemset> elt) { Logger::log(elt->toString(), "\n"); });
		Logger::log("...\n");
	}
	else
		for_each(mt.begin(), mt.end(), [&](const std::shared_ptr<Itemset> elt) { Logger::log(elt->toString(), "\n"); });
	
	//Logger::log(RESET);

	// write tree into js
	//JsonTree::writeJsonNode(graph_mt);

	//SIZE_T used1 = Utils::printUsedMemoryForCrtProcess();
	//std::cout << "allocated memory " << used1 - used0 << std::endl;
}

