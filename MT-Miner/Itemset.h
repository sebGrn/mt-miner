#pragma once
#include <vector>
#include <string>
#include <algorithm>
#include "utils.h"
#include "Item.h"
#include "SparseBitset.h"

class Itemset
{
private:
	std::vector<std::shared_ptr<Item>> itemset;

public:
	// true if bitset_count & or value has to be computed
	bool dirty; 
	// stored OR value of all bitset from the item set
	StaticBitset orValue;
	// support of the current itemset (nb 1's bit) of OR computation
	unsigned int orSupport;

	// stored AND value of all bitset from the item set
	StaticBitset andValue;
	// support of the current itemset (nb 1's bit) of AND computation
	unsigned int andSupport;

	bool isEssential;
	
public:
	Itemset()
	{
		this->orSupport = 0;
		this->andSupport = 0;
		this->dirty = true;
		this->isEssential = false;
	}

	Itemset(const Itemset* itemset)
	{
		this->orValue = itemset->orValue;
		this->andValue = itemset->andValue;
		this->orSupport = itemset->orSupport;
		this->andSupport = itemset->andSupport;
		this->dirty = itemset->dirty;
		this->isEssential = itemset->isEssential;
		// copy items
		for (auto it = itemset->itemset.begin(); it != itemset->itemset.end(); it++)
			this->itemset.push_back(std::make_shared<Item>(it->get()));
	}

	void addItem(const std::shared_ptr<Item>& item);
	std::string toString() const;
	//static std::string itemsetListToString(const std::vector<Itemset>& v);

	static std::shared_ptr<Itemset> combineItemset(const std::shared_ptr<Itemset>& str1, const std::shared_ptr<Itemset>& str2);

	unsigned int getItemCount() const;
	std::shared_ptr<Item> getItem(unsigned int i) const;
	void replaceItem(unsigned int i, const std::shared_ptr<Item>& item);

	bool containsAClone() const;

	bool operator==(const Itemset& other);
};

inline bool Itemset::containsAClone() const
{
	for (auto elt : this->itemset)
	{
		if (elt->isAClone())
			return true;
	}
	return false;
}

inline void Itemset::addItem(const std::shared_ptr<Item>& item)
{
	this->itemset.push_back(item);
	if(this->itemset.size() == 1)
	{
		this->orValue = item->staticBitset;
		this->orSupport = this->orValue.count();
		this->andValue = item->staticBitset;
		this->andSupport = orSupport;
		this->dirty = false;
		this->isEssential = true;
	}
	else
	{
		this->orValue = this->orValue | item->staticBitset;
		this->orSupport = this->orValue.count();
		this->andValue = this->andValue & item->staticBitset;
		this->andSupport = this->andValue.count();
		if (!this->andSupport)
			this->isEssential = true;
	}
}

inline bool Itemset::operator==(const Itemset& other) 
{
	// first test if support is different, itemsets are differents
	if ((!this->dirty && !other.dirty) && (this->orSupport != other.orSupport))
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