#pragma once

#include <unordered_map>


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
	std::unordered_map<unsigned int, T> binaryRepresentation;

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
	BinaryRepresentation(const FormalContext_impl& context);
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
			for (int i = 0, n = it->second.size(); i < n; i++)
			{
				fileStream << it->second.get(i) ? "1" : "0";
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

	T getBitsetFromKey(unsigned int key) const
	{
		assert(binaryRepresentation.find(key) != binaryRepresentation.end());
		return binaryRepresentation.at(key);
	}
};