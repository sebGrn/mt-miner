#pragma once

#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <algorithm>

/// <summary>
/// extract a list of int from a string
/// </summary>
/// <param name="s"></param>
/// <param name="delimiter"></param>
/// <returns></returns>
template <typename Out>
inline void split(const std::string& s, char delim, Out result)
{
	std::istringstream iss(s);
	std::string item;
	while (std::getline(iss, item, delim)) {
		*result++ = std::stoi(item);
	}
}

/// <summary>
/// extract a list of int from a string
/// </summary>
/// <param name="s"></param>
/// <param name="delimiter"></param>
/// <returns></returns>
inline std::vector<unsigned int> splitToVectorOfInt(const std::string& s, char delim)
{
	std::vector<unsigned int> elems;
	split(s, delim, std::back_inserter(elems));
	return elems;
}

/// <summary>
/// extract a list of int from a string
/// </summary>
/// <param name="s"></param>
/// <param name="delimiter"></param>
/// <returns></returns>
inline std::vector<int> splitPattern(const std::string& s, const std::string& delimiter)
{
	std::string tmpstr = s;
	size_t pos = 0;
	std::string token;
	std::vector<int> v;
	while ((pos = tmpstr.find(delimiter)) != std::string::npos)
	{
		token = tmpstr.substr(0, pos);
		v.push_back(atol(token.c_str()));
		tmpstr.erase(0, pos + delimiter.length());
	}
	v.push_back(atol(tmpstr.c_str()));
	return v;
}


inline std::string& ltrim(std::string& str, const std::string& chars = "\t\n\v\f\r ")
{
	str.erase(0, str.find_first_not_of(chars));
	return str;
}

inline std::string& rtrim(std::string& str, const std::string& chars = "\t\n\v\f\r ")
{
	str.erase(str.find_last_not_of(chars) + 1);
	return str;
}

inline std::string& trim(std::string& str, const std::string& chars = "\t\n\v\f\r ")
{
	return ltrim(rtrim(str, chars), chars);
}

inline void printStringVectorList(const std::vector<std::string>& v)
{
	for_each(v.begin(), v.end(), [&](const std::string& elt) {
		std::cout << "{" << elt << "}, ";
		});
}

struct compare_int
{
	int key;
	compare_int(int const& i) : key(i) { }

	bool operator()(int const& i)
	{
		return (i == key);
	}
};

struct compare_str
{
	std::string key;
	compare_str(std::string const& i) : key(i) { }

	bool operator()(std::string const& i)
	{
		return (i == key);
	}
};

inline std::string combineIntoString(const std::string& str1, const std::string& str2)
{
	std::vector<unsigned int> intList1 = splitToVectorOfInt(str1, ' ');
	std::vector<unsigned int> intList2 = splitToVectorOfInt(str2, ' ');
	// "1" + "2" => "12"
	// "71" + "72" => "712"
	std::vector<std::string> combinedListElt;
	for_each(intList1.begin(), intList1.end(), [&](unsigned int i) {
		auto it = std::find_if(intList2.begin(), intList2.end(), compare_int(i));
		if (it != intList2.end())
		{
			// remove elt
			intList2.erase(it);
		}
		});
	// merge 2 lists into intList1
	intList1.insert(intList1.end(), intList2.begin(), intList2.end());
	// transform int list into string list seperated by ' '
	std::string combinedElt;
	for_each(intList1.begin(), intList1.end(), [&](unsigned int i) {
		combinedElt += std::to_string(i) + ' ';
		});
	// remove last character
	combinedElt.pop_back();
	return combinedElt;
}

// sort each element of minimalTransversals
inline std::vector<std::string> sortVectorOfString(const std::vector<std::string>& strVector)
{
	std::vector<std::string> sortedList = strVector;
	transform(strVector.begin(), strVector.end(), sortedList.begin(), [&](const std::string& elt) {
		std::vector<unsigned int> v = splitToVectorOfInt(elt, ' ');
		std::sort(v.begin(), v.end());

		std::string res;
		for_each(v.begin(), v.end(), [&](unsigned int i) {
			res += std::to_string(i) + ' ';
		});
		res.pop_back();
		return res;
	});
	return sortedList;
}
