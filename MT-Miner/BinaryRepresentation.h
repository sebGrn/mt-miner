#pragma once

#include <unordered_map>

#include "utils.h"
#include "Bitset.h"
#include "FormalContext.h"

#define BITSET_SIZE				32
#define GET_BIT(bitset, i)		(bitset >> i) & 1UL
#define SET_BIT(bitset, bit, i)	(bitset |= (bit ? 1UL : 0UL) << i)
#define COUNT_BIT(bitset)		BinaryRepresentation::countBit(bitset)

/** 
 * a binary representation is a formal context representation in columns
 * each vector contains the attributes/items boolean values
 * the index of the map is the item number
 * this representation is usefull to compute disjonctif support easyly (with a OR operator)
 */
class BinaryRepresentation
{
private:
	///  key/value definition of a binary represention (key as the attribute id, value as the bitset)
	std::unordered_map<unsigned int, unsigned long> binaryRepresentation;	

	/// number of objects/lines
	unsigned int objectCount;

	/// number of items/attributes/columns
	unsigned int itemCount;

	/// count number of cloned itemsets has beend removed from mt computation
	unsigned int nbItemsetNotAddedFromClone;

	/// pair of original index, clone index
	std::vector<std::pair<unsigned int, unsigned int>> clonedBitsetIndexes;

private:
	bool compareItemsets(const Itemset& itemset1, const Itemset& itemset2) const;

public:
	/// build binary representation from formal context
	BinaryRepresentation(const FormalContext& context);
	///
	~BinaryRepresentation();
	///
	unsigned int computeDisjonctifSupport(const Itemset& pattern) const;
	/// return true if element is essential
	bool isEssential(const Itemset& itemsOfPattern);
	///
	unsigned int buildCloneList();
	///
	bool containsAClone(const Itemset& itemset);
	///
	bool containsOriginals(const Itemset& itemset, std::vector<std::pair<unsigned int, unsigned int>>& originalClonedIndexes) const;

	//
	void serialize(const std::string& outputile) const
	{
		std::ofstream fileStream = std::ofstream(outputile, std::ofstream::out);
		for (auto it = this->binaryRepresentation.begin(); it != this->binaryRepresentation.end(); it++)
		{
			unsigned long bitset = it->second;
			for (int i = 0, n = BITSET_SIZE; i < n; i++)
			{				
				bool bit = (bitset >> i) & 1ULL;
				fileStream << bit ? "1" : "0";
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

	unsigned long getBitsetFromKey(unsigned int key) const
	{
		assert(binaryRepresentation.find(key) != binaryRepresentation.end());
		return binaryRepresentation.at(key);
	}

	static unsigned int countBit(unsigned long bitset)
	{
		unsigned int count(0);
		unsigned long int n(bitset);
		while (n)
		{
			n &= (n - 1);
			count++;
		}
		return count;
	}
};