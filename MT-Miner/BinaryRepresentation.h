#pragma once

#include <vector>
#include <map>
#include <list>
#include <chrono>

#ifndef _DEBUG
#include <boost/dynamic_bitset.hpp>
#endif

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
	std::map<unsigned int, Bitset> binaryRepresentation;

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
			for (int i = 0; i < it->second.size(); i++)
			{
				fileStream << (it->second)[i] ? "1" : "0";
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

	Bitset getBitset(unsigned int key) const
	{
		assert(binaryRepresentation.find(key) != binaryRepresentation.end());
		return binaryRepresentation.at(key);
	}
};