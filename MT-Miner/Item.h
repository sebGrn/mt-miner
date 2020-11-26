#pragma once
#include <vector>
#include <algorithm>
#include <cassert>
#include <memory>
#include <bitset>

#include "utils.h"
#include "SparseBitset.h"

//#define BITSET_SIZE 32768	// dualmatching30 --> OK, 10 sec
//#define BITSET_SIZE 65536	// dualmatching32 --> OK, 48 sec
//#define BITSET_SIZE 131072	// dualmatching34 --> OK, 444 sec
//#define BITSET_SIZE 262144	// dualmatching36 --> PAS OK, 5 min, 47Go memory
//#define BITSET_SIZE 524288	// dualmatching38
#define BITSET_SIZE 800
typedef std::bitset<BITSET_SIZE> StaticBitset;

class Item
{
public:
	unsigned int attributeIndex;
	StaticBitset staticBitset;
	
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

	~Item()
	{}

	void addClone(const std::shared_ptr<Item>& clone);
	void setClone();
	bool isAnOriginal() const;
	unsigned int getCloneCount() const;
	std::shared_ptr<Item> getClone(unsigned int i) const;
	void resetClonedAttributesIndexes();
	bool isAClone();

	bool operator==(const Item& other);
};

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
	return other.staticBitset == this->staticBitset;
}
