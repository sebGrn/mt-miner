#pragma once
#include <vector>
#include <cassert>
#include <fstream>
#include <memory>

#include "SparseBitset.h"
#include "HyperGraph.h"

class FormalContext
{
private:
	// a formal context is a matrix of bool
	std::vector<SparseBitset> formalContext;

	// number of columns / number of boolean values
	unsigned int itemCount;
	
	// number of lines
	unsigned int objectCount;

public:
	FormalContext(const HyperGraph& hypergraph)
	{
		this->itemCount = hypergraph.getItemCount();
		this->objectCount = hypergraph.getObjectCount();

		unsigned int index = 0;
		SparseBitset bitset;
		// build formal context
		for (unsigned int i = 0, n = hypergraph.getObjectCount(); i < n; i++)
		{
			bitset.reset();						
			// loop on hyper graph and build formal context
			std::vector<unsigned int> line = hypergraph.getLine(i);
			for (size_t j = 0, k = line.size(); j < k; j++)
			{
				if (hypergraph.getOneBasedIndex())
				{
					assert(line[j] >= 1);
					index = line[j] - 1;
				}
				else
				{
					index = line[j];
				}
				assert(index >= 0);
				bitset.set(index);				
			}
			// add bitset for this object (line)
			this->formalContext.push_back(bitset);
		}
	};

	void serialize(const std::string& outputile) const
	{
		unsigned int bitsetSize = this->itemCount;

		std::ofstream fileStream = std::ofstream(outputile, std::ofstream::out);
		for (auto it = this->formalContext.begin(); it != this->formalContext.end(); it++)
		{
			for (int i = 0, n = bitsetSize; i < n; i++)
			{
				fileStream << it->get(i) ? "1" : "0";
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

	/// get boolean value from formal context
	/// @param iObject object index (ie line number)
	/// @param iAttribute attribute index (ie column number)
	bool getBit(unsigned int iObject, unsigned int iAttribute) const
	{
		assert(iObject < this->formalContext.size());
		auto& bitset = this->formalContext[iObject];
		return bitset.get(iAttribute);
	};
};

