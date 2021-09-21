#pragma once
#include <vector>
#include <algorithm>
#include <cassert>
#include <memory>
#include <bitset>

#include "utils.h"
#include "SparseBitset.h"

// 32768	// dualmatching30 --> OK, 10 sec --> 6 sec
// 65536	// dualmatching32 --> OK, 48 sec --> 28 sec --> 5 sec
// 131072	// dualmatching34 --> OK, 444 sec --> 159 sec --> 114 sec (max 14.5go)
// 262144	// dualmatching36 --> 104 sec
// 524288	// dualmatching38
// 1600		// lose1600 --> 10.5 sec
#define BITSET_SIZE 262144

typedef std::bitset<BITSET_SIZE> StaticBitset;

class Item
{
	// Class Itemset has access to all private members of an item
	friend class Itemset;

private:
	unsigned int attributeIndex;

	std::unique_ptr<StaticBitset> staticBitset;
	
	// true if this item is a clone
	bool isClone;
	
	// contains a list of clone for this item (same bitset)
	std::vector<Item*> clones;

public:
	Item(int index, unsigned int bitsetSize)
	{
		this->staticBitset = std::make_unique<StaticBitset>();
		this->attributeIndex = index;
		this->isClone = false;
	}

	~Item()
	{
		clones.clear();
		staticBitset.reset();
	}

	void set(unsigned int i, bool value);
	bool get(unsigned int) const;
	unsigned int count() const;
	void addClone(Item* clone);
	void setClone();
	unsigned int getAttributeIndex() const;
	bool isAnOriginal() const;
	unsigned int getCloneCount() const;
	Item* getClone(unsigned int i) const;
	void resetClonedAttributesIndexes();
	bool isAClone() const;

	bool operator==(const Item& other);	
};

inline unsigned int Item::getAttributeIndex() const
{
	return this->attributeIndex;
}

inline void Item::set(unsigned int i, bool value)
{
	this->staticBitset->set(i, value);
}

inline bool Item::get(unsigned int i) const
{
	return this->staticBitset->test(i);
}

inline unsigned int Item::count() const
{
	return this->staticBitset->count();
}

inline void Item::addClone(Item* clone)
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

inline Item* Item::getClone(unsigned int i) const
{
	assert(i < this->clones.size());
	return this->clones[i];
}

inline bool Item::isAClone() const
{
	return this->isClone;
}

inline bool Item::operator==(const Item& other)
{
	if(other.attributeIndex == this->attributeIndex)
		return true;
	return (*other.staticBitset) == (*this->staticBitset);
}
