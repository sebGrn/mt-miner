#pragma once
#include <vector>
#include <string>
#include <algorithm>
#include "utils.h"
#include "Item.h"
#include "SparseBitset.h"

class Itemset
{
public:
	//std::vector<unsigned int> itemset_list;
	std::vector<Item> itemset;
	
	// true if bitset_count & or value has to be computed
	bool dirty; 
	// stored OR value of all bitset from the item set
	StaticBitset orValue;
	// support of the current itemset (nb 1's bit) of OR computation
	unsigned int bitsetCount;

	// 
	bool is_essential_computed;
	bool is_essential;
	
public:
	Itemset() : orValue()
	{
		this->bitsetCount = 0;
		this->dirty = true;
	}

	std::string toString() const;
	static std::string itemsetListToString(const std::vector<Itemset>& v);

	static Itemset combineItemset(const Itemset& str1, const Itemset& str2);

	bool operator==(const Itemset& other);
};

inline bool Itemset::operator==(const Itemset& other) 
{
	// first test if support is different, itemsets are differents
	if ((!this->dirty && !other.dirty) && (this->bitsetCount != other.bitsetCount))
		return false;

	// test each attributeIndex of itemset
	auto it1 = other.itemset.begin();
	for (auto it2 = this->itemset.begin(); it2 != this->itemset.end(); it1++, it2++)
	{
		if (it1->attributeIndex != it2->attributeIndex)
			return false;
	}
	return true;
}