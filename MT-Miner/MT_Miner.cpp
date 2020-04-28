#include "MT_Miner.h"
#include "utils.h"

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
	// build formal context from hypergraph
	auto beginTime = std::chrono::system_clock::now();
	FormalContext formalContext(hypergraph);
	//formalContext.serialize("format_context.csv");
	int64_t duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - beginTime).count();
	Logger::log("building formal context from hypergraph ", duration, " ms\n\n");

	// build binary representation from formal context
	beginTime = std::chrono::system_clock::now();
	binaryRepresentation.reset();
	binaryRepresentation = std::make_shared<BinaryRepresentation>(formalContext);
	//binaryRepresentation->serialize("binary_rep.csv");
	duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - beginTime).count();
	Logger::log("building binary representation from formalcontext done in ", duration, " ms\n\n");

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
		beginTime = std::chrono::system_clock::now();
		unsigned int cloneListSize = binaryRepresentation->buildCloneList();
		duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - beginTime).count();
		Logger::log("detecting bitset clones in binary representation done in ", duration, " ms - found ", cloneListSize, " clones\n\n");
	}

	toTraverse.clear();
	for (unsigned int i = 1; i <= formalContext.getItemCount(); i++)
	{
		Utils::Itemset itemset(1, i);
		if (!this->binaryRepresentation->containsAClone(itemset))	
			toTraverse.push_back(itemset);
	}
	duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - beginTime).count();
	Logger::log("removing bitset clones in initial toTraverse list done in ", duration, " ms\n\n");
}


std::vector<Utils::Itemset> MT_Miner::computeMinimalTransversals(const std::vector<Utils::Itemset>& toTraverse)
{
	// lambda function called during parsing every minutes
	auto callback = [](bool& done, std::vector<Utils::Itemset>& graph_mt) {
		int n = 0;
		while (!done)
		{
			if (n)
				std::cout << "computing minimal transversals in progress : " << graph_mt.size() << " minimal transversals found after " << 30 * n << " seconds" << std::endl;
			std::this_thread::sleep_for(std::chrono::seconds(30));
			n++;
		}
	};

	std::vector<Utils::Itemset> graph_mt;
	
	// instanciate new thead for regular log
	std::thread thread(callback, std::ref(this->computeMtDone), std::ref(graph_mt));

	// create a graph, then compute minimal transversal from the binary representation
	TreeNode rootNode(this->useCloneOptimization, toTraverse, binaryRepresentation);
	auto beginTime = std::chrono::system_clock::now();
	rootNode.computeMinimalTransversals(graph_mt);
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - beginTime).count();
	Logger::log("computing minimal transversals done in ", duration, " ms\n\n");
	this->computeMtDone = true;

	thread.detach();

	return graph_mt;
}

