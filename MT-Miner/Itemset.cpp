#include "Itemset.h"
#include <unordered_map>
#include <bit>
#include "BinaryRepresentation.h"

Itemset::ItemsetType Itemset::itemsetType = Itemset::ItemsetType::DISJUNCTIVE;

Itemset::Itemset()
{
	this->supportBitset = std::make_unique<StaticBitset>();
#ifdef ISESSENTIAL_ON_TOEXPLORE
	this->isEssential = false; 
#endif	
	this->cumulatedXorbitset = std::make_unique<StaticBitset>();
	this->noiseBitset = std::make_unique<StaticBitset>();

	this->supportValue = 0;
	this->dirty = true;
	this->hasClone = false;	
}

Itemset::Itemset(Item* item)
{
	assert(this->itemset.empty());
	this->supportBitset = std::make_unique<StaticBitset>(*item->staticBitset);

	this->cumulatedXorbitset = std::make_unique<StaticBitset>(*item->staticBitset);
	this->noiseBitset = std::make_unique<StaticBitset>();

#ifdef ISESSENTIAL_ON_TOEXPLORE
	this->isEssential = false;
#endif

	this->supportValue = item->count();
	this->dirty = false;
	this->hasClone = false;
	if (item->isAClone())
		this->hasClone = true;

	this->itemset.push_back(item);
}

Itemset::Itemset(const std::shared_ptr<Itemset>& itemset)
{
	assert(this->itemset.empty());
	this->supportBitset = std::make_unique<StaticBitset>(*itemset->supportBitset);
#ifdef ISESSENTIAL_ON_TOEXPLORE
	this->isEssential = itemset->isEssential;
#endif
	this->cumulatedXorbitset = std::make_unique<StaticBitset>(*itemset->cumulatedXorbitset);
	this->noiseBitset = std::make_unique<StaticBitset>(*itemset->noiseBitset);

	this->supportValue = itemset->supportValue;
	this->dirty = itemset->dirty;
	this->hasClone = itemset->hasClone;

	this->itemset.reserve(itemset->itemset.size());
	std::copy(itemset->itemset.begin(), itemset->itemset.end(), std::back_inserter(this->itemset));	
}

Itemset::~Itemset()
{
	this->itemset.clear();
}

// make a copy of currentItemset and replace ith item by clone item
std::shared_ptr<Itemset> Itemset::createAndReplaceItem(unsigned int iToReplace, Item* itemToReplace)
{
	try
	{
		std::shared_ptr<Itemset> clonedItemset(new Itemset());
		if (clonedItemset)
		{
			(*clonedItemset->supportBitset) = (*this->supportBitset);

			(*clonedItemset->cumulatedXorbitset) = (*this->cumulatedXorbitset);
			(*clonedItemset->noiseBitset) = (*this->noiseBitset);
#ifdef ISESSENTIAL_ON_TOEXPLORE
			this->isEssential = false;
#endif
			clonedItemset->supportValue = this->supportValue;
			clonedItemset->dirty = this->dirty;
			clonedItemset->hasClone = this->hasClone;

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

bool Itemset::isEssentialRapid(std::shared_ptr<Itemset>& left, std::shared_ptr<Itemset>& right)
{
	auto itItemToAdd = right->itemset.end() - 1;
	
	StaticBitset combined_bitset;
	if (itemsetType == CONSJONCTIVE)
		combined_bitset = (*(*itItemToAdd)->staticBitset) & (*left->supportBitset);
	else
		combined_bitset = (*(*itItemToAdd)->staticBitset) | (*left->supportBitset);
	
	unsigned int supportCombined = combined_bitset.count();	

	// si support 715 == support 71 ou support 75
	if (supportCombined == left->getSupport() || supportCombined == right->getSupport())
	{
		return false;
	}

	// first test : or(left) xor or(right)
	StaticBitset tmp_xor = (*left->supportBitset) ^ (*right->supportBitset);
	if (tmp_xor.none())
	{
		// all bits from left and right are "1", this is not essential
		// we dont have here a potentiel candidate for is essential
		return false;
	}

	/*
	if (left->getSupport() + right->getSupport() == left->getSupport())
	{
		//std::cout << "OPTIMIZED 1" << std::endl;
		return false;
	}

	if (combined->getSupport() == combined->cumulatedXorbitset->count())
	{
		//std::cout << "OPTIMIZED 2" << std::endl;
		return false;
	}

	if (combined->getSupport() == right->getSupport())
	{
		//std::cout << "OPTIMIZED 2" << std::endl;
		return false;
	}*/

	return true;
}

void Itemset::combineItemset(const Itemset* itemset_right)
{
	/// return true / false
	/// return true --> combine ok
	/// return false --> combine pas ok
	/// if (support left + right == support left) OR (support left + right == support right) RETURN FALSE
	/// IF (SUPPORT itemset == SUPPORT cumulatedXorbitset) OR (SUPPORT itemset == bitsetToAdd) RETURN FALSE
	/// --> rapidEssentiality


	// "1" + "2" => "12"
	// "71" + "72" => "712"
	// we can always add the last one
	auto itItemToAdd = itemset_right->itemset.end() - 1;

	// update support
	if (itemsetType == CONSJONCTIVE)
		(*this->supportBitset) = (*(*itItemToAdd)->staticBitset) & (*this->supportBitset);
	else
		(*this->supportBitset) = (*(*itItemToAdd)->staticBitset) | (*this->supportBitset);
	
#ifndef ISESSENTIAL_ON_TOEXPLORE
	(*this->noiseBitset) = (*this->noiseBitset) | ((*this->cumulatedXorbitset) & (*(*itItemToAdd)->staticBitset) ^ (*this->cumulatedXorbitset) ^ (*this->cumulatedXorbitset) );
	(*this->cumulatedXorbitset) = (*(*itItemToAdd)->staticBitset) ^ (*this->cumulatedXorbitset);
#endif

	// update clone status
	if ((*itItemToAdd)->isAClone())
		this->hasClone = true;

	// update support
	this->supportValue = (*this->supportBitset).count();
	// combined item set is not dirty, all supportBitsets have been computed
	this->dirty = false;

	// finally add the last item
	this->itemset.push_back(*itItemToAdd);
};

#ifndef ISESSENTIAL_ON_TOEXPLORE

// itemset is essential if and only if we have this pattern
// 0 1
// 1 0
// or
// 1 0 0
// 0 1 0
// 0 0 1

// called every time on combine
bool Itemset::computeIsEssential(const std::shared_ptr<Itemset>& left, const std::shared_ptr<Itemset>& right)
{
	auto it_item = right->itemset.end() - 1;
	StaticBitset bitsetToAdd = (*(*it_item)->staticBitset);
	{
		StaticBitset xorBitset = (*left->cumulatedXorbitset) ^ bitsetToAdd;
		unsigned int support_xor = xorBitset.count();
		if (support_xor < (left->getItemCount() + 1))
		{
			return false;
		}
		else
		{
			// create another list to compute essentiality, pick the last one from right list
			//std::vector<Item*> itemsetList;
			//std::copy(left->itemset.begin(), left->itemset.end(), std::back_inserter(itemsetList));
			//itemsetList.push_back(*it_item);

			// add item on left list then pop it (avoid to create another list)
			left->itemset.push_back(*it_item);

			StaticBitset validatorBitset = (*left->noiseBitset);
			validatorBitset = validatorBitset.flip() & xorBitset;

			for (int i = 0, n = left->itemset.size(); i != n; i++)
			{
				StaticBitset res = (*left->itemset[i]->staticBitset) & validatorBitset;
				if (res.none())
				{
					left->itemset.pop_back();
					return false;
				}
			}
			left->itemset.pop_back();
			return true;
		}
	}
}

#else

bool Itemset::computeIsEssentialParameters(const std::shared_ptr<Itemset>& itemset, StaticBitset& cumulatedXorbitset, StaticBitset& noiseBitset)
{
	for_each(itemset->itemset.begin(), itemset->itemset.end() - 1, [&cumulatedXorbitset, &noiseBitset](Item* item) {
		noiseBitset = noiseBitset | ((cumulatedXorbitset & ((*item->staticBitset) ^ cumulatedXorbitset)) ^ cumulatedXorbitset);
		cumulatedXorbitset = cumulatedXorbitset ^ (*item->staticBitset);
		});
	return true;
}

// called only on toExplore 
bool Itemset::computeIsEssential(const std::shared_ptr<Itemset>& itemset, bool mtComputation)
{
	/// if (support left + right == support left) OR (support left + right == support right) RETURN FALSE
	/// IF (SUPPORT itemset == SUPPORT cumulatedXorbitset) OR (SUPPORT itemset == bitsetToAdd) RETURN FALSE

	if (itemset->getItemCount() == 0)
	{
		return false;
	}
	else if (itemset->getItemCount() == 1)
	{
		return true;
	}
	else
	{
		// create a temporary list from itemset without the last item (ie item do add)
		//std::vector<Item*> itemsetList;
		//std::copy(itemset->itemset.begin(), itemset->itemset.end() - 1, std::back_inserter(itemsetList));

		// compute cumulated xor for itemsetList
		StaticBitset cumulatedXorbitset;
		StaticBitset noiseBitset;
		if (!itemset->isEssential)
		{
			//if (itemsetList.size() == 1)
			if (itemset->itemset.size() == 1)
			{
				cumulatedXorbitset = *((itemset->itemset[0])->staticBitset);
				//cumulatedXorbitset = (*itemsetList[0]->staticBitset);
			}
			else
			{
				Itemset::computeIsEssentialParameters(itemset, cumulatedXorbitset, noiseBitset);
				//for_each(itemset->itemset.begin(), itemset->itemset.end() - 1, [&cumulatedXorbitset, &noiseBitset](Item* item) {
				//	noiseBitset = noiseBitset | ((cumulatedXorbitset & ((*item->staticBitset) ^ cumulatedXorbitset)) ^ cumulatedXorbitset);
				//	cumulatedXorbitset = cumulatedXorbitset ^ (*item->staticBitset);
				//	});

				//for_each(itemsetList.begin(), itemsetList.end(), [&cumulatedXorbitset, &noiseBitset](Item* item) {
				//	noiseBitset = noiseBitset | ((cumulatedXorbitset & ((*item->staticBitset) ^ cumulatedXorbitset)) ^ cumulatedXorbitset);
				//	cumulatedXorbitset = cumulatedXorbitset ^ (*item->staticBitset);
				//	});
			}
		}
		else
		{
#ifdef TRACE
//			std::cout << "optimized is essential " << std::endl;						
#endif
			// get bitsets from previous essential itemset
			cumulatedXorbitset = (*itemset->cumulatedXorbitset);
			noiseBitset = (*itemset->noiseBitset);
		}

		// check essentality with last item
		Item* right = *(itemset->itemset.end() - 1);
		StaticBitset bitsetToAdd = (*right->staticBitset);
		{
			StaticBitset xorBitset = cumulatedXorbitset ^ bitsetToAdd;
			if (xorBitset.none())
			{
				// all bits from left and right are "1", this is not essential
				// we dont have here a potentiel candidate for is essential
				//std::cout << "OPTIMIZED XOR 1" << std::endl;
				return false;
			}

			unsigned int support_xor = xorBitset.count();
			if (support_xor < itemset->itemset.size())
			{
				return false;
			}
			else
			{
				// add the last item to test to the temporary list
				//itemsetList.push_back(right);

				StaticBitset validatorBitset = noiseBitset;
				validatorBitset = validatorBitset.flip() & xorBitset;

				//for (int i = 0, n = itemsetList.size(); i != n; i++)
				for (int i = 0, n = itemset->itemset.size(); i != n; i++)
				{
					StaticBitset res = (*itemset->itemset[i]->staticBitset) & validatorBitset;
					//StaticBitset res = (*itemsetList[i]->staticBitset) & validatorBitset;
					if (res.none())
						return false;
				}

				// itemset is Essential
				// store essentiality for next round
				// it is useless to store it before minimal transverse computation
				if (!mtComputation)
				{
					(*itemset->noiseBitset) = noiseBitset | ((cumulatedXorbitset & (bitsetToAdd ^ cumulatedXorbitset)) ^ cumulatedXorbitset);
					(*itemset->cumulatedXorbitset) = xorBitset;					
					itemset->isEssential = true;
				}

				return true;				
			}
		}
	}
}

#endif



unsigned int Itemset::computeSupport(const Itemset& left, const std::shared_ptr<Itemset>& right)
{
	StaticBitset* res = new StaticBitset();
	if (itemsetType == CONSJONCTIVE)
		(*res) = (*left.supportBitset) & (*right->supportBitset);
	else
		(*res) = (*left.supportBitset) | (*right->supportBitset);
	unsigned int r = res->count();
	delete res;
	return r;
}

bool Itemset::operator==(const Itemset& other)
{
	// first test if support is different, itemsets are differents
	if ((!this->dirty && !other.dirty) && (this->supportValue != other.supportValue))
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

std::string Itemset::toString() const
{
	std::string res = "{";
	for_each(this->itemset.begin(), this->itemset.end(), [&](const Item* item) {
		res += std::to_string(item->getAttributeIndex());
		res += ",";
		});
	res.pop_back();
	res += "}";
	return res;
}

/*void Itemset::copyRightIntoLeft(Itemset& left, const std::shared_ptr<Itemset>& right)
{
	left.itemset.clear();

	for (auto it = right->itemset.begin(); it != right->itemset.end(); it++)
		left.itemset.push_back(*it);

	left.dirty = right->dirty;
	*(left.supportBitset) = *(right->supportBitset);
	left.supportValue = right->supportValue;
	left.hasClone = right->hasClone;
}*/
