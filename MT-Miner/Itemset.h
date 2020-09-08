#pragma once
#include <vector>
#include <string>
#include <algorithm>
#include "utils.h"
#include "Item.h"
#include "SparseBitset.h"

class Itemset
{
private:
	std::vector<std::shared_ptr<Item>> itemset;
	
	// used to compute isEssential
	SparseBitset cumulatedMatrix;

public:
	// true if bitset_count & or value has to be computed
	bool dirty; 
	// stored OR value of all bitset from the item set
	StaticBitset orValue;
	// support of the current itemset (nb 1's bit) of OR computation
	unsigned int orSupport;

	//
	bool isEssential;
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
	
public:
	Itemset();
	Itemset(const Itemset* itemset);

	void addItem(const std::shared_ptr<Item>& item);
	std::string toString() const;
	//static std::string itemsetListToString(const std::vector<Itemset>& v);

	static std::shared_ptr<Itemset> combineItemset(const std::shared_ptr<Itemset>& str1, const std::shared_ptr<Itemset>& str2);

	unsigned int getItemCount() const;
	std::shared_ptr<Item> getItem(unsigned int i) const;
	void replaceItem(unsigned int i, const std::shared_ptr<Item>& item);
	void UpdateIsEssential(const std::shared_ptr<Item>& item);

	bool containsAClone() const;

	//bool compareItemIndex(const std::pair<unsigned int, unsigned int>& a, const std::pair<unsigned int, unsigned int>& b);

	bool operator==(const Itemset& other);
};

inline unsigned int Itemset::getItemCount() const
{
	return static_cast<unsigned int>(this->itemset.size());
}

inline std::shared_ptr<Item> Itemset::getItem(unsigned int i) const
{
	assert(i < this->itemset.size());
	return this->itemset[i];
}

//inline bool Itemset::compareItemIndex(const std::pair<unsigned int, unsigned int>& a, const std::pair<unsigned int, unsigned int>& b)
//{
//	return a.second == b.second;
//}