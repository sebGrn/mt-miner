#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <sstream>

#include "MT_Miner.h"

/// this class parse a hypergraph and store it
/// an hypergraph is a list of int
/// object count 
class HypergraphParser
{
private:
	/// hypergraph ie matrix of int
	std::vector<std::vector<unsigned int>> hypergraph;

	/// number of objects/lines
	unsigned int objectCount;

	/// number of items/attributes/columns
	unsigned int itemCount;

public:
	HypergraphParser();
	~HypergraphParser();

	/// parse a hypergraph and store it
	bool parse(const std::string& file);

	std::vector<std::vector<unsigned int>> getHypergraph() const
	{
		return this->hypergraph;
	};

	unsigned int getItemCount() const
	{
		return this->itemCount;
	};

	unsigned int getObjectCount() const
	{
		return this->objectCount;
	}
};

