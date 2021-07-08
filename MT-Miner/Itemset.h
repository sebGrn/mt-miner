#pragma once
#include <vector>
#include <string>
#include <algorithm>
#include "utils.h"
#include "Item.h"
#include "SparseBitset.h"


#define ISESSENTIAL_ON_TOEXPLORE

class Itemset
{
public:
	enum ItemsetType {
		CONSJONCTIVE,	// AND
		DISJUNCTIVE		// OR
	};
	static ItemsetType itemsetType;

private:
	/// list of item
	std::vector<Item*> itemset;
	/// true if bitset_count & or value has to be computed
	bool dirty; 
	/// true if itemset has at least an item which is a clone
	bool hasClone;

	// disjunctive support : computed with OR
	// consjonctive support : computed with AND
	std::unique_ptr<StaticBitset> supportBitset;
	unsigned int supportValue;

#ifndef ISESSENTIAL_ON_TOEXPLORE
	std::unique_ptr<StaticBitset> xorSupportBitset;
	std::unique_ptr<StaticBitset> noiseBitset;
#else
	bool isEssential;
	std::unique_ptr<StaticBitset> cumulatedXorbitset;
	std::unique_ptr<StaticBitset> noiseBitset;
#endif

public:	
	Itemset();
	Itemset(Item* item);
	Itemset(const std::shared_ptr<Itemset>& itemset);
	~Itemset();
	
	void flip();
	
	unsigned int getItemCount() const;
	Item* getItem(unsigned int i) const;
	bool containsAClone() const;
	std::string toString() const;
	
	std::shared_ptr<Itemset> createAndReplaceItem(unsigned int i, Item* item);
	void combineItemset(const Itemset* itemset);
	
	unsigned int getSupport() const;
	
	bool operator==(const Itemset& other);

	void writeToBinaryFile(std::ofstream& output);
	void readFromBinaryFile(std::ifstream& output);

	//static bool computeIsEssential_old(const std::shared_ptr<Itemset>& left, const std::shared_ptr<Itemset>& right);
#ifndef ISESSENTIAL_ON_TOEXPLORE
	static bool computeIsEssential(const std::shared_ptr<Itemset>& left, const std::shared_ptr<Itemset>& right);
#else
	static bool computeIsEssential(const std::shared_ptr<Itemset>& itemset, bool mtComputation = false);
#endif
	static unsigned int computeSupport(const Itemset& left, const std::shared_ptr<Itemset>& right);
	static void copyRightIntoLeft(Itemset& left, const std::shared_ptr<Itemset>& right);
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
	return static_cast<unsigned int>(this->itemset.size());
}

inline Item* Itemset::getItem(unsigned int i) const
{
	assert(i < this->itemset.size());
	return this->itemset[i];
}

inline void Itemset::flip()
{
	if (itemsetType == CONSJONCTIVE)
	{
		for (auto& elt : itemset)
		{
			(*elt).staticBitset->flip();
		}
		this->supportBitset->flip();
		this->supportValue = (*this->supportBitset).count();
		this->dirty = true;
	}
}