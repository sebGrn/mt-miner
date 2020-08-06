#include "Item.h"


Item::Item(int index, unsigned int bitsetSize)
{
	attributeIndex = index;
	staticBitset = std::make_shared<StaticBitset>(bitsetSize);
	sparseBitset = std::make_shared<SparseBitset>(bitsetSize);
	isOriginal = isClone = false;
}

Item::~Item()
{}

void Item::setAsAnOriginal(unsigned int cloneIndex)
{
	this->isOriginal = true;
	this->clonedAttributeIndexes.push_back(cloneIndex);
}

void Item::setAsAClone()
{
	this->isClone = true;
}

bool Item::isAnOriginal() const
{
	return this->isOriginal;
}

unsigned int Item::getCloneAttributeIndexesCount() const
{
	return static_cast<unsigned int>(this->clonedAttributeIndexes.size());
}

unsigned int Item::getCloneAttributeIndex(unsigned int i) const
{
	assert(i < this->cloneIndexes.size());
	return this->clonedAttributeIndexes[i];
}

bool Item::isAClone()
{
	return this->isClone;
}