#pragma once

#include <cassert>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <algorithm>

class Utils
{
public:
	using Itemset = std::vector<unsigned int>;

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

	struct compare_itemset
	{
		Itemset key;
		compare_itemset(Itemset const& i) : key(i) { }

		bool operator()(Itemset const& item)
		{
			// use AND operator for comparaison
#ifdef _DEBUG
			for (unsigned int i = 0; i < item.size(); i++)
			{
				assert(i < key.size());
				if (item[i] != key[i])
					return false;				
			}
			return true;
#else
			return key == item;
#endif

		}
	};

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

	static std::string itemsetToString(const Itemset& v)
	{
		std::string res = "{";
		for_each(v.begin(), v.end(), [&](unsigned int i) {
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

	// sort each element of minimalTransversals
	static std::vector<Itemset> sortVectorOfItemset(const std::vector<Itemset>& strVector)
	{
		std::vector<Itemset> sortedList = strVector;
		transform(strVector.begin(), strVector.end(), sortedList.begin(), [&](const Itemset& elt) {
			//Itemset v = splitToVectorOfInt(elt, ' ');		
			Itemset v = elt;
			std::sort(v.begin(), v.end());

			Itemset res;
			for_each(v.begin(), v.end(), [&](unsigned int i) {
				res.push_back(i);
				});
			return res;
			});
		return sortedList;
	}

	static Utils::Itemset combineItemset(const Utils::Itemset& str1, const Utils::Itemset& str2)
	{
		// "1" + "2" => "12"
		// "71" + "72" => "712"
		Utils::Itemset left = str1;
		Utils::Itemset right = str2;
		std::vector<Utils::Itemset> combinedListElt;
		for_each(str1.begin(), str1.end(), [&](unsigned int i) {
			auto it = std::find_if(right.begin(), right.end(), Utils::compare_int(i));
			if (it != right.end())
			{
				// remove elt
				right.erase(it);
			}
			});
		// merge 2 lists into intList1
		left.insert(left.end(), right.begin(), right.end());
		// transform int list into string list seperated by ' '
		Utils::Itemset combinedElt;
		for_each(left.begin(), left.end(), [&](unsigned int i) {
			combinedElt.push_back(i);
			});
		return combinedElt;
	};
	
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