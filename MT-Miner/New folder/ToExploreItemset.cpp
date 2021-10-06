#include "ToExploreItemset.h"
#include "BinaryRepresentation.h"

ToExploreItemset::ToExploreItemset()
{
	this->supportBitset = StaticBitset();
#ifdef ISESSENTIAL_ON_TOEXPLORE
	this->isEssential = false; 
#endif	
	this->cumulatedXorbitset = StaticBitset();
	this->noiseBitset = StaticBitset();

	this->supportValue = 0;
	this->hasClone = false;	
}

ToExploreItemset::ToExploreItemset(unsigned int binaryRepIndex)
{
	assert(this->itemsetIndexVector.empty());
	std::shared_ptr<Item> item = BinaryRepresentation::getItemFromKey(binaryRepIndex);
	// update member for minimal transverse computation
	this->supportBitset = (*item->staticBitset);
	// update member for isEssential and combine computation
	this->cumulatedXorbitset = (*item->staticBitset);
	this->noiseBitset = StaticBitset();

#ifdef ISESSENTIAL_ON_TOEXPLORE
	this->isEssential = false;
#endif

	this->supportValue = item->count();
	this->hasClone = false;
	if (item->isAClone())
		this->hasClone = true;

	this->itemsetIndexVector.push_back(binaryRepIndex);
}

ToExploreItemset::ToExploreItemset(const std::shared_ptr<Itemset>& itemset)
{
	assert(this->itemsetIndexVector.empty());
	ToExploreItemset* toExploreItemset = static_cast<ToExploreItemset*>(itemset.get());
	assert(toExploreItemset);

	this->supportBitset = itemset->getSupportBitset();
#ifdef ISESSENTIAL_ON_TOEXPLORE
	this->isEssential = toExploreItemset->isEssential();
#endif
	this->cumulatedXorbitset = toExploreItemset->getCumulatedXorBitset();
	this->noiseBitset = toExploreItemset->getNoiseBitset();

	this->supportValue = toExploreItemset->supportValue;
	this->hasClone = toExploreItemset->hasClone;

	// copy list of itemset indexes from itemset
	itemset->copyItemset(this->itemsetIndexVector);
}

// make a copy of currentItemset and replace ith item by clone item
std::shared_ptr<Itemset> ToExploreItemset::createAndReplaceItem(unsigned int iToReplace, Item* itemToReplace)
{
	try
	{
		std::shared_ptr<ToExploreItemset> clonedItemset(new ToExploreItemset());
		if (clonedItemset)
		{
			clonedItemset->setSupportBitset(this->supportBitset, this->supportValue);
			clonedItemset->setNoiseBitset(this->noiseBitset);
			clonedItemset->setCumulatedXorBitset(this->cumulatedXorbitset);
			clonedItemset->setCloneValue(this->hasClone);
#ifdef ISESSENTIAL_ON_TOEXPLORE
			this->isEssential = false;
#endif

			for (unsigned int i = 0; i < this->getItemsetSize(); i++)
			{
				if (iToReplace == i)
				{
					clonedItemset->itemsetIndexVector.push_back(itemToReplace->attributeIndex);
					if (itemToReplace->isAClone())
						clonedItemset->hasClone = true;
				}
				else
				{
					clonedItemset->itemsetIndexVector.push_back(this->itemsetIndexVector[i]);
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

bool ToExploreItemset::isEssentialRapid(std::shared_ptr<Itemset>& left, std::shared_ptr<Itemset>& right)
{
	unsigned int indexItemToAdd = right->getItemsetFromIndex(right->getItemsetSize() - 1);
	Item* itItemToAdd = BinaryRepresentation::getItemFromKey(indexItemToAdd).get();
	
	StaticBitset combined_bitset;
	if (itemsetType == CONSJONCTIVE)
		combined_bitset = (*itItemToAdd->staticBitset) & left->getSupportBitset();
	else
		combined_bitset = (*itItemToAdd->staticBitset) | left->getSupportBitset();
	
	unsigned int supportCombined = combined_bitset.count();

	// si support 715 == support 71 ou support 75
	if (supportCombined == left->getSupport() || supportCombined == right->getSupport())
	{
#ifdef _DEBUG
		{
			std::shared_ptr<Itemset> newItemset = std::make_shared<Itemset>(left);
			newItemset->combineItemset(right.get());
			std::cout << left->toString() << " combined with " << right->toString() << " has same support as " << newItemset->toString() << ", no task created for this itemset" << std::endl;
		}
#endif // _DEBUG
		return false;
	}

	// first test : or(left) xor or(right)
	StaticBitset tmp_xor = left->getSupportBitset() ^ right->getSupportBitset();
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

void ToExploreItemset::combineItemset(const std::shared_ptr<Itemset>& itemset_right)
{
	/// return true / false
	/// return true --> combine ok
	/// return false --> combine pas ok
	/// if (support left + right == support left) OR (support left + right == support right) RETURN FALSE
	/// IF (SUPPORT itemset == SUPPORT cumulatedXorbitset) OR (SUPPORT itemset == bitsetToAdd) RETURN FALSE

	// "1" + "2" => "12"
	// "71" + "72" => "712"
	// we can always add the last one
	unsigned int lastIndex = itemset_right->getItemsetSize() - 1;
	unsigned int indexItemToAdd = itemset_right->getItemsetFromIndex(lastIndex);
	//Item* itItemToAdd = BinaryRepresentation::getItemFromKey(indexItemToAdd).get();
	Item* itItemToAdd = Itemset::getItem(itemset_right, indexItemToAdd);

	// update support
	if (itemsetType == CONSJONCTIVE)	
		this->supportBitset = (*itItemToAdd->staticBitset) & this->getSupportBitset();
	else
		this->supportBitset = (*itItemToAdd->staticBitset) | this->getSupportBitset();

#ifndef ISESSENTIAL_ON_TOEXPLORE
	(*this->noiseBitset) = (*this->noiseBitset) | ((*this->cumulatedXorbitset) & (*itItemToAdd->staticBitset) ^ (*this->cumulatedXorbitset) ^ (*this->cumulatedXorbitset));
	(*this->cumulatedXorbitset) = (*itItemToAdd->staticBitset) ^ (*this->cumulatedXorbitset);
#endif

	// update clone status
	if (itItemToAdd->isAClone())
		this->hasClone = true;

	// update support
	this->supportValue = this->supportBitset.count();

	// finally add the last item
	this->itemsetIndexVector.push_back(indexItemToAdd);
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
	unsigned int indexItemToAdd = right->itemsetIndexVector[right->itemsetIndexVector.size() - 1];
	Item* itItemToAdd = BinaryRepresentation::getItemFromKey(indexItemToAdd).get();

	StaticBitset bitsetToAdd = (*itItemToAdd->staticBitset);
	{
		StaticBitset xorBitset = (*left->cumulatedXorbitset) ^ bitsetToAdd;
		unsigned int support_xor = xorBitset.count();
		if (support_xor < (left->getItemsetSize() + 1))
		{
			return false;
		}
		else
		{
			// add item on left list then pop it (avoid to create another list)
			left->itemsetIndexVector.push_back(indexItemToAdd);

			StaticBitset validatorBitset = (*left->noiseBitset);
			validatorBitset = validatorBitset.flip() & xorBitset;

			for (int i = 0, n = left->itemsetIndexVector.size(); i != n; i++)
			{
				unsigned int key = left->itemsetIndexVector[i];
				Item* item = BinaryRepresentation::getItemFromKey(key).get();

				StaticBitset res = (*item->staticBitset) & validatorBitset;
				if (res.none())
				{
					left->itemsetIndexVector.pop_back();
					return false;
				}
			}
			left->itemsetIndexVector.pop_back();

			return true;
		}
	}
}

#else

bool ToExploreItemset::computeIsEssentialParameters(const std::shared_ptr<Itemset>& itemset, StaticBitset& cumulatedXorbitset, StaticBitset& noiseBitset)
{
	//for_each(itemset->itemsetIndexVector.begin(), itemset->itemsetIndexVector.end() - 1, [&cumulatedXorbitset, &noiseBitset](unsigned int index) {
	for(unsigned int i = 0, n = itemset->getItemsetSize(); i < n; i++) 
	{
		unsigned int index = itemset->getItemsetFromIndex(i);
		Item* item = BinaryRepresentation::getItemFromKey(index).get();
		noiseBitset = noiseBitset | ((cumulatedXorbitset & ((*item->staticBitset) ^ cumulatedXorbitset)) ^ cumulatedXorbitset);
		cumulatedXorbitset = cumulatedXorbitset ^ (*item->staticBitset);
	}
	return true;
}

// called only on toExplore 
bool ToExploreItemset::computeIsEssential(const std::shared_ptr<Itemset>& itemset, bool mtComputation)
{
	/// if (support left + right == support left) OR (support left + right == support right) RETURN FALSE
	/// IF (SUPPORT itemset == SUPPORT cumulatedXorbitset) OR (SUPPORT itemset == bitsetToAdd) RETURN FALSE

	if (itemset->getItemsetSize() == 0)
	{
		return false;
	}
	else if (itemset->getItemsetSize() == 1)
	{
		return true;
	}
	else
	{
		ToExploreItemset* toExploreItemset = dynamic_cast<ToExploreItemset*>(itemset.get());
		assert(toExploreItemset);
		
		// compute cumulated xor for itemsetList
		StaticBitset cumulatedXorbitset;
		StaticBitset noiseBitset;
		if (!toExploreItemset->isEssential())
		{
			if (toExploreItemset->itemsetIndexVector.size() == 1)
			{
				Item* itItemToAdd = Itemset::getItem(itemset, 0);
				cumulatedXorbitset = *(itItemToAdd->staticBitset);
			}
			else
			{
				ToExploreItemset::computeIsEssentialParameters(itemset, cumulatedXorbitset, noiseBitset);
			}
		}
		else
		{
			// get bitsets from previous essential itemset
			cumulatedXorbitset = toExploreItemset->getCumulatedXorBitset();
			noiseBitset = toExploreItemset->getNoiseBitset();
		}

		// check essentality with last item
		Item* right = Itemset::getItem(itemset, itemset->getItemsetSize() - 1);
		StaticBitset bitsetToAdd = (*right->staticBitset);
		{
			StaticBitset xorBitset = cumulatedXorbitset ^ bitsetToAdd;
			if (xorBitset.none())
			{
				// all bits from left and right are "1", this is not essential
				// we dont have here a potentiel candidate for is essential
				return false;
			}

			unsigned int support_xor = xorBitset.count();
			if (support_xor < itemset->getItemsetSize())
			{
				return false;
			}
			else
			{
				StaticBitset validatorBitset = noiseBitset;
				validatorBitset = validatorBitset.flip() & xorBitset;

				for (int i = 0, n = itemset->getItemsetSize(); i != n; i++)
				{
					Item* item = Itemset::getItem(itemset, i);
					StaticBitset res = (*item->staticBitset) & validatorBitset;
					if (res.none())
						return false;
				}

				// itemset is Essential
				// store essentiality for next round
				// it is useless to store it before minimal transverse computation
				if (!mtComputation)
				{
					toExploreItemset->setNoiseBitset(noiseBitset | ((cumulatedXorbitset & (bitsetToAdd ^ cumulatedXorbitset)) ^ cumulatedXorbitset));
					toExploreItemset->setCumulatedXorBitset(xorBitset);
					toExploreItemset->setEssentiality(true);
				}

				return true;				
			}
		}
	}
}
#endif
