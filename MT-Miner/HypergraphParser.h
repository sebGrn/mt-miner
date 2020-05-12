#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <sstream>
#include <chrono>

#include "Logger.h"
#include "HyperGraph.h"
#include "MT_Miner.h"

/**
 * Load and parse a hypergraph and stores it
 */
class HypergraphParser
{
private:
	/// hypergraph ie matrix of int
	std::shared_ptr<HyperGraph> hypergraph;
	bool parsingDone;

public:
	HypergraphParser();
	~HypergraphParser();

	/// parse a hypergraph and store it
	bool parse(const std::string& file);

	std::shared_ptr<HyperGraph> getHypergraph() const
	{
		return this->hypergraph;
	};

	unsigned int getItemCount() const
	{
		return this->hypergraph->getItemCount();
	};

	unsigned int getObjectCount() const
	{
		return this->hypergraph->getObjectCount();
	};
};

