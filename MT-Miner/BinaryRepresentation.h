#pragma once

#include <vector>
#include <map>
#include <list>
#include <boost/dynamic_bitset.hpp>

#include "utils.h"
#include "Bitset.h"
#include "FormalContext.h"

class BinaryRepresentation
{
private:
	/// a binary representation is a formal context representation in columns
	/// each vector contains the attributes/items boolean values
	/// the index of the map is the item number
	/// this representation is usefull to compute disjonctif support easyly (with a OR operator)
	///  - TODO: not sure if a map is usefull here
	std::map<unsigned int, Bitset> binaryRepresentation;

	/// number of objects/lines
	unsigned int objectCount;

	/// number of items/attributes/columns
	unsigned int itemCount;

	/// pair of original index, clone index
	std::vector<std::pair<unsigned int, unsigned int>> clonedBitsetIndexes;

private:
	bool checkOneItem(int itemBar, const Utils::Itemset& itemsOfpattern) const;
	bool compareItemsets(const Utils::Itemset& itemset1, const Utils::Itemset& itemset2) const;

public:
	/// build binary representation from formal context
	BinaryRepresentation(const FormalContext& context);
	///
	unsigned int computeDisjonctifSupport(const Utils::Itemset& pattern) const;
	/// return true if element is essential
	bool isEssential(const Utils::Itemset& itemsOfPattern) const;
	///
	void buildCloneList();
	///
	bool containsAClone(const Utils::Itemset& itemset) const;
	///
	bool containsOriginals(const Utils::Itemset& itemset, std::vector<std::pair<unsigned int, unsigned int>>& originalClonedIndexes) const;

	unsigned int getItemCount() const
	{
		return this->itemCount;
	};

	unsigned int getObjectCount() const
	{
		return this->objectCount;
	};

	Bitset getBitset(unsigned int key) const
	{
		assert(binaryRepresentation.find(key) != binaryRepresentation.end());
		return binaryRepresentation.at(key);
	}
};