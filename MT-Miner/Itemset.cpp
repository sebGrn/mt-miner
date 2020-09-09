#include "Itemset.h"
#include <unordered_map>

Itemset::Itemset()
{
	this->orSupport = 0;
	this->dirty = true;
	this->isEssential = false;
}

Itemset::Itemset(const Itemset* itemset)
{
	this->orValue = itemset->orValue;
	this->orSupport = itemset->orSupport;
	this->dirty = itemset->dirty;
	this->isEssential = itemset->isEssential;
	// copy items
	for (auto it = itemset->itemset.begin(); it != itemset->itemset.end(); it++)
		this->itemset.push_back(std::make_shared<Item>(it->get()));
}

bool Itemset::containsAClone() const
{
	for (auto elt : this->itemset)
	{
		if (elt->isAClone())
			return true;
	}
	return false;
}

void Itemset::addItem(const std::shared_ptr<Item>& item)
{
	if (this->itemset.size() == 0)
	{
		//Item::buildSparseMatrix(this->cumulatedMatrix, item->staticBitset);

		for (auto it = item->sparseBitset.bitset_value.begin(); it != item->sparseBitset.bitset_value.end(); it++)
		{
			this->minimalTransaction.push_back(std::pair<unsigned int, unsigned int>(*it, 0));
		}

		this->orValue = item->staticBitset;
		this->orSupport = this->orValue.count();
		this->dirty = false;
		this->isEssential = false;
	}
	else
	{
		this->orValue = this->orValue | item->staticBitset;
		this->orSupport = this->orValue.count();
		UpdateIsEssential(item);
	}
	this->itemset.push_back(item);
}

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
			
			if(!isMinimal)
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
	}
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
	
	// create a new itemset
	std::shared_ptr<Itemset> combinedItemset = std::make_shared<Itemset>();
	// copy left item set into new combined itemset
	//std::copy(itemset_left->itemset.begin(), itemset_left->itemset.end(), std::back_inserter(combinedItemset->itemset));
	for (auto it = itemset_left->itemset.begin(); it != itemset_left->itemset.end(); it++)
		combinedItemset->addItem(*it);

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
			combinedItemset->addItem(item);
		}
	});

	// compute OR value from left and right itemsets
	//if (itemset_left->dirty && itemset_right->dirty)
	//{
	//	combinedItemset->orValue = itemset_left->orValue | itemset_right->orValue;
	//	combinedItemset->orSupport = combinedItemset->orValue.count();
	//	combinedItemset->dirty = false;
	//}
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
