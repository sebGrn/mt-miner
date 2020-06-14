#pragma once

#include <unordered_map>

#include "Itemset.h"
#include "utils.h"
#include "Bitset.h"
#include "FormalContext.h"

/** 
 * a binary representation is a formal context representation in columns
 * each vector contains the attributes/items boolean values
 * the index of the map is the item number
 * this representation is usefull to compute disjonctif support easyly (with a OR operator)
 */
template <class T>
class BinaryRepresentation
{
private:
	///  key/value definition of a binary represention (key as the attribute id, value as the bitset)
	static std::unordered_map<unsigned int, T> binaryRepresentation;

	/// number of objects/lines
	static unsigned int objectCount;

	/// number of items/attributes/columns
	static unsigned int itemCount;

	/// count number of cloned itemsets has beend removed from mt computation
	static unsigned int nbItemsetNotAddedFromClone;

	/// pair of original index, clone index
	static std::vector<std::pair<unsigned int, unsigned int>> clonedBitsetIndexes;

private:
	static bool compareItemsets(Itemset& itemset1, Itemset& itemset2);

public:
	/// build binary representation from formal context
	static void buildFromFormalContext(const FormalContext& context);
	///
	static unsigned int computeDisjonctifSupport(Itemset& pattern);
	/// return true if element is essential
	static bool isEssential(Itemset& itemsOfPattern);
	///
	static unsigned int buildCloneList();
	///
	static bool containsAClone(const Itemset& itemset);
	///
	static bool containsOriginals(const Itemset& itemset, std::vector<std::pair<unsigned int, unsigned int>>& originalClonedIndexes);

	//
	static void serialize(const std::string& outputile)
	{
		std::ofstream fileStream = std::ofstream(outputile, std::ofstream::out);
		for (auto it = binaryRepresentation.begin(); it != binaryRepresentation.end(); it++)
		{
			T bitset = it->second;
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

	static unsigned int getItemCount()
	{
		return itemCount;
	};

	static unsigned int getObjectCount()
	{
		return objectCount;
	};

	static unsigned long getBitsetFromKey(unsigned int key)
	{
		assert(binaryRepresentation.find(key) != binaryRepresentation.end());
		return binaryRepresentation.at(key);
	}
};