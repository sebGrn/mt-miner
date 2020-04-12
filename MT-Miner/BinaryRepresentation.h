#pragma once

#include <vector>
#include <map>
#include <list>
#include <boost/dynamic_bitset.hpp>

#include "Bitset.h"
#include "FormalContext.h"

class BinaryRepresentation
{
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

	/// contains the list of column indexes which have the same disjoncif support 
	/// these are clones
	//std::vector<std::list<unsigned int>> cloneList;

private:
	/*bool checkClone(unsigned int currentKey, const Bitset& currentBitset)
	{
		if (binaryRepresentation.empty())
			return false;

		bool clone = true;
		unsigned int cloneKey = 0;
		std::list<unsigned int> indexBitsetClone;
		for (const auto& [key, bitset] : binaryRepresentation)
		{
#ifdef _DEBUG
			// reset clone bolean
			clone = true;
			// loop on bits for bitset
			for (unsigned int i = 0; i < bitset.size(); i++)
			{
				assert(i < currentBitset.size());
				if (currentBitset[i] != bitset[i])
				{
					clone = false;
					break;
				}
			}
			if (clone) 
			{
				// columns at key and currentKey are the same
				indexBitsetClone.push_back(key);
				indexBitsetClone.push_back(currentKey);
			}
#else
			clone = bitset & currentBitset;
			if (clone)
			{
				// columns at key and currentKey are the same
				indexBitsetClone.push_back(key);
				indexBitsetClone.push_back(currentKey);
			}
#endif
		}
		if (clone)
		{
			// sort and remove duplicate elements
			indexBitsetClone.sort();
			indexBitsetClone.unique();

			// store list of clones indexes
			cloneList.push_back(indexBitsetClone);
		}

		return clone;
	};*/

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
			unsigned int currentKey = j + 1;
			this->binaryRepresentation[currentKey] = bitset;

			/*// check if this bitset has not a clone
			if (!checkClone(currentKey, bitset))
			{
				this->binaryRepresentation[currentKey] = bitset;
			}*/
		}
	};

	bool checkOneItem(int itemBar, const Utils::Itemset& itemsOfpattern) const
	{
		Bitset SumOfN_1Items(this->objectCount);

		for_each(itemsOfpattern.begin(), itemsOfpattern.end(), [&](unsigned int elt) {
			if (elt != itemBar)
			{
#ifdef _DEBUG
				for (int j = 0; j < this->objectCount; j++)
				{
					SumOfN_1Items[j] = SumOfN_1Items[j] || this->getElement(elt)[j];
				}
#else
				SumOfN_1Items = SumOfN_1Items | this->binaryRepresentation->getElement(elt);
#endif
			}
			});

		Bitset bitset = this->getElement(itemBar);
		for (int i = 0; i < this->objectCount; i++)
		{
			if (SumOfN_1Items[i] == false && bitset[i] == true)
				return true;
		}
		return false;
	}

	// return true if element is essential
	bool isEssential(const Utils::Itemset& itemsOfPattern) const
	{
		if (itemsOfPattern.size() == 1)
			return true;

		for (int i = 0; i < itemsOfPattern.size(); i++)
		{
			if (!checkOneItem(itemsOfPattern[i], itemsOfPattern))
				return false;
		}
		return true;
	}

	unsigned int computeDisjonctifSupport(const Utils::Itemset& pattern) const
	{
		Bitset SumOfN_1Items(this->objectCount);

		for (int i = 0; i < pattern.size(); i++)
		{
			unsigned int columnKey = pattern[i];
#ifdef _DEBUG
			for (int j = 0; j < this->objectCount; j++)
			{
				SumOfN_1Items[j] = SumOfN_1Items[j] || this->getElement(columnKey)[j];
			}
#else
			SumOfN_1Items = SumOfN_1Items | this->binaryRepresentation->getElement(columnKey);
#endif			
		}
		unsigned int disSupp = 0;
		for (int i = 0; i < this->objectCount; i++)
		{
			if (SumOfN_1Items[i] == 1)
				disSupp++;
		}
		return disSupp;
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