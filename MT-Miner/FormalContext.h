#pragma once
#include <vector>
#include <cassert>
#include <boost/dynamic_bitset.hpp>

#include "HyperGraph.h"

class FormalContext
{
	using Bitset = boost::dynamic_bitset<>;

private:
	// a formal context is a matrix of bool
	std::vector<Bitset> formalContext;
	// number of columns / number of boolean values
	unsigned int itemCount;
	// number of lines
	unsigned int objectCount;

public:
	FormalContext(const std::shared_ptr<HyperGraph>& hypergraph)
	{
		this->itemCount = hypergraph->getItemCount();
		this->objectCount = hypergraph->getObjectCount();

		// build formal context
		for (unsigned int i = 0; i < hypergraph->getObjectCount(); i++)
		{
			// init bitset, all 0's by default
			Bitset bitset(hypergraph->getItemCount());

			// loop on hyper graph and build formal context
			std::vector<unsigned int> line = hypergraph->getLine(i);
			for (unsigned int j = 0; j < line.size(); j++)
			{
				assert(line[j] >= 1);
				unsigned int index = line[j] - 1;
				//unsigned int index = line[j];
				assert(index < bitset.size());
				bitset[index] = true;
			}
			this->formalContext.push_back(bitset);
		}
	};

	unsigned int getItemCount() const
	{
		return this->itemCount;
	};

	unsigned int getObjectCount() const
	{
		return this->objectCount;
	};

	unsigned int getElement(unsigned int i, unsigned int j) const
	{
		assert(i < this->formalContext.size());
		assert(j < this->formalContext[i].size());
		return this->formalContext[i][j];
	};
};
