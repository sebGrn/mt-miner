#pragma once
#include <vector>
#include <algorithm>
#include <cassert>
#include <memory>
#include <bitset>

#include "utils.h"
#include "SparseBitset.h"

#define BITSET_SIZE 800
typedef std::bitset<BITSET_SIZE> StaticBitset;

class Item
{
public:
	unsigned int attributeIndex;
	StaticBitset staticBitset;
	SparseBitset sparseBitset;

	// true if this item is a clone
	bool isClone;
	
	// contains a list of clone for this item (same bitset)
	std::vector<std::shared_ptr<Item>> clones;

public:
	Item(int index, unsigned int bitsetSize)
	{
		this->attributeIndex = index;
		this->isClone = false;
	}

	Item(const Item* item)
	{
		this->attributeIndex = item->attributeIndex;
		this->staticBitset = item->staticBitset;
		this->sparseBitset = item->sparseBitset;
		this->isClone = item->isClone;
		this->clones = item->clones;
	}

	~Item()
	{}

	static void buildSparseMatrix(SparseBitset& dest, const StaticBitset& src);

	void addClone(const std::shared_ptr<Item>& clone);
	void setClone();
	bool isAnOriginal() const;
	unsigned int getCloneCount() const;
	std::shared_ptr<Item> getClone(unsigned int i) const;
	void resetClonedAttributesIndexes();
	bool isAClone();

	bool operator==(const Item& other);
};

inline void Item::buildSparseMatrix(SparseBitset& dest, const StaticBitset& bitset)
{
	dest.bitset_value.clear();
	for (unsigned int i = 0; i < bitset.size(); i++)
	{
		bool bit = bitset[i];
		if (bit)
			dest.bitset_value.emplace_back(i);
	}
	dest.bitset_value.sort();
}

inline void Item::addClone(const std::shared_ptr<Item>& clone)
{
	this->clones.push_back(clone);
}

inline void Item::setClone()
{
	this->isClone = true;
}

inline bool Item::isAnOriginal() const
{
	return !this->clones.empty();
}

inline void Item::resetClonedAttributesIndexes()
{
	this->clones.clear();
}

inline unsigned int Item::getCloneCount() const
{
	return static_cast<unsigned int>(this->clones.size());
}

inline std::shared_ptr<Item> Item::getClone(unsigned int i) const
{
	assert(i < this->clones.size());
	return this->clones[i];
}

inline bool Item::isAClone()
{
	return this->isClone;
}

inline bool Item::operator==(const Item& other)
{
	if(other.attributeIndex == this->attributeIndex)
		return true;
	bool res = other.staticBitset == this->staticBitset;
	return res;
}
