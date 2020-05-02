#include "MT_Miner.h"
#include "utils.h"
#include "Profiler.h"

MT_Miner::MT_Miner(bool useCloneOptimization)
{
	this->useCloneOptimization = useCloneOptimization;
	this->computeMtDone = false;
}

MT_Miner::~MT_Miner()
{

}

void MT_Miner::init(const std::shared_ptr<HyperGraph>& hypergraph, std::vector<Utils::Itemset>& toTraverse)
{
	START_PROFILING(__func__)
	// build formal context from hypergraph
	FormalContext formalContext(hypergraph);
	//formalContext.serialize("format_context.csv");

	// build binary representation from formal context
	binaryRepresentation.reset();
	binaryRepresentation = std::make_shared<BinaryRepresentation>(formalContext);
	//binaryRepresentation->serialize("binary_rep.csv");

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
		unsigned int cloneListSize = binaryRepresentation->buildCloneList();
	}

	toTraverse.clear();
	for (unsigned int i = 1; i <= formalContext.getItemCount(); i++)
	{
		Utils::Itemset itemset(1, i);
		if (!this->binaryRepresentation->containsAClone(itemset))	
			toTraverse.push_back(itemset);
	}
	END_PROFILING(__func__)
}


std::vector<Utils::Itemset> MT_Miner::computeMinimalTransversals(const std::vector<Utils::Itemset> && toTraverse)
{
	// lambda function called during parsing every minutes
	auto callback = [](bool& done, const TreeNode& treeNode) {
		
		const int secondsToWait = 20;
		int n = 0;
		while (!done)
		{
			if (n)
				std::cout << CYAN << "computing minimal transversals in progress : " << secondsToWait * n << " seconds, " << treeNode.getTotalChildren() << " nodes created" << RESET << std::endl;
				
			std::this_thread::sleep_for(std::chrono::seconds(secondsToWait));
			n++;
		}
	};

	auto beginTime = std::chrono::system_clock::now();

	// create a graph, then compute minimal transversal from the binary representation
	TreeNode rootNode(this->useCloneOptimization, this->binaryRepresentation);

	// instanciate new thead for regular log
	std::thread thread(callback, std::ref(this->computeMtDone), std::ref(rootNode));

	// compute all minimal transversal from the root node
	std::vector<Utils::Itemset>&& graph_mt = rootNode.computeMinimalTransversals(toTraverse);
	
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - beginTime).count();
	Logger::log(YELLOW, "computing minimal transversals done in ", duration, " ms\n", RESET);
	for(auto it = Profiler::functionDurationMap.begin(); it != Profiler::functionDurationMap.end(); it++)
		std::cout << CYAN << "total duration of " << it->first << " : " << it->second.count() << " ms" << RESET << std::endl;

	// stop the thread and detach it (dont not wait next n seconds)
	this->computeMtDone = true;
	thread.detach();

	// sort transversals itemset
	//graph_mt = sortVectorOfItemset(graph_mt);

	// print minimal transversals
	Logger::log(YELLOW, "\nminimal transversals count : ", graph_mt.size(), "\n", RESET);
	if (graph_mt.size() > 6)
	{
		for_each(graph_mt.begin(), graph_mt.begin() + 5, [&](const Utils::Itemset& elt) { Logger::log(GREEN, Utils::itemsetToString(elt), "\n", RESET); });
		Logger::log(GREEN, "...\n", RESET);
	}
	else
		for_each(graph_mt.begin(), graph_mt.end(), [&](const Utils::Itemset& elt) { Logger::log(GREEN, Utils::itemsetToString(elt), "\n", RESET); });
	
	return graph_mt;
}

