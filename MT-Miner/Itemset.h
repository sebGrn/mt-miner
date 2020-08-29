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
	std::vector<std::shared_ptr<Item>> itemset;
	
	// true if bitset_count & or value has to be computed
	bool dirty; 
	// stored OR value of all bitset from the item set
	StaticBitset orValue;
	// support of the current itemset (nb 1's bit) of OR computation
	unsigned int bitsetCount;
	
public:
	Itemset()
	{
		this->bitsetCount = 0;
		this->dirty = true;
	}

	Itemset(const Itemset* itemset)
	{
		this->bitsetCount = itemset->bitsetCount;
		this->dirty = itemset->dirty;
		// copy items
		for (auto it = itemset->itemset.begin(); it != itemset->itemset.end(); it++)
			this->itemset.push_back(std::make_shared<Item>(it->get()));
	}

	std::string toString() const;
	//static std::string itemsetListToString(const std::vector<Itemset>& v);

	static std::shared_ptr<Itemset> combineItemset(const std::shared_ptr<Itemset>& str1, const std::shared_ptr<Itemset>& str2);

	unsigned int getItemCount() const;
	std::shared_ptr<Item> getItem(unsigned int i) const;
	void replaceItem(unsigned int i, const std::shared_ptr<Item>& item);

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
		if ((*it1)->attributeIndex != (*it2)->attributeIndex)
			return false;
	}
	return true;
}

inline unsigned int Itemset::getItemCount() const
{
	return static_cast<unsigned int>(this->itemset.size());
}

inline std::shared_ptr<Item> Itemset::getItem(unsigned int i) const
{
	assert(i < this->itemset.size());
	return this->itemset[i];
}

inline void Itemset::replaceItem(unsigned int i, const std::shared_ptr<Item>& item)
{
	assert(i < this->itemset.size());
	if (i < this->itemset.size())
	{
		// remove previous element
		this->itemset.erase(this->itemset.begin() + i);
		// insert element
		this->itemset.insert(this->itemset.begin() + i, item);
	}
}