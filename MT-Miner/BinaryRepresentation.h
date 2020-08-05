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
class BinaryRepresentation
{
private:
	///  key/value definition of a binary represention (key as the attribute id, value as the bitset)
	static std::unordered_map<unsigned int, Itemset::Item> binaryRepresentationMap;

	//static std::unique_ptr<Itemset> binaryRepresentation;

	/// number of objects/lines
	static unsigned int objectCount;

	/// number of items/attributes/columns
	static unsigned int itemCount;

	/// count number of cloned itemsets has been removed from mt computation
	static unsigned int nbItemsetNotAddedFromClone;

private:
	//static bool compareItemsets(Itemset& itemset1, Itemset& itemset2);

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
		for (auto it = binaryRepresentationMap.begin(); it != binaryRepresentationMap.end(); it++)
		{
			StaticBitset bitset = it->second.item_bitset;
			//for (int i = 0, n = bitset.size(); i < n; i++)
			for (int i = 0, n = 32; i < n; i++)
			{
				bool bit = bitset.get(i);
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

	static Itemset::Item getBitsetFromKey(unsigned int key)
	{
		assert(binaryRepresentationMap.find(key) != binaryRepresentationMap.end());
		return binaryRepresentationMap.at(key);
	}
};