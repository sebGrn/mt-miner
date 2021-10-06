#pragma once
#include <vector>
#include <string>
#include <algorithm>
#include "utils.h"
#include "Item.h"
#include "SparseBitset.h"

class BinaryRepresentation;

#define ISESSENTIAL_ON_TOEXPLORE

class Itemset
{
public:
	enum ItemsetType {
		CONSJONCTIVE,	// AND
		DISJUNCTIVE		// OR
	};
	static ItemsetType itemsetType;

protected:
	/// list of item index (faster to copy from other itemset)
	std::vector<unsigned int> itemsetIndexVector;

	/// true if bitset_count & or value has to be computed
	/// true if itemset has at least an item which is a clone
	bool hasClone;

	// disjunctive support : computed with OR
	// consjonctive support : computed with AND
	StaticBitset supportBitset;
	unsigned int supportValue;

public:	
	virtual ~Itemset();
	
	unsigned int getItemCount() const;
	
	bool containsAClone() const;	
	unsigned int getSupport() const;
	StaticBitset getSupportBitset() const;
	unsigned int getItemsetSize() const;
	unsigned int getItemsetFromIndex(unsigned int index) const;
	void setSupportBitset(const StaticBitset& supportBitset, unsigned int supportValue);
	void setCloneValue(bool value);

	std::string toString() const;

	void copyItemset(std::vector<unsigned int>& dest) const;

	// only for consjonctive
	void flip();

	//
	bool operator==(const Itemset& other);

	// pure virtual interfaces	
	virtual std::shared_ptr<Itemset> createAndReplaceItem(unsigned int i, Item* item) = 0;
	virtual void combineItemset(const std::shared_ptr<Itemset>& itemset_right) = 0;
	
	// statics
	static Item* getItem(const std::shared_ptr<Itemset>& itemset, unsigned int i);
	static unsigned int computeSupport(const Itemset& left, const std::shared_ptr<Itemset>& right);		
};

inline unsigned int Itemset::getItemsetFromIndex(unsigned int i) const
{
	assert(i < itemset->itemsetIndexVector.size());
	return this->itemsetIndexVector[i];
}

inline void Itemset::setSupportBitset(const StaticBitset& supportBitset, unsigned int supportValue)
{
	this->supportBitset = supportBitset;
	this->supportValue = supportValue;
}

inline void Itemset::copyItemset(std::vector<unsigned int>& dest) const
{
	dest.clear();
	dest.reserve(this->itemsetIndexVector.size());
	dest = this->itemsetIndexVector;
}

inline unsigned int Itemset::getItemsetSize() const
{
	return static_cast<unsigned int>(itemsetIndexVector.size());
}

inline StaticBitset Itemset::getSupportBitset() const
{
	return supportBitset;
}

inline unsigned int Itemset::getSupport() const
{
	return supportValue;
}

inline void Itemset::setCloneValue(bool value)
{
	this->hasClone = value;
}

inline bool Itemset::containsAClone() const
{
	return this->hasClone;
}
