#pragma once
#include <vector>
#include <string>
#include <algorithm>
#include "utils.h"
#include "Bitset.h"
#include "SparseBitset.h"


/**
 *
 *
 */
class Itemset
{
public:
	struct Item
	{
		unsigned int item_index;
		StaticBitset item_bitset;
		SparseBitset item_sparse_bitset;
	};
	std::vector<Item> itemset;

	// true if bitset_count & or value have been computed
	bool computed; 
	// support of the current itemset (nb 1's bit)
	unsigned int bitset_count;
	// stored OR value of all bitset from the item set
	StaticBitset or_value;

	// 
	bool is_essential_computed;
	bool is_essential;


public:
	Itemset()
	{
		this->bitset_count = 0;
		this->computed = false;
		this->is_essential_computed = false;
		this->is_essential = false;
	}

	//bool operator!=(const Itemset& other)
	//{
	//	if (other.itemset.size() != this->itemset.size())
	//		return true;
	//	auto it2 = other.itemset.begin();
	//	for (auto it1 = this->itemset.begin(); it1 != this->itemset.end(); it1++, it2++)
	//		if(it1->item_index != it2->item_index)
	//			return true;
	//	return false;
	//}

	// ------------------------------------------------------------------------------------------------------------------------- //

	static std::string itemsetToString(const Itemset& v)
	{
		std::string res = "{";
		for_each(v.itemset.begin(), v.itemset.end(), [&](const Item& item) {
			res += std::to_string(item.item_index);
			res += ",";
			});
		res.pop_back();
		res += "}";
		return res;
	}

	static std::string itemsetListToString(const std::vector<Itemset>& v)
	{
		std::string res = "{";
		for_each(v.begin(), v.end(), [&](const Itemset& elt) {
			res += itemsetToString(elt);
			res += ",";
			});
		res.pop_back();
		res += "}";
		return res;
	}

	// ------------------------------------------------------------------------------------------------------------------------- //

	// sort each element of minimalTransversals
	//static std::vector<Itemset> sortVectorOfItemset(const std::vector<Itemset>& strVector)
	//{
	//	std::vector<Itemset> sortedList = strVector;
	//	std::transform(strVector.begin(), strVector.end(), sortedList.begin(), [&](const Itemset& elt) {
	//		//Itemset v = splitToVectorOfInt(elt, ' ');	
	//		Itemset v = elt;
	//		std::sort(v.item_list.begin(), v.item_list.end());

	//		Itemset res;
	//		for_each(v.item_list.begin(), v.item_list.end(), [&](unsigned int i) {
	//			res.item_list.push_back(i);
	//			});
	//		return res;
	//		});
	//	return sortedList;
	//}

	// ------------------------------------------------------------------------------------------------------------------------- //

	static Itemset combineItemset(const Itemset& str1, const Itemset& str2)
	{
		// "1" + "2" => "12"
		// "71" + "72" => "712"
		Itemset left = str1;
		Itemset right = str2;
		
		// remove duplicate indexes from left into right itemset
		std::vector<Itemset> combinedListElt;
		for_each(str1.itemset.begin(), str1.itemset.end(), [&](const Item& item) {
			auto it = right.itemset.begin();
			while (it != right.itemset.end())
			{
				if (it->item_index == item.item_index)
					break;
				it++;
			}
			if(it != right.itemset.end())
			{
				// remove elt
				right.itemset.erase(it);							
			}
		});

		// merge right itemset at the end of left itemset
		//left.itemset_list.insert(left.itemset_list.end(), right.itemset_list.begin(), right.itemset_list.end());
		std::copy(right.itemset.begin(), right.itemset.end(), std::back_inserter(left.itemset));
		
		// create a new combined itemset
		//Itemset combinedElt;
		//std::copy(left.itemset_list.begin(), left.itemset_list.end(), std::back_inserter(combinedElt.itemset_list));

		// compute OR value from left and right itemsets
		if (str1.computed && str2.computed)
		{
			//if (str1.or_value)
			//	left.or_value = str2.or_value;
			//else if (!str2.or_value)
			//	left.or_value = str1.or_value;
			//else
			left.or_value = str1.or_value | str2.or_value;
			left.bitset_count = left.or_value.count();
			left.computed = true;
		}
		return left;
	};

	// check if itemset contains a 0 index
	//static bool containsZero(const Itemset& data)
	//{
	//	for(auto it = data.itemset.begin(); it != data.itemset.end(); it++)
	//	{
	//		if (it->item_index == 0)
	//			return true;
	//	}
	//	return false;
	//}
};

struct compare_itemset
{
	Itemset key;
	compare_itemset(Itemset const& i) : key(i) { }

	bool operator()(Itemset const& item)
	{
		//return key.itemset == item.itemset;
		//std::function<bool(const Itemset&, const Itemset&)> comparator = [](const Itemset& item1, const Itemset& item2) {
		//	return std::equal
		//});

		if (key.itemset.size() != item.itemset.size())
			return false;

		auto it2 = item.itemset.begin();
		for (auto it1 = key.itemset.begin(); it1 != key.itemset.end(); it1++, it2++)
		{
			if (it1->item_index != it2->item_index)
				return false;
		}
		return true;
	}
};