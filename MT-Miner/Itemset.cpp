#include "Itemset.h"
#include <unordered_map>
#include "BinaryRepresentation.h"

Itemset::Itemset()
{
	this->orValue = std::make_unique<StaticBitset>();
	this->orSupport = 0;
	this->dirty = true;
	this->hasClone = false;
#ifndef _OLD_ISESSENTIAL
	this->isEssential = true;
	this->isEssentialADNBitset = std::make_unique<StaticBitset>();
	this->markedNonEssentialBitsetIndex = std::make_unique<StaticBitset>();
	this->temporaryBitset = std::make_unique<StaticBitset>();
	this->isEssentialADNBitset->reset();
	this->markedNonEssentialBitsetIndex->reset();
	this->temporaryBitset->reset();
#endif
}

Itemset::Itemset(const std::shared_ptr<Item>& item)
{
	assert(this->itemset.empty());

	this->orValue = std::make_unique<StaticBitset>(*item->staticBitset);
	this->orSupport = item->count();
	this->dirty = false;
	this->hasClone = false;
#ifndef _OLD_ISESSENTIAL
	this->isEssential = true;
	this->isEssentialADNBitset = std::make_unique<StaticBitset>(*this->orValue);
	this->markedNonEssentialBitsetIndex = std::make_unique<StaticBitset>();
	this->temporaryBitset = std::make_unique<StaticBitset>();
	this->markedNonEssentialBitsetIndex->reset();
	this->temporaryBitset->reset();
#endif
	if (item->isAClone())
		this->hasClone = true;

	this->itemset.push_back(item);
}

Itemset::Itemset(const std::shared_ptr<Itemset> itemset)
{
	assert(this->itemset.empty());

	this->orValue = std::make_unique<StaticBitset>(*itemset->orValue);
	this->orSupport = itemset->orSupport;
	this->dirty = itemset->dirty;
	this->hasClone = itemset->hasClone;
#ifndef _OLD_ISESSENTIAL
	this->isEssential = itemset->isEssential;
	this->isEssentialADNBitset = std::make_unique<StaticBitset>(*itemset->isEssentialADNBitset);
	this->markedNonEssentialBitsetIndex = std::make_unique<StaticBitset>(*itemset->markedNonEssentialBitsetIndex);
	this->temporaryBitset = std::make_unique<StaticBitset>();
#endif
	std::copy(itemset->itemset.begin(), itemset->itemset.end(), std::back_inserter(this->itemset));	
}

Itemset::~Itemset()
{
#ifndef _OLD_ISESSENTIAL
	this->isEssentialADNBitset.reset();
	this->markedNonEssentialBitsetIndex.reset();
	this->markedNonEssentialBitsetIndex.reset();
	this->temporaryBitset.reset();
#endif
	this->itemset.clear();
}

// not optimized, only for test
void Itemset::addItem(const std::shared_ptr<Item>& item)
{
	//(*this->orValue) = (*item->value()) | (*this->orValue);
	(*this->orValue) = (*item->staticBitset) | (*this->orValue);
	this->orSupport = (*this->orValue).count();
	this->dirty = false;
	if (item->isAClone())
		this->hasClone = true;

#ifndef _OLD_ISESSENTIAL
	updateIsEssential(item);
#endif

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
		if ((*it1)->getAttributeIndex() != (*it2)->getAttributeIndex())
			return false;
	}
	return true;
}

// make a copy of currentItemset and replance ith item by clone item
std::shared_ptr<Itemset> Itemset::createAndReplaceItem(unsigned int iToReplace, const std::shared_ptr<Item>& itemToReplace)
{
	try
	{
		std::shared_ptr<Itemset> clonedItemset(new Itemset());
		if (clonedItemset)
		{
			(*clonedItemset->orValue) = (*this->orValue);
			clonedItemset->orSupport = this->orSupport;
			clonedItemset->dirty = this->dirty;
			clonedItemset->hasClone = this->hasClone;
#ifndef _OLD_ISESSENTIAL
			clonedItemset->isEssential = this->isEssential;
			(*clonedItemset->isEssentialADNBitset) = (*this->isEssentialADNBitset);
#endif
			for (unsigned int i = 0; i < this->getItemCount(); i++)
			{
				if (iToReplace == i)
				{
					clonedItemset->itemset.push_back(itemToReplace);
					if (itemToReplace->isAClone())
						clonedItemset->hasClone = true;
				}
				else
				{
					clonedItemset->itemset.push_back(this->itemset[i]);
				}
			}
		}
		return clonedItemset;
	}
	catch (std::exception& e)
	{
		std::cout << "during createAndReplaceItem " << e.what() << std::endl;
	}
	return nullptr;
}

void Itemset::combineItemset(const std::shared_ptr<Itemset> itemset_right)
{
	// "1" + "2" => "12"
	// "71" + "72" => "712"

	for (auto it_item = itemset_right->itemset.begin(); it_item != itemset_right->itemset.end(); it_item++)
	{
		// search if right itemset contains current item from left
		bool found = false;
		for (auto it = this->itemset.begin(); it != this->itemset.end(); it++)
		{
			if ((*it)->getAttributeIndex() == (*it_item)->getAttributeIndex())
			{
				found = true;
				break;
			}
		}
		if (!found)
		{
			// didnt find duplicates, we can add the item at the end of the list
			// add item into itemset list
			this->itemset.push_back((*it_item));

#ifndef _OLD_ISESSENTIAL
			//
			updateIsEssential((*it_item));
#endif
			// update disjonctive support
			(*this->orValue) = (*(*it_item)->staticBitset) | (*this->orValue);
			// update clone status
			if ((*it_item)->isAClone())
				this->hasClone = true;
		}

		// update support
		this->orSupport = (*this->orValue).count();
		// combined item set is not dirty, all values have been computed
		this->dirty = false;
	}
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
			for (int i = 0, n = this->getItemCount(); i != n; i++)
			{
				// test for each bits AND operator between current bitset and ADN bitset holder
				(*this->temporaryBitset) = (*this->itemset[i]->staticBitset) & (*this->isEssentialADNBitset);
				this->isEssential = temporaryBitset->any();
				if(!this->isEssential)
				{
					// this item is not essential, so itemset is not essential, we can leave
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
void Itemset::updateIsEssential(const std::shared_ptr<Item>& item)
{
	(*this->temporaryBitset) = (*this->isEssentialADNBitset) | (*this->markedNonEssentialBitsetIndex);
	
	int i = 0;
	int n = BinaryRepresentation::getObjectCount();
	for (i = 0; i < n; i++)
	{
		if (item->get(i))
		{
			if (this->temporaryBitset->test(i))
			{
				// ith bit was already part of minimal ADN	
				// we have to remove it from the minimal ADN and remove ith bit as a candidate for minimal AND index
				this->isEssentialADNBitset->set(i, false);
				this->markedNonEssentialBitsetIndex->set(i, true);
			}
			else 
			{
				this->isEssentialADNBitset->set(i, true);
				this->markedNonEssentialBitsetIndex->set(i, false);
			}
		}
	}
}
#endif

unsigned int Itemset::computeDisjunctifSupport(const Itemset& left, const std::shared_ptr<Itemset> right)
{
	StaticBitset* res = new StaticBitset();
	(*res) = (*left.orValue) | (*right->orValue);
	unsigned int r = res->count();
	delete res;
	return r;
}

void Itemset::combineRightIntoLeft(Itemset& itemset_left, const std::shared_ptr<Itemset> itemset_right)
{
	// "1" + "2" => "12"
	// "71" + "72" => "712"
	// combine without duplicates
	for (auto it_item = itemset_right->itemset.begin(); it_item != itemset_right->itemset.end(); it_item++)
	{
		// search if right itemset contains current item from left
		bool found = false;
		for (auto it = itemset_left.itemset.begin(); it != itemset_left.itemset.end(); it++)
		{
			if ((*it)->getAttributeIndex() == (*it_item)->getAttributeIndex())
			{
				found = true;
				break;
			}
		}
		if (!found)
		{
			// didnt find duplicates, we can add the item at the end of the list
			// add item into itemset list
			itemset_left.itemset.push_back((*it_item));
#ifndef _OLD_ISESSENTIAL
			// update isEssential
			itemset_left.updateIsEssential((*it_item));
#endif
		}
	}
	
	// dont need to loop on all items, just combine itemset properties
	(*itemset_left.orValue) = (*itemset_left.orValue) | (*itemset_right->orValue);
	itemset_left.orSupport = (*itemset_left.orValue).count();
	itemset_left.hasClone = itemset_left.hasClone | itemset_right->hasClone;
	itemset_left.dirty = false;
}

void Itemset::copyRightIntoLeft(Itemset& left, const std::shared_ptr<Itemset> right)
{
	left.itemset.clear();
	
	for (auto it = right->itemset.begin(); it != right->itemset.end(); it++)
		left.itemset.push_back(*it);

	left.dirty = right->dirty;
	*left.orValue = *right->orValue;
	left.orSupport = right->orSupport;
	left.hasClone = right->hasClone;
	
#ifndef _OLD_ISESSENTIAL
	left.isEssential = right->isEssential;
	*left.isEssentialADNBitset = *right->isEssentialADNBitset;
	*left.markedNonEssentialBitsetIndex = *right->markedNonEssentialBitsetIndex;
#endif	
}

std::string Itemset::toString() const
{
	std::string res = "{";
	for_each(this->itemset.begin(), this->itemset.end(), [&](const std::shared_ptr<Item>& item) {
		res += std::to_string(item->getAttributeIndex());
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
