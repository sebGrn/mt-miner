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

	/// pair of original index, clone index
	std::vector<std::pair<unsigned int, unsigned int>> clonedBitsetIndexes;

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
		unsigned int disSupp = 0;
		Bitset SumOfN_1Items(this->objectCount);

		for (int i = 0; i < pattern.size(); i++)
		{
			unsigned int columnKey = pattern[i];
			Bitset bitset = this->getElement(columnKey);
#ifdef _DEBUG
			for (int j = 0; j < this->objectCount; j++)
			{
				SumOfN_1Items[j] = SumOfN_1Items[j] || bitset[j];
			}
#else
			SumOfN_1Items = SumOfN_1Items | this->binaryRepresentation->getElement(columnKey);
#endif			
		}
		for (int i = 0; i < this->objectCount; i++)
		{
			if (SumOfN_1Items[i] == 1)
				disSupp++;
		}
		return disSupp;
	};

	bool compareItemsets(const Utils::Itemset& itemset1, const Utils::Itemset& itemset2) const
	{
		bool sameItemset = true;
		unsigned int supp1 = computeDisjonctifSupport(itemset1);
		unsigned int supp2 = computeDisjonctifSupport(itemset2);
		if (supp1 != supp2)
			sameItemset = false;
		else
		{
			for (int i = 0; i < itemset1.size(); i++)
			{
				unsigned int columnKey_itemset1 = itemset1[i];
				unsigned int columnKey_itemset2 = itemset2[i];
				
				Bitset bitset1 = this->getElement(columnKey_itemset1);
				Bitset bitset2 = this->getElement(columnKey_itemset2);
#ifdef _DEBUG
				Bitset result;
				for (int j = 0; j < this->objectCount; j++)
				{
					result.push_back(bitset1[j] && bitset2[j]);
				}
				sameItemset = ((result == bitset1) && (result == bitset2));
#else
				Bitset result = bitset1 & bitset2;
				sameItemset = ((result == bitset1) && (result == bitset2));
#endif			
			}
		}
		return sameItemset;
	}

	void buildCloneList()
	{
		for (auto it1 = this->binaryRepresentation.begin(); it1 != this->binaryRepresentation.end(); it1++)
		{
			for (auto it2 = it1; it2 != this->binaryRepresentation.end(); it2++)
			{
				if (it1 != it2)
				{
					if (it1->second == it2->second)
					{
						// push a clone <original index, clone index>
						clonedBitsetIndexes.push_back(std::pair<unsigned int, unsigned int>(it1->first, it2->first));
					}
				}
			}
		}
	};

	bool containsAClone(const Utils::Itemset& itemset, unsigned int& originalIndex, unsigned int& clonedIndex)
	{
		for (auto it = clonedBitsetIndexes.begin(); it != clonedBitsetIndexes.end(); it++)
		{
			// check if 
			if (std::find(itemset.begin(), itemset.end(), it->second) != itemset.end())
			{
				originalIndex = it->first;
				clonedIndex = it->second;
				return true;
			}
		}
		return false;
	}

	bool containsAnOriginal(const Utils::Itemset& itemset, unsigned int& originalIndex, unsigned int& clonedIndex)
	{
		for (auto it = clonedBitsetIndexes.begin(); it != clonedBitsetIndexes.end(); it++)
		{
			// check if 
			if (std::find(itemset.begin(), itemset.end(), it->first) != itemset.end())
			{
				originalIndex = it->first;
				clonedIndex = it->second;
				return true;
			}
		}
		return false;
	}

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