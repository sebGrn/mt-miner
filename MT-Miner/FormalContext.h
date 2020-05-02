#pragma once
#include <vector>
#include <cassert>
#include <boost/dynamic_bitset.hpp>
#include <fstream>

#include "Bitset.h"
#include "HyperGraph.h"

class FormalContext
{
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

		unsigned int bitsetSize = this->itemCount;
		if (!hypergraph->getOneBasedIndex())
			bitsetSize += 1;

		unsigned int index = 0;

		// build formal context
		for (unsigned int i = 0; i < hypergraph->getObjectCount(); i++)
		{
			// init bitset, all 0's by default
			Bitset bitset(bitsetSize);

			// loop on hyper graph and build formal context
			std::vector<unsigned int> line = hypergraph->getLine(i);
			for (unsigned int j = 0; j < line.size(); j++)
			{
				if (hypergraph->getOneBasedIndex())
				{
					assert(line[j] >= 1);
					index = line[j] - 1;
				}
				else
				{
					index = line[j];
				}
				assert(index >= 0);
				assert(index < bitset.size());
				bitset[index] = true;
			}
			this->formalContext.push_back(bitset);
		}
	};

	void serialize(const std::string& outputile) const
	{
		std::ofstream fileStream = std::ofstream(outputile, std::ofstream::out);
		for (auto it = this->formalContext.begin(); it != this->formalContext.end(); it++)
		{
			for (int i = 0; i < it->size(); i++)
			{
				fileStream << (*it)[i] ? "1" : "0";				
				fileStream << ";";
			}
			fileStream << std::endl;
		}
		fileStream.close();
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
