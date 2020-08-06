#pragma once
#include <vector>
#include <algorithm>
#include <cassert>

#include "utils.h"
#include "Bitset.h"
#include "SparseBitset.h"

typedef std::bitset<1000> StaticBitset;

class Item
{
public:
	unsigned int attributeIndex;
	std::shared_ptr<StaticBitset> staticBitset;
	std::shared_ptr<SparseBitset> sparseBitset;

	// true if this item is an original
	bool isOriginal;
	// true if this item is a clone
	bool isClone;
	// if this bitset is an original, store its clone index from binaryRepresentation map
	std::vector<unsigned int> clonedAttributeIndexes;

public:
	Item(int index, unsigned int bitsetSize)
	{
		attributeIndex = index;
		staticBitset = std::make_shared<StaticBitset>(bitsetSize);
		sparseBitset = std::make_shared<SparseBitset>(bitsetSize);
		isOriginal = isClone = false;
	}

	~Item()
	{}

	void setAsAnOriginal(unsigned int cloneIndex);
	void setAsAClone();
	bool isAnOriginal() const;
	unsigned int getCloneAttributeIndexesCount() const;
	unsigned int getCloneAttributeIndex(unsigned int i) const;
	bool isAClone();

	bool operator==(const Item& other);
};

inline void Item::setAsAnOriginal(unsigned int cloneIndex)
{
	this->isOriginal = true;
	this->clonedAttributeIndexes.push_back(cloneIndex);
}

inline void Item::setAsAClone()
{
	this->isClone = true;
}

inline bool Item::isAnOriginal() const
{
	return this->isOriginal;
}

inline unsigned int Item::getCloneAttributeIndexesCount() const
{
	return static_cast<unsigned int>(this->clonedAttributeIndexes.size());
}

inline unsigned int Item::getCloneAttributeIndex(unsigned int i) const
{
	assert(i < this->clonedAttributeIndexes.size());
	return this->clonedAttributeIndexes[i];
}

inline bool Item::isAClone()
{
	return this->isClone;
}

inline bool Item::operator==(const Item& other)
{
	return (other.attributeIndex == this->attributeIndex);
}
