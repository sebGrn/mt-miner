#pragma once
#include <vector>
#include <string>
#include <algorithm>
#include "utils.h"
#include "Bitset.h"


class Itemset
{
public:
	std::vector<unsigned int> itemset_list;
	unsigned int bitset_count;
	unsigned long or_value;
	bool computed;

public:
	Itemset()
	{
		this->bitset_count = 0;
		this->or_value = 0;
		this->computed = false;
	}

	//static bool compare_itemset(const Itemset& item)
	//{
	//	// use AND operator for comparaison ?
	//	return itemset_list == item.itemset_list;
	//}

	static std::string itemsetToString(const Itemset& v)
	{
		std::string res = "{";
		for_each(v.itemset_list.begin(), v.itemset_list.end(), [&](unsigned int i) {
			res += std::to_string(i);
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
	static std::vector<Itemset> sortVectorOfItemset(const std::vector<Itemset>& strVector)
	{
		std::vector<Itemset> sortedList = strVector;
		std::transform(strVector.begin(), strVector.end(), sortedList.begin(), [&](const Itemset& elt) {
			//Itemset v = splitToVectorOfInt(elt, ' ');	
			Itemset v = elt;
			std::sort(v.itemset_list.begin(), v.itemset_list.end());

			Itemset res;
			for_each(v.itemset_list.begin(), v.itemset_list.end(), [&](unsigned int i) {
				res.itemset_list.push_back(i);
				});
			return res;
			});
		return sortedList;
	}

	static Itemset combineItemset(const Itemset& str1, const Itemset& str2)
	{
		// "1" + "2" => "12"
		// "71" + "72" => "712"
		Itemset left = str1;
		Itemset right = str2;
		std::vector<Itemset> combinedListElt;
		for_each(str1.itemset_list.begin(), str1.itemset_list.end(), [&](unsigned int i) {
			auto it = std::find_if(right.itemset_list.begin(), right.itemset_list.end(), Utils::compare_int(i));
			if (it != right.itemset_list.end())
			{
				// remove elt
				right.itemset_list.erase(it);
			}
			});
		// merge 2 lists into intList1
		left.itemset_list.insert(left.itemset_list.end(), right.itemset_list.begin(), right.itemset_list.end());
		// transform int list into string list seperated by ' '
		Itemset combinedElt;
		for_each(left.itemset_list.begin(), left.itemset_list.end(), [&combinedElt](unsigned int i) { combinedElt.itemset_list.push_back(i); });
		if (str1.computed && str2.computed)
		{
			if (!str1.or_value)
				combinedElt.or_value = str2.or_value;
			else if (!str2.or_value)
				combinedElt.or_value = str1.or_value;
			else
				combinedElt.or_value = str1.or_value | str2.or_value;
			combinedElt.bitset_count = COUNT_BIT(combinedElt.or_value);
			combinedElt.computed = true;
		}
		return combinedElt;
	};

	static bool containsZero(const Itemset& data)
	{
		return (std::find(data.itemset_list.begin(), data.itemset_list.end(), 0) != data.itemset_list.end());
	}
};


struct compare_itemset
{
	Itemset key;
	compare_itemset(Itemset const& i) : key(i) { }

	bool operator()(Itemset const& item)
	{
		// use AND operator for comparaison
		return key.itemset_list == item.itemset_list;
	}
};