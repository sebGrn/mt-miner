#include "Itemset.h"
#include <unordered_map>

Itemset::Itemset()
{
	this->orSupport = 0;
	this->dirty = true;
	this->isEssential = false;
	this->hasClone = false;
}

Itemset::Itemset(const Itemset* itemset)
{
	this->orValue = itemset->orValue;
	this->orSupport = itemset->orSupport;
	this->dirty = itemset->dirty;
	this->isEssential = itemset->isEssential;
	this->hasClone = itemset->hasClone;
	// copy items
	for (auto it = itemset->itemset.begin(); it != itemset->itemset.end(); it++)
		this->itemset.push_back(std::make_shared<Item>(it->get()));
}

bool Itemset::containsAClone() const
{
	return this->hasClone;
}

void Itemset::addFirstItem(const std::shared_ptr<Item>& item)
{
	assert(this->itemset.size() == 0);

#ifdef NEW_ESSENTIAL
	for (auto it = item->sparseBitset.bitset_value.begin(); it != item->sparseBitset.bitset_value.end(); it++)
	{
		this->minimalTransaction.push_back(std::pair<unsigned int, unsigned int>(*it, 0));
	}
#endif

	this->orValue = item->staticBitset;
	this->orSupport = this->orValue.count();
	this->dirty = false;
	this->isEssential = false;
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

#ifdef NEW_ESSENTIAL
	for (auto it = item->sparseBitset.bitset_value.begin(); it != item->sparseBitset.bitset_value.end(); it++)
	{
		this->minimalTransaction.push_back(std::pair<unsigned int, unsigned int>(*it, 0));
	}
#endif

	this->orValue = item->staticBitset | this->orValue;
	this->orSupport = this->orValue.count();
	this->dirty = false;
	this->isEssential = false;
	// update clone
	if (item->isClone)
		this->hasClone = true;
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

void Itemset::replaceItem(unsigned int i, const std::shared_ptr<Item>& item)
{
	assert(i < this->itemset.size());
	if (i < this->itemset.size())
	{
		// remove previous element
		this->itemset.erase(this->itemset.begin() + i);
		// insert element
		this->itemset.insert(this->itemset.begin() + i, item);

		// an item has changed, compute clone for all item
		this->hasClone = false;
		for (auto elt : this->itemset)
		{
			if (elt->isAClone())
			{
				this->hasClone = true;
				break;
			}
		}
	}
}

// make a copy of currentItemset and replance ith item by clone item
std::shared_ptr<Itemset> Itemset::createAndReplaceItem(unsigned int iToReplace, const std::shared_ptr<Item>& itemToReplace)
{
	std::shared_ptr<Itemset> clonedItemset = std::make_shared<Itemset>();
	clonedItemset->orValue = this->orValue;
	clonedItemset->orSupport = this->orSupport;
	clonedItemset->dirty = this->dirty;
	clonedItemset->isEssential = this->isEssential;
	clonedItemset->hasClone = this->hasClone;

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
			clonedItemset->itemset.push_back(this->getItem(i));
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


#ifdef NEW_ESSENTIAL
void Itemset::UpdateIsEssential(const std::shared_ptr<Item>& item)
{
	// if isMinimal is false, it means that there is a transaction where '1' is present for each item

	// 1st step : update minimalTransaction list

	// try with AND operator on static bitset
	/*unsigned int iItem(0);
	for (auto it_item = this->itemset.begin(); it_item != this->itemset.end(); it_item++, iItem++)
	{
		StaticBitset bitset = (*it_item)->staticBitset & item->staticBitset;
		for (unsigned int iTransaction = 0, n = bitset.size(); iTransaction < n; iTransaction++)
		{
			if (bitset.test(iTransaction))
			{
				// erase the minimal transaction at transaction number iTransaction
				for (auto it_minimalTransaction = this->minimalTransaction.begin(); it_minimalTransaction != this->minimalTransaction.end(); it_minimalTransaction++)
				{
					if (iTransaction == (*it_minimalTransaction).first)
					{
						this->minimalTransaction.erase(it_minimalTransaction);
						// transaction has been found, we can leave
						break;
					}
				}
			}
			else
			{
				if (item->staticBitset.test(iTransaction))
				{
					this->minimalTransaction.push_back(std::pair<unsigned int, unsigned int>(iTransaction, iItem + 1));
				}
			}
		}
	}*/


	// loop on bits from item's bitset (check sparse bitset index)	
	for (auto it_elt = item->sparseBitset.bitset_value.begin(); it_elt != item->sparseBitset.bitset_value.end(); it_elt++)
	{
		// the transation (line) number is set
		unsigned int iTransaction = (*it_elt);
		bool isMinimal = false;
		// search if we have a transaction into itemset (for each line) where we have a bit at iTransaction value
		unsigned int iItem(0);
		for (auto it_item = this->itemset.begin(); it_item != this->itemset.end(); it_item++, iItem++)
		{
			// very slow !!!
			// find iTransaction into sparce bitset
			isMinimal = true;
			//std::cout << (*it_item)->sparseBitset.bitset_value.size() << std::endl;
			for (auto it_sparse = (*it_item)->sparseBitset.bitset_value.begin(); it_sparse != (*it_item)->sparseBitset.bitset_value.end(); it_sparse++)
			{
				if ((*it_sparse) == iTransaction)
				{
					isMinimal = false;

					// find if iTransaction is in minimalTransaction
					for (auto it_minimalTransaction = this->minimalTransaction.begin(); it_minimalTransaction != this->minimalTransaction.end(); it_minimalTransaction++)
					{
						if (iTransaction == (*it_minimalTransaction).first)
						{
							// erase the minimal transaction
							this->minimalTransaction.erase(it_minimalTransaction);
							break;
						}
					}
					break;
				}
			}
			//auto it_res = std::find((*it_item)->sparseBitset.bitset_value.begin(), (*it_item)->sparseBitset.bitset_value.end(), iTransaction);
			//isMinimal = (it_res == (*it_item)->sparseBitset.bitset_value.end());

			if (!isMinimal)
			{
				break;
			}
		}

		// 
		if (isMinimal)
			this->minimalTransaction.push_back(std::pair<unsigned int, unsigned int>(iTransaction, iItem));
	}

	// 2nd step : check minimalTransaction list and update isEssential

	this->isEssential = true;
	// check we have only on "1" for each item (column) in minimalTransaction list
	// add 1 on itemlist size for next item which is not in the list
	for (unsigned int i = 0, n = this->itemset.size() + 1; i < n; i++)
	{
		auto it = std::find_if(this->minimalTransaction.begin(), this->minimalTransaction.end(), [i](const std::pair<unsigned int, unsigned int>& a) {
			return a.second == i;
			});
		if (it == this->minimalTransaction.end())
		{
			// an item exists without set bit
			this->isEssential = false;
			break;
		}
	}
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
