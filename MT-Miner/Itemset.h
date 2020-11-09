#pragma once
#include <vector>
#include <string>
#include <algorithm>
#include "utils.h"
#include "Item.h"
#include "SparseBitset.h"

#define _OLD_ISESSENTIAL

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
	bool isEssential;
	StaticBitset isEssentialADNBitset;
	SparseBitset isEssentialADNBitset2;

	// utiliser un bitset et faire des OR pour tester
	// 1 dans le bitset --> bit non essentiel
	std::list<unsigned int> markedNonEssetialBisetIndex;
#endif	

	// this map contains the transaction index (line number) / item index (column index) of the minimal transaction list
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
	std::vector<std::pair<unsigned int, unsigned int>> minimalTransaction;
	
private:

#ifndef _OLD_ISESSENTIAL
	void updateIsEssentialSparseMatrix(const std::shared_ptr<Item>& item);
#endif

public:
	Itemset();

	void addFirstItem(const std::shared_ptr<Item>& item);
	void addItem(const std::shared_ptr<Item>& item);
		
	static std::shared_ptr<Itemset> combineItemset(const std::shared_ptr<Itemset>& str1, const std::shared_ptr<Itemset>& str2);

	unsigned int getItemCount() const;
	unsigned int getDisjunctifSupport() const;

	void recurseOnClonedItemset(unsigned int iItem, std::vector<std::shared_ptr<Itemset>>& graph_mt);

	std::shared_ptr<Itemset> createAndReplaceItem(unsigned int i, const std::shared_ptr<Item>& item);

	bool computeIsEssential();

	bool containsAClone() const;

	bool operator==(const Itemset& other);

	std::string toString() const;
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