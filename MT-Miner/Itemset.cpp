#include "Itemset.h"
#include <unordered_map>
#include "BinaryRepresentation.h"

Itemset::Itemset()
{
	// set default value
	this->orSupport = 0;
	this->dirty = true;
	this->hasClone = false;
#ifndef _OLD_ISESSENTIAL
	this->isEssential = true;
#endif
}

bool Itemset::containsAClone() const
{
	return this->hasClone;
}

void Itemset::addFirstItem(const std::shared_ptr<Item>& item)
{
	assert(this->itemset.size() == 0);

	this->orValue = item->staticBitset;
	this->orSupport = this->orValue.count();
	this->dirty = false;
	// a itemset with one element is essential
#ifndef _OLD_ISESSENTIAL
	this->isEssential = true;
	this->isEssentialADNBitset = item->staticBitset;
#endif
	// update clone
	if (item->isClone)
		this->hasClone = true;
	// add item
	this->itemset.push_back(item);
}

// not optimized, only for test
void Itemset::addItem(const std::shared_ptr<Item>& item)
{
	assert(this->itemset.size() == 0);

	this->orValue = item->staticBitset | this->orValue;
	this->orSupport = this->orValue.count();
	this->dirty = false;
	// update clone
	if (item->isClone)
		this->hasClone = true;
	// update is essential value
#ifndef _OLD_ISESSENTIAL
	updateIsEssentialSparseMatrix(item);
#endif
	// add item
	this->itemset.push_back(item);
}

bool Itemset::operator==(const Itemset& other)
{
	// first test if support is different, itemsets are differents
	if ((!this->dirty && !other.dirty) && (this->orSupport != other.orSupport))
		return false;

	// test each attributeIndex of itemset
	auto it1 = other.itemset.begin();
	for (auto it2 = this->itemset.begin(); it2 != this->itemset.end(); it1++, it2++)
	{
		if ((*it1)->attributeIndex != (*it2)->attributeIndex)
			return false;
	}
	return true;
}

// make a copy of currentItemset and replance ith item by clone item
std::shared_ptr<Itemset> Itemset::createAndReplaceItem(unsigned int iToReplace, const std::shared_ptr<Item>& itemToReplace)
{
	std::shared_ptr<Itemset> clonedItemset = std::make_shared<Itemset>();
	clonedItemset->orValue = this->orValue;
	clonedItemset->orSupport = this->orSupport;
	clonedItemset->dirty = this->dirty;
	clonedItemset->hasClone = this->hasClone;
#ifndef _OLD_ISESSENTIAL
	clonedItemset->isEssential = this->isEssential;
	clonedItemset->isEssentialADNBitset = this->isEssentialADNBitset;
#endif

	for (unsigned int i = 0; i < this->getItemCount(); i++)
	{
		if (iToReplace == i)
		{
			clonedItemset->itemset.push_back(itemToReplace);
			if(itemToReplace->isAClone())
				clonedItemset->hasClone = true;
		}
		else
		{
			//std::shared_ptr<Item> item = this->getItem(i);
			//this->itemset.push_back(std::make_shared<Item>(item));
			clonedItemset->itemset.push_back(this->itemset[i]);
		}
	}
	return clonedItemset;
}

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

void Itemset::recurseOnClonedItemset(unsigned int iItem, std::vector<std::shared_ptr<Itemset>>& graph_mt)
{
	assert(iItem < this->getItemCount());

	std::shared_ptr<Item> item = this->itemset[iItem];

	// test if current item contains an original for all its items
	if (item->isAnOriginal())
	{
		// item is an original
		// create a new itemset by replacing original with its clone and update graph mt list
		// then recurse on new itemset
		for (unsigned int j = 0, cloneCount = item->getCloneCount(); j < cloneCount; j++)
		{
			// get clone index for current itemset
			std::shared_ptr<Item> clone = item->getClone(j);

			// make a copy of currentItemset and replace ith item by clone item
			std::shared_ptr<Itemset> clonedItemset = this->createAndReplaceItem(iItem, clone);

			graph_mt.push_back(clonedItemset);

			// recurse on new cloned itemset to replace kth original by 
			for (unsigned int k = iItem, n = clonedItemset->getItemCount(); k < n; k++)
				clonedItemset->recurseOnClonedItemset(k, graph_mt);
		}
	}
}

std::shared_ptr<Itemset> Itemset::combineItemset(const std::shared_ptr<Itemset>& itemset_left, const std::shared_ptr<Itemset>& itemset_right)
{
	// "1" + "2" => "12"
	// "71" + "72" => "712"
	
	// create a new itemset from left itemset
	std::shared_ptr<Itemset> combinedItemset = std::make_shared<Itemset>(*itemset_left);

	// 
	for_each(itemset_right->itemset.begin(), itemset_right->itemset.end(), [combinedItemset](const std::shared_ptr<Item>& item) {
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
			// add item into itemset list
			combinedItemset->itemset.push_back(item);
#ifndef _OLD_ISESSENTIAL
			// 
			combinedItemset->updateIsEssentialSparseMatrix(item);
#endif
			// update disjonctive support
			combinedItemset->orValue = combinedItemset->orValue | item->staticBitset;
			// update clone status
			if (item->isClone)
				combinedItemset->hasClone = true;
		}
	});

	// update support
	combinedItemset->orSupport = combinedItemset->orValue.count();
	// combined item set is not dirty, all values have been computed
	combinedItemset->dirty = false;

	return combinedItemset;
};

#ifndef _OLD_ISESSENTIAL

// return true if element is essential
bool Itemset::computeIsEssential()
{
	if (this->getItemCount() == 1)
	{
		this->isEssential = true;
	}
	else
	{
		// it itemset is not essential, it wont be essential with a new item
		if (this->isEssential)
		{
			//is->isEssential = false;
			for (int i = 0, n = this->getItemCount(); i != n; i++)
			{
				// test for each bits AND operator between current bitset and ADN bitset holder
				StaticBitset res = this->itemset[i]->staticBitset & this->isEssentialADNBitset;
				if (res.any())
				{
					this->isEssential = true;
				}
				else
				{
					// this item is not essential, so itemset is not essential, we can leave
					this->isEssential = false;
					break;
				}
			}
		}
	}
	return this->isEssential;
}
	
#else

// return true if element is essential
bool Itemset::computeIsEssential()
{
	if (this->getItemCount() == 1)
	{
		return true;
	}
	else
	{
		unsigned int objectCount = BinaryRepresentation::getObjectCount();
		bool isEssential = false;
		StaticBitset SumOfN_1Items;
		for (int i = 0, n = this->getItemCount(); i != n; i++)
		{
			// dont forget to initialize boolean
			SumOfN_1Items.reset();
			isEssential = false;

			for (int j = 0; j < n; j++)
			{
				if (i != j)
				{
					StaticBitset bitset = this->itemset[j]->staticBitset;
					if (!bitset.none())
						SumOfN_1Items = SumOfN_1Items | bitset;
				}
			}

			StaticBitset bitset = this->itemset[i]->staticBitset;
			for (unsigned int k = 0; k < objectCount; k++)
			{
				// compare bit
				bool bit0 = SumOfN_1Items[k];
				bool bit1 = bitset[k];
				if (!bit0 && bit1)
				{
					// this bitset is essential, check with next bitset
					isEssential = true;
					break;
				}
			}

			// one item is not essential, we can return  
			if (!isEssential)
			{
				// this bitset is not essential, break the main loop and return false
				return false;
				//break;
			}
		}
		return isEssential;
	}	
}

#endif

#ifndef _OLD_ISESSENTIAL
void Itemset::updateIsEssentialSparseMatrix(const std::shared_ptr<Item>& item)
{
	//StaticBitset isEssentialADNBitset_2 = this->isEssentialADNBitset;
	//StaticBitset markedNonEssetialBisetIndex_2 = this->markedNonEssetialBisetIndex;

	/*for (unsigned int i = 0, n = BinaryRepresentation::getObjectCount(); i < n; i++)
	{
		if (item->staticBitset[i])
		{
			if (this->isEssentialADNBitset[i] || (this->markedNonEssetialBisetIndex[i]))
			{
				// ith bit was already part of minimal ADN
				// we have to remove it from the minimal ADN and remove ith bit as a candidate for minimal AND index
				this->isEssentialADNBitset[i] = false;
				this->markedNonEssetialBisetIndex[i] = true;
			}
			else
			{
				this->isEssentialADNBitset[i] = true;
				this->markedNonEssetialBisetIndex[i] = false;
			}
		}
	}*/


	//StaticBitset res_test = item->staticBitset & (this->isEssentialADNBitset | this->markedNonEssetialBisetIndex);

	StaticBitset res = this->isEssentialADNBitset | this->markedNonEssetialBisetIndex;
	//StaticBitset res1 = item->staticBitset & res;
	//StaticBitset res2 = item->staticBitset & ~res;
	
	//#pragma omp parallel
	{
		int i = 0;
		int n = BinaryRepresentation::getObjectCount();
		//#pragma omp for
		for (i = 0; i < n; i++)
		{
			if (item->staticBitset.test(i) && res.test(i))
			{
				// ith bit was already part of minimal ADN
				// we have to remove it from the minimal ADN and remove ith bit as a candidate for minimal AND index
				this->isEssentialADNBitset[i] = false;
				this->markedNonEssetialBisetIndex[i] = true;
			}
			else if (item->staticBitset.test(i) && !res.test(i))
			{
				this->isEssentialADNBitset[i] = true;
				this->markedNonEssetialBisetIndex[i] = false;
			}
		}
	}

	/*if (item->staticBitset.any())
	{
		StaticBitset res = item->staticBitset & (this->isEssentialADNBitset | this->markedNonEssetialBisetIndex);
		this->markedNonEssetialBisetIndex = res;
		this->isEssentialADNBitset = ~res;
	}*/

	/*StaticBitset orRes = item->staticBitset | isEssentialADNBitset_2;
	StaticBitset andRes = orRes & markedNonEssetialBisetIndex_2;
	if (!andRes.count())
	{
		isEssentialADNBitset_2 = orRes;
		assert(isEssentialADNBitset_2 == this->isEssentialADNBitset);
	}
	else
	{
		// set to false every bit where andRes is set
		isEssentialADNBitset_2 = andRes ^ orRes;
		// set to true every bit where andRes is set
		StaticBitset markedNonEssetialBisetIndex_2 = item->staticBitset & isEssentialADNBitset_2;

		assert(isEssentialADNBitset_2 == this->isEssentialADNBitset);
		assert(markedNonEssetialBisetIndex_2 == this->markedNonEssetialBisetIndex);
	}*/
}
#endif



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
