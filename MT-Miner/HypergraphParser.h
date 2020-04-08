#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <sstream>

#include "MT_Miner.h"

///
class HypergraphParser
{
private:
	std::vector<std::vector<unsigned int>> hypergraph;
	unsigned int objectCount;
	unsigned int itemCount;

public:
	HypergraphParser();
	~HypergraphParser();

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

