#pragma once
#include <vector>
#include <cassert>
#include <fstream>
#include <memory>

#include "Bitset.h"
#include "HyperGraph.h"

template <class T>
class FormalContext
{
private:
	// a formal context is a matrix of bool
	std::vector<T> formalContext;

	//std::vector<Bitset> formalContext;
	
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
			T bitset(bitsetSize);
						
			// loop on hyper graph and build formal context
			std::vector<unsigned int> line = hypergraph->getLine(i);
			for (size_t j = 0, k = line.size(); j < k; j++)
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
		auto& bitset = this->formalContext[iObject];
		return bitset.get(iAttribute);
	};
};

// --------------------------------------------------------------------------------------------------------------------------------- //
// --------------------------------------------------------------------------------------------------------------------------------- //

// template implementation
template class FormalContext<StaticBitset<std::bitset<SIZE_0>>>;
template class FormalContext<StaticBitset<std::bitset<SIZE_1>>>;
template class FormalContext<StaticBitset<std::bitset<SIZE_2>>>;
template class FormalContext<StaticBitset<std::bitset<SIZE_3>>>;
template class FormalContext<StaticBitset<std::bitset<SIZE_4>>>;
template class FormalContext<StaticBitset<std::bitset<SIZE_5>>>;
template class FormalContext<StaticBitset<std::bitset<SIZE_6>>>;
template class FormalContext<CustomBitset>;
#ifdef _WIN32
template class FormalContext<VariantBitset>; 
template class FormalContext<AnyBitset>;
#endif

// --------------------------------------------------------------------------------------------------------------------------------- //
// --------------------------------------------------------------------------------------------------------------------------------- //

//typedef FormalContext<AnyBitset> FormalContext_impl;
//typedef FormalContext<StaticBitset<std::bitset<SIZE_0>>> FormalContext_impl;
typedef FormalContext<CustomBitset> FormalContext_impl;
//typedef FormalContext<VariantBitset> FormalContext_impl;

