#include "Itemset.h"

// ------------------------------------------------------------------------------------------------------------------------- //

std::string Itemset::toString() const
{
	std::string res = "{";
	for_each(this->itemset.begin(), this->itemset.end(), [&](const Item& item) {
		res += std::to_string(item.attributeIndex);
		res += ",";
		});
	res.pop_back();
	res += "}";
	return res;
}

std::string Itemset::itemsetListToString(const std::vector<Itemset>& v)
{
	std::string res = "{";
	for_each(v.begin(), v.end(), [&](const Itemset& elt) {
		res += elt.toString();
		res += ",";
		});
	res.pop_back();
	res += "}";
	return res;
}

//// sort each element of minimalTransversals
//static std::vector<Itemset> sortVectorOfItemset(const std::vector<Itemset>& strVector)
//{
//	std::vector<Itemset> sortedList = strVector;
//	std::transform(strVector.begin(), strVector.end(), sortedList.begin(), [&](const Itemset& elt) {
//		//Itemset v = splitToVectorOfInt(elt, ' ');	
//		Itemset v = elt;
//		std::sort(v.itemset.begin(), v.itemset.end());
//
//		Itemset res;
//		for_each(v.itemset.begin(), v.itemset.end(), [&](unsigned int i) {
//			res.itemset.push_back(i);
//			});
//		return res;
//		});
//	return sortedList;
//}

// ------------------------------------------------------------------------------------------------------------------------- //

Itemset Itemset::combineItemset(const Itemset& str1, const Itemset& str2)
{
	// "1" + "2" => "12"
	// "71" + "72" => "712"
	Itemset left = str1;
	Itemset right = str2;

	// remove duplicate indexes from left into right itemset
	std::vector<Itemset> combinedListElt;
	for_each(str1.itemset.begin(), str1.itemset.end(), [&](const Item& item) {
		//auto it = std::find_if(right.itemset_list.begin(), right.itemset_list.end(), Utils::compare_int(i));
		//auto it = std::find(right.itemset.begin(), right.itemset.end(), item.attributeIndex);
		
		// search if right itemset contains current item from left
		auto it = right.itemset.begin();
		for (; it != right.itemset.end(); it++)
		{
			if (it->attributeIndex == item.attributeIndex)
				break;
		}		
		if (it != right.itemset.end())
		{
			// remove duplicate elt
			right.itemset.erase(it);
		}
	});

	// merge right itemset at the end of left itemset
	std::copy(right.itemset.begin(), right.itemset.end(), std::back_inserter(left.itemset));

	// create a new combined itemset
	//Itemset combinedElt;
	//std::copy(left.itemset_list.begin(), left.itemset_list.end(), std::back_inserter(combinedElt.itemset_list));

	// compute OR value from left and right itemsets
	if (!str1.dirty || !str2.dirty)
	{
		//if (str1.or_value)
		//	left.or_value = str2.or_value;
		//else if (!str2.or_value)
		//	left.or_value = str1.or_value;
		//else
		left.orValue = str1.orValue | str2.orValue;
		left.bitsetCount = left.orValue.count();
		left.dirty = false;
	}
	return left;
};
