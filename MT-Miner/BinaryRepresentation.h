#pragma once

#include <vector>
#include <map>
#include <boost/dynamic_bitset.hpp>

#include "FormalContext.h"

class BinaryRepresentation
{
	using Bitset = boost::dynamic_bitset<>;

private:
	/// a binary representation is a formal context representation in columns
	/// each vector contains the attributes/items boolean values
	/// the index of the map is the item number
	/// this representation is usefull to compute disjonctif support easyly (with a OR operator)
	///  - TODO: we should use a dynamic bitset here to optimise & operation
	///  - TODO: not sure if a map is usefull here
	std::map<unsigned int, Bitset> binaryRepresentation;

	/// number of objects/lines
	unsigned int objectCount;

	/// number of items/attributes/columns
	unsigned int itemCount;

public:
	/// build binary representation from formal context
	BinaryRepresentation(const FormalContext& context)
	{
		this->objectCount = context.getObjectCount();
		this->itemCount = context.getItemCount();

		for (unsigned int j = 0; j < this->itemCount; j++)			// 8 on test.txt
		{
			Bitset bitset(this->objectCount);
			for (unsigned int i = 0; i < this->objectCount; i++)	// 6 on test.txt
			{
				bitset[i] = context.getElement(i, j);
			}
			this->binaryRepresentation[j + 1] = bitset;
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

	Bitset getElement(unsigned int key) const
	{
		assert(binaryRepresentation.find(key) != binaryRepresentation.end());
		return binaryRepresentation.at(key);
	}
};