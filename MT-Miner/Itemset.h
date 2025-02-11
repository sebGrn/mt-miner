#pragma once
#include <vector>
#include <string>
#include <algorithm>
#include "utils.h"
#include "Item.h"
#include "SparseBitset.h"

#include <ctime>

class BinaryRepresentation;

class Itemset
{
private:
	/// list of item index (faster to copy from other itemset)
	std::vector<unsigned int> itemsetIndexVector;

	/// true if itemset has at least an item which is a clone
	bool hasClone;

	// disjunctive support : computed with OR
	StaticBitset supportBitset;
	unsigned int supportValue;

	std::unique_ptr<StaticBitset> cumulatedXorbitset;
	std::unique_ptr<StaticBitset> noiseBitset;

	bool isEssential;

private:
	static bool computeIsEssentialParameters(const std::shared_ptr<Itemset>& itemset, StaticBitset& cumulatedXorbitset, StaticBitset& noiseBitset);

public:	
	Itemset();
	Itemset(unsigned int binaryRepIndex);
	Itemset(const std::shared_ptr<Itemset>& itemset);
	~Itemset();
		
	unsigned int getItemCount() const;
	bool containsAClone() const;
	unsigned int getLastItemAttributeIndex() const;

	std::string toString() const;
	
	std::shared_ptr<Itemset> createAndReplaceItem(unsigned int i, Item* item);
	
	void combine(unsigned int rightAttributeIndex);
	
	unsigned int getSupport() const;
	
	bool operator==(const Itemset& other);

	static bool computeIsEssential(const std::shared_ptr<Itemset>& itemset, bool storeEssentiality = true);
	static unsigned int computeSupport(const Itemset& left, const std::shared_ptr<Itemset>& right);
	static Item* getItem(const std::shared_ptr<Itemset>& itemset, unsigned int i);
	static bool isEssentialRapid(std::shared_ptr<Itemset>& left, unsigned int itemIndexToAdd);
};

inline unsigned int Itemset::getSupport() const
{
	return supportValue;
}

inline bool Itemset::containsAClone() const
{
	return this->hasClone;
}

inline unsigned int Itemset::getItemCount() const
{
	return static_cast<unsigned int>(this->itemsetIndexVector.size());
}

inline unsigned int Itemset::getLastItemAttributeIndex() const
{
	return this->itemsetIndexVector[this->itemsetIndexVector.size() - 1];
}

inline unsigned int Itemset::computeSupport(const Itemset& left, const std::shared_ptr<Itemset>& right)
{
	return(left.supportBitset | right->supportBitset).count();
}