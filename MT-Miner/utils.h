#pragma once

#include <cassert>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <algorithm>
#include "Profiler.h"

// ------------------------------------------------------------------------------------------------------------------------- //

//struct Itemset
//{
//	std::vector<unsigned int> itemset_list;
//	unsigned int bitset_count= 0;
//	unsigned long or_value = 0;
//	bool computed = false;
//};
//
//typedef std::vector<Itemset> ItemsetList;

// ------------------------------------------------------------------------------------------------------------------------- //

class Utils
{
public:
	struct compare_int
	{
		int key;
		compare_int(int const& i) : key(i) { }

		bool operator()(int const& i)
		{
			return (i == key);
		}
	};

	//struct compare_itemset
	//{
	//	Itemset key;
	//	compare_itemset(Itemset const& i) : key(i) { }

	//	bool operator()(Itemset const& item)
	//	{
	//		// use AND operator for comparaison
	//		return key.itemset_list == item.itemset_list;
	//	}
	//};

	struct compare_second_value_of_pair
	{
		unsigned int key;
		compare_second_value_of_pair(unsigned int const& i) : key(i) { }

		// check that original (it1->first) is not already registered as a clone (it2->first) in clonedBitsetIndexes
		bool operator()(std::pair<unsigned int, unsigned int> const& item)
		{
			return item.second == key;
		}
	};

	// ------------------------------------------------------------------------------------------------------------------------- //

	//static std::string itemsetToString(const Itemset& v)
	//{
	//	std::string res = "{";
	//	for_each(v.itemset_list.begin(), v.itemset_list.end(), [&](unsigned int i) {
	//		res += std::to_string(i);
	//		res += ",";
	//		});
	//	res.pop_back();
	//	res += "}";
	//	return res;
	//}

	//static std::string itemsetListToString(const ItemsetList& v)
	//{
	//	std::string res = "{";
	//	for_each(v.begin(), v.end(), [&](const Itemset& elt) {
	//		res += itemsetToString(elt);
	//		res += ",";
	//		});
	//	res.pop_back();
	//	res += "}";
	//	return res;
	//}

	//// ------------------------------------------------------------------------------------------------------------------------- //

	//// sort each element of minimalTransversals
	//static ItemsetList sortVectorOfItemset(const ItemsetList& strVector)
	//{
	//	ItemsetList sortedList = strVector;
	//	transform(strVector.begin(), strVector.end(), sortedList.begin(), [&](const Itemset& elt) {
	//		//Itemset v = splitToVectorOfInt(elt, ' ');	
	//		Itemset v = elt;
	//		std::sort(v.itemset_list.begin(), v.itemset_list.end());

	//		Itemset res;
	//		for_each(v.itemset_list.begin(), v.itemset_list.end(), [&](unsigned int i) {
	//			res.itemset_list.push_back(i);
	//			});
	//		return res;
	//		});
	//	return sortedList;
	//}

	//static Itemset combineItemset(const Itemset& str1, const Itemset& str2)
	//{
	//	// "1" + "2" => "12"
	//	// "71" + "72" => "712"
	//	Itemset left = str1;
	//	Itemset right = str2;
	//	ItemsetList combinedListElt;
	//	for_each(str1.itemset_list.begin(), str1.itemset_list.end(), [&](unsigned int i) {
	//		auto it = std::find_if(right.itemset_list.begin(), right.itemset_list.end(), Utils::compare_int(i));
	//		if (it != right.itemset_list.end())
	//		{
	//			// remove elt
	//			right.itemset_list.erase(it);
	//		}
	//		});
	//	// merge 2 lists into intList1
	//	left.itemset_list.insert(left.itemset_list.end(), right.itemset_list.begin(), right.itemset_list.end());
	//	// transform int list into string list seperated by ' '
	//	Itemset combinedElt;
	//	for_each(left.itemset_list.begin(), left.itemset_list.end(), [&combinedElt](unsigned int i) { combinedElt.itemset_list.push_back(i); });
	//	if (str1.computed && str2.computed)
	//	{
	//		if(!str1.or_value)
	//			combinedElt.or_value = str2.or_value;
	//		else if (!str2.or_value)
	//			combinedElt.or_value = str1.or_value;
	//		else
	//			combinedElt.or_value = str1.or_value | str2.or_value;
	//		combinedElt.bitset_count = COUNT_BIT(combinedElt.or_value);
	//		combinedElt.computed = true;
	//	}
	//	return combinedElt;
	//};
	//
	//static bool containsZero(const Itemset& data)
	//{
	//	return (std::find(data.itemset_list.begin(), data.itemset_list.end(), 0) != data.itemset_list.end());
	//}

	// ------------------------------------------------------------------------------------------------------------------------- //

	/// <summary>
	/// extract a list of int from a string
	/// </summary>
	/// <param name="s"></param>
	/// <param name="delimiter"></param>
	/// <returns></returns>
	template <typename Out>
	static void split(const std::string& s, char delim, Out result)
	{
		std::istringstream iss(s);
		std::string item;
		while (std::getline(iss, item, delim)) {
			*result++ = std::stoi(item);
		}
	}

	/// <summary>
	/// 
	/// </summary>
	/// <param name="s"></param>
	/// <param name="delimiter"></param>
	/// <returns></returns>
	static std::vector<unsigned int> splitToVectorOfInt(const std::string& s, char delim)
	{
		std::vector<unsigned int> elems;
		split(s, delim, std::back_inserter(elems));
		return elems;
	}

	// ------------------------------------------------------------------------------------------------------------------------- //

	static std::string& ltrim(std::string& str, const std::string& chars = "\t\n\v\f\r ")
	{
		str.erase(0, str.find_first_not_of(chars));
		return str;
	}

	static std::string& rtrim(std::string& str, const std::string& chars = "\t\n\v\f\r ")
	{
		str.erase(str.find_last_not_of(chars) + 1);
		return str;
	}

	static std::string& trim(std::string& str, const std::string& chars = "\t\n\v\f\r ")
	{
		return ltrim(rtrim(str, chars), chars);
	}
};