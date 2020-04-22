#include "MT_Miner.h"
#include "utils.h"

MT_Miner::MT_Miner(bool verbose)
{
	this->verbose = verbose;
}

MT_Miner::~MT_Miner()
{

}

void MT_Miner::init(const std::shared_ptr<HyperGraph>& hypergraph)
{
	// build formal context from hypergraph
	FormalContext formalContext(hypergraph);

	// build binary representation from formal context
	binaryRepresentation.reset();
	binaryRepresentation = std::make_shared<BinaryRepresentation>(formalContext);

	// avant de commencer l'exploration des TM tu cherches dans le fichier les colonnes clones
	// qui ont le meme support et qui couvrent exactement les memes objets
	// si tu fais le ET logique du support de la colonne avec son clone tu obtiens le meme vecteur
	// donc c'est comme �a que tu identifies ces clones une fois tu les as
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


std::vector<Utils::Itemset> MT_Miner::computeMinimalTransversals(bool showClones, const std::vector<Utils::Itemset>& toTraverse) const
{
	std::vector<Utils::Itemset> graph_mt;
	GraphNode rootNode(this->verbose, showClones, toTraverse, binaryRepresentation);
	rootNode.computeMinimalTransversals(graph_mt);
	return graph_mt;
}

