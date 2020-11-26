#pragma once
#include <vector>
#include <string>
#include <algorithm>
#include "utils.h"
#include "Item.h"
#include "SparseBitset.h"

//#define _OLD_ISESSENTIAL

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
	// true if itemset has at least an item who is a clone
	bool hasClone;

#ifndef _OLD_ISESSENTIAL
	//
	bool isEssential;

	// this bitset contains the transaction the minimal transaction 
	//     0 1 2 3 
	//   ----------
	// 0 | 1 1 0 0
	// 1 | 0 1 0 1
	// 2 | 0 0 0 1
	// 3 | 1 0 0 0
	// 4 | 0 0 0 1
	// minimal transactions are <2,3> (transaction number 2, set bit is at index number 3)
	//                          <3,0> (transaction number 3, set bit is at index number 0)
	//                          <4,3> (transaction number 4, set bit is at index number 1)
	StaticBitset isEssentialADNBitset;

	// if ith bit is set, then i index cannot be part of minimal ADN
	// ie there is at least one set bit at ith index into itemset's bitset
	StaticBitset markedNonEssentialBisetIndex;

	StaticBitset temporaryBitset;
#endif	
	
private:

#ifndef _OLD_ISESSENTIAL
	void updateIsEssential(const std::shared_ptr<Item>& item);
#endif

public:
	Itemset();

	void addFirstItem(const std::shared_ptr<Item>& item);
	void addItem(const std::shared_ptr<Item>& item);
		
	void combineItemset(const std::shared_ptr<Itemset>& str2);
	
	unsigned int getItemCount() const;
	unsigned int getDisjunctifSupport() const;
	std::shared_ptr<Item> getItem(unsigned int i) const;

	std::shared_ptr<Itemset> createAndReplaceItem(unsigned int i, const std::shared_ptr<Item>& item);

	bool computeIsEssential();

	bool containsAClone() const;

	bool operator==(const Itemset& other);

	std::string toString() const;

	static unsigned int computeDisjunctifSupport(const Itemset& left, const std::shared_ptr<Itemset>& right);
	static void combineRightIntoLeft(Itemset& left, const std::shared_ptr<Itemset>& right);
	static void copyRightIntoLeft(Itemset& left, const std::shared_ptr<Itemset>& right);

};

inline unsigned int Itemset::getItemCount() const
{
	return static_cast<unsigned int>(this->itemset.size());
}

inline unsigned int Itemset::getDisjunctifSupport() const
{
	// OR operation has already been computed for this itemset into combined itemset function
	assert(!this->dirty);
	return this->orSupport;
}

inline std::shared_ptr<Item> Itemset::getItem(unsigned int i) const
{
	assert(i < this->itemset.size());
	return this->itemset[i];
}
