#include "Itemset.h"

// ------------------------------------------------------------------------------------------------------------------------- //

std::string Itemset::toString() const
{
	std::string res = "{";
	for_each(this->itemset.begin(), this->itemset.end(), [&](const std::shared_ptr<Item>& item) {
		res += std::to_string(item->attributeIndex);
		res += ",";
		});
	res.pop_back();
	res += "}";
	return res;
}

//std::string Itemset::itemsetListToString(const std::vector<Itemset>& v)
//{
//	std::string res = "{";
//	for_each(v.begin(), v.end(), [&](const Itemset& elt) {
//		res += elt.toString();
//		res += ",";
//		});
//	res.pop_back();
//	res += "}";
//	return res;
//}

std::shared_ptr<Itemset> Itemset::combineItemset(const std::shared_ptr<Itemset>& itemset_left, const std::shared_ptr<Itemset>& itemset_right)
{
	// "1" + "2" => "12"
	// "71" + "72" => "712"
	std::shared_ptr<Itemset> combinedItemset = std::make_shared<Itemset>();
	std::copy(itemset_left->itemset.begin(), itemset_left->itemset.end(), std::back_inserter(combinedItemset->itemset));

	// 
	for_each(itemset_right->itemset.begin(), itemset_right->itemset.end(), [&](const std::shared_ptr<Item>& item) {
		// search if right itemset contains current item from left
		bool found = false;
		for (auto it = combinedItemset->itemset.begin(); it != combinedItemset->itemset.end(); it++)
		{
			if ((*it)->attributeIndex == item->attributeIndex)
			{
				found = true;
				break;
			}				
		}
		if (!found)
		{
			// didnt find duplicates, we can add the item at the end of the list
			combinedItemset->itemset.push_back(item);
		}
	});

	// compute OR value from left and right itemsets
	if (!itemset_left->dirty || !itemset_right->dirty)
	{
		combinedItemset->orValue = itemset_left->orValue | itemset_right->orValue;
		combinedItemset->bitsetCount = combinedItemset->orValue.count();
		combinedItemset->dirty = false;
	}
	return combinedItemset;
};

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
