#pragma once
#include <vector>
#include <cassert>
#include <fstream>

#ifndef _DEBUG
#include <boost/dynamic_bitset.hpp>
#include <bitset>
#endif
#include <any>
#include <memory>

#include "Bitset.h"
#include "HyperGraph.h"

class FormalContext
{
private:
	//typedef std::bitset<ITEM_COUNT> FormalContextBitset;
	// a formal context is a matrix of bool
	//std::vector<FormalContextBitset> formalContext;
	//std::vector<std::any> formalContext;
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
		for (unsigned int i = 0, n = hypergraph->getObjectCount(); i < n; i++)
		{
			// init bitset, all 0's by default
			Bitset bitset(bitsetSize);
			
			// loop on hyper graph and build formal context
			std::vector<unsigned int> line = hypergraph->getLine(i);
			for (unsigned int j = 0, k = line.size(); j < k; j++)
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
				assert(index < bitsetSize);
				bitset.set(index);
			}
			// add bitset for this object (line)
			this->formalContext.push_back(bitset);

		}
	};

	void serialize(const std::string& outputile) const
	{
		std::ofstream fileStream = std::ofstream(outputile, std::ofstream::out);
		for (auto it = this->formalContext.begin(); it != this->formalContext.end(); it++)
		{
			for (int i = 0, n = it->size(); i < n; i++)
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
		Bitset bitset = this->formalContext[iObject];
		return bitset.get(iAttribute);
	};
};
