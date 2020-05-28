#include "MT_Miner.h"
#include "utils.h"
#include "Profiler.h"
#include "JsonTree.h"

std::atomic_bool MT_Miner::stop(false);

MT_Miner::MT_Miner(const std::shared_ptr<HyperGraph>& hypergraph, bool useCloneOptimization)
{
	this->useCloneOptimization = useCloneOptimization;
	
	createBinaryRepresentation(hypergraph);
}

MT_Miner::~MT_Miner()
{

}

void MT_Miner::createBinaryRepresentation(const std::shared_ptr<HyperGraph>& hypergraph)
{
	// build formal context from hypergraph
	FormalContext_impl formalContext(hypergraph);
	formalContext.serialize("format_context.csv");

	// build binary representation from formal context
	binaryRepresentation.reset();
	binaryRepresentation = std::make_shared<BinaryRepresentation_impl>(formalContext);
	binaryRepresentation->serialize("binary_rep.csv");

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
		Logger::log(GREEN, "computing clones\n", RESET);
		unsigned int cloneListSize = binaryRepresentation->buildCloneList();
		Logger::log(GREEN, "found ", cloneListSize, " clones\n", RESET);

		if (cloneListSize == 0)
			this->useCloneOptimization = false;
	}
}

ItemsetList MT_Miner::computeInitalToTraverseList()
{
	ItemsetList toTraverse;
	for (unsigned int i = 1; i <= this->binaryRepresentation->getItemCount(); i++)
	{
		Itemset itemset(1, i);
		if (!this->binaryRepresentation->containsAClone(itemset))	
			toTraverse.push_back(itemset);
	}
	return toTraverse;
}

ItemsetList MT_Miner::computeMinimalTransversals()
{
	auto beginTime = std::chrono::system_clock::now();

	ItemsetList toTraverse = computeInitalToTraverseList();

	// create a graph, then compute minimal transversal from the binary representation
	TreeNode rootNode(this->useCloneOptimization, this->binaryRepresentation);

	// lambda function called during parsing every 20 seconds
	auto ftr = std::async(std::launch::async, [&rootNode]() {
		const int secondsToWait = 20;
		int n = 0;
		while (!stop)
		{
			if (n)
				std::cout << CYAN << "computing minimal transversals in progress : " << secondsToWait * n << " seconds, " 
				<< rootNode.getTotalChildren() << " nodes created"
				<< RESET << std::endl;
			std::this_thread::sleep_for(std::chrono::seconds(secondsToWait));
			n++;
		}
	});

	// compute all minimal transversal from the root node
	std::vector<Itemset>&& graph_mt = rootNode.computeMinimalTransversals_recursive(toTraverse);
	//std::vector<Itemset>&& graph_mt = rootNode.computeMinimalTransversals_iterative(toTraverse);
	
	// stop the thread and detach it (dont not wait next n seconds)
	this->stop = true;
	ftr.get();

	// print timer
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - beginTime).count();
	Logger::log(YELLOW, "computing minimal transversals done in ", duration, " ms\n", RESET);
	for(auto it = Profiler::functionDurationMap.begin(); it != Profiler::functionDurationMap.end(); it++)
		std::cout << CYAN << "total duration of " << it->first << " : " << it->second.count() << " ms" << RESET << std::endl;

	// sort transversals itemset
	//graph_mt = sortVectorOfItemset(graph_mt);

	// print minimal transversals
	Logger::log(YELLOW, "\nminimal transversals count : ", graph_mt.size(), "\n", RESET);
	if (graph_mt.size() > 6)
	{
		for_each(graph_mt.begin(), graph_mt.begin() + 5, [&](const Itemset& elt) { Logger::log(GREEN, Utils::itemsetToString(elt), "\n", RESET); });
		Logger::log(GREEN, "...\n", RESET);
	}
	else
		for_each(graph_mt.begin(), graph_mt.end(), [&](const Itemset& elt) { Logger::log(GREEN, Utils::itemsetToString(elt), "\n", RESET); });

	// write tree into js
	//JsonTree::writeJsonNode(graph_mt);
	
	return graph_mt;
}

