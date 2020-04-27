#include "MT_Miner.h"
#include "utils.h"

MT_Miner::MT_Miner(bool useCloneOptimization)
{
	this->useCloneOptimization = useCloneOptimization;
}

MT_Miner::~MT_Miner()
{

}

void MT_Miner::init(const std::shared_ptr<HyperGraph>& hypergraph, std::vector<Utils::Itemset>& toTraverse)
{
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
		binaryRepresentation->buildCloneList();
	}

	toTraverse.clear();
	for (unsigned int i = 1; i <= formalContext.getItemCount(); i++)
	{
		Utils::Itemset itemset(1, i);
		if (!this->binaryRepresentation->containsAClone(itemset))	
			toTraverse.push_back(itemset);
	}
}


std::vector<Utils::Itemset> MT_Miner::computeMinimalTransversals(const std::vector<Utils::Itemset>& toTraverse) const
{
	// create a graph, then compute minimal transversal from the binary representation
	std::vector<Utils::Itemset> graph_mt;
	TreeNode rootNode(this->useCloneOptimization, toTraverse, binaryRepresentation);
	rootNode.computeMinimalTransversals(graph_mt);
	return graph_mt;
}

