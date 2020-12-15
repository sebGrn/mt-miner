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
#define BITSET_SIZE 1000
typedef std::bitset<BITSET_SIZE> StaticBitset;

class Item
{
private:
//public:
	unsigned int attributeIndex;
	std::shared_ptr<StaticBitset> staticBitset;
	
	// true if this item is a clone
	bool isClone;
	
	// contains a list of clone for this item (same bitset)
	std::vector<std::shared_ptr<Item>> clones;

public:
	Item(int index, unsigned int bitsetSize)
	{
		this->staticBitset = std::make_shared<StaticBitset>();
		this->attributeIndex = index;
		this->isClone = false;
	}

	~Item()
	{
		clones.clear();
	}

	void set(unsigned int i, bool value);
	bool get(unsigned int) const;
	unsigned int count() const;
	std::shared_ptr<StaticBitset> value() const;
	void addClone(const std::shared_ptr<Item>& clone);
	void setClone();
	unsigned int getAttributeIndex() const;
	bool isAnOriginal() const;
	unsigned int getCloneCount() const;
	std::shared_ptr<Item> getClone(unsigned int i) const;
	void resetClonedAttributesIndexes();
	bool isAClone() const;

	bool operator==(const Item& other);	
};

inline unsigned int Item::getAttributeIndex() const
{
	return this->attributeIndex;
}

inline std::shared_ptr<StaticBitset> Item::value() const
{
	return this->staticBitset;
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
