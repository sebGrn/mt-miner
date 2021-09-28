#include "Itemset.h"
#include <unordered_map>
#include <bit>
#include "BinaryRepresentation.h"

Itemset::Itemset()
{
	this->supportBitset = std::make_unique<StaticBitset>();
	this->isEssential = false; 
	// TODO allocate for toExplore only
	this->cumulatedXorbitset = std::make_unique<StaticBitset>();
	this->noiseBitset = std::make_unique<StaticBitset>();

	this->supportValue = 0;
	this->hasClone = false;	
}

Itemset::Itemset(unsigned int binaryRepIndex)
{
	assert(this->itemsetIndexVector.empty());
	std::shared_ptr<Item> item = BinaryRepresentation::getItemFromKey(binaryRepIndex);
	this->supportBitset = std::make_unique<StaticBitset>(*item->staticBitset);
	// TODO allocate for toExplore only
	this->cumulatedXorbitset = std::make_unique<StaticBitset>(*item->staticBitset);
	this->noiseBitset = std::make_unique<StaticBitset>();
	this->isEssential = false;
	this->supportValue = item->count();
	this->hasClone = false;
	if (item->isAClone())
		this->hasClone = true;

	this->itemsetIndexVector.push_back(binaryRepIndex);
}

Itemset::Itemset(const std::shared_ptr<Itemset>& itemset)
{
	assert(this->itemsetIndexVector.empty());
	this->supportBitset = std::make_unique<StaticBitset>(*itemset->supportBitset);
	this->isEssential = itemset->isEssential;
	// TODO allocate for toExplore only
	this->cumulatedXorbitset = std::make_unique<StaticBitset>(*itemset->cumulatedXorbitset);
	this->noiseBitset = std::make_unique<StaticBitset>(*itemset->noiseBitset);

	this->supportValue = itemset->supportValue;
	this->hasClone = itemset->hasClone;

	this->itemsetIndexVector.reserve(itemset->itemsetIndexVector.size());
	this->itemsetIndexVector = itemset->itemsetIndexVector;
}

Itemset::~Itemset()
{
	this->itemsetIndexVector.clear();
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
			this->isEssential = false;
			clonedItemset->supportValue = this->supportValue;
			//clonedItemset->dirty = this->dirty;
			clonedItemset->hasClone = this->hasClone;

			for (unsigned int i = 0; i < this->getItemCount(); i++)
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

bool Itemset::isEssentialRapid(std::shared_ptr<Itemset>& left, unsigned int itemIndexToAdd)
{
	// CARE : support of 72 is different than support of 2 when combining 71 with 72 ???

	std::shared_ptr<Item> itItemToAdd = BinaryRepresentation::getItemFromKey(itemIndexToAdd);
	
	StaticBitset combined_bitset = combined_bitset = (*itItemToAdd->staticBitset) | (*left->supportBitset);
	
	// must count bits into combined bitset, not so fast...
	unsigned int supportCombined = combined_bitset.count();
	unsigned int rightSupport = itItemToAdd->staticBitset->count();

	// si support 715 == support 71 ou support 75
	if (supportCombined == left->getSupport() || supportCombined == rightSupport)
	{
/*#ifdef _DEBUG
		{
			std::shared_ptr<Itemset> newItemset = std::make_shared<Itemset>(left);
			newItemset->combine(itemIndexToAdd);
			std::cout << left->toString() << " combined with " << itemIndexToAdd << " has same support as " << newItemset->toString() << ", no task created for this itemset" << std::endl;
		}
#endif*/
		return false;
	}

	// first test : or(left) xor or(right)
	StaticBitset tmp_xor = (*left->supportBitset) ^ (*itItemToAdd->staticBitset);
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

void Itemset::combine(unsigned int rightAttributeIndex)
{
	/// return true / false
	/// return true --> combine ok
	/// return false --> combine pas ok
	/// if (support left + right == support left) OR (support left + right == support right) RETURN FALSE
	/// IF (SUPPORT itemset == SUPPORT cumulatedXorbitset) OR (SUPPORT itemset == bitsetToAdd) RETURN FALSE

	// "1" + "2" => "12"
	// "71" + "72" => "712"
	// we can always add the last one
	std::shared_ptr<Item> rightItem = BinaryRepresentation::getItemFromKey(rightAttributeIndex);

	// update support
	(*this->supportBitset) = (*rightItem->staticBitset) | (*this->supportBitset);

	// update clone status
	if (rightItem->isAClone())
		this->hasClone = true;

	// update support
	this->supportValue = (*this->supportBitset).count();

	// finally add the last item
	this->itemsetIndexVector.push_back(rightAttributeIndex);
};


bool Itemset::computeIsEssentialParameters(const std::shared_ptr<Itemset>& itemset, StaticBitset& cumulatedXorbitset, StaticBitset& noiseBitset)
{
	for_each(itemset->itemsetIndexVector.begin(), itemset->itemsetIndexVector.end() - 1, [&cumulatedXorbitset, &noiseBitset](unsigned int index) {
		Item* item = BinaryRepresentation::getItemFromKey(index).get();
		noiseBitset = noiseBitset | ((cumulatedXorbitset & ((*item->staticBitset) ^ cumulatedXorbitset)) ^ cumulatedXorbitset);
		cumulatedXorbitset = cumulatedXorbitset ^ (*item->staticBitset);
		});
	return true;
}

// called only on toExplore 
bool Itemset::computeIsEssential(const std::shared_ptr<Itemset>& itemset, bool mtComputation)
{
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
		// compute cumulated xor for itemsetList
		StaticBitset cumulatedXorbitset;
		StaticBitset noiseBitset;
		if (!itemset->isEssential)
		{
			if (itemset->itemsetIndexVector.size() == 1)
			{
				Item* itItemToAdd = Itemset::getItem(itemset, 0);
				cumulatedXorbitset = *(itItemToAdd->staticBitset);
			}
			else
			{
				Itemset::computeIsEssentialParameters(itemset, cumulatedXorbitset, noiseBitset);
			}
		}
		else
		{
			// get bitsets from previous essential itemset
			cumulatedXorbitset = (*itemset->cumulatedXorbitset);
			noiseBitset = (*itemset->noiseBitset);
		}

		// check essentality with last item
		Item* right = Itemset::getItem(itemset, itemset->itemsetIndexVector.size() - 1);
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
			if (support_xor < itemset->itemsetIndexVector.size())
			{
				return false;
			}
			else
			{
				StaticBitset validatorBitset = noiseBitset;
				validatorBitset = validatorBitset.flip() & xorBitset;

				for (int i = 0, n = itemset->itemsetIndexVector.size(); i != n; i++)
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
					(*itemset->noiseBitset) = noiseBitset | ((cumulatedXorbitset & (bitsetToAdd ^ cumulatedXorbitset)) ^ cumulatedXorbitset);
					(*itemset->cumulatedXorbitset) = xorBitset;					
					itemset->isEssential = true;
				}

				return true;				
			}
		}
	}
}

unsigned int Itemset::computeSupport(const Itemset& left, const std::shared_ptr<Itemset>& right)
{
	StaticBitset* res = new StaticBitset();
	(*res) = (*left.supportBitset) | (*right->supportBitset);
	unsigned int r = res->count();
	delete res;
	return r;
}

bool Itemset::operator==(const Itemset& other)
{
	// first test if support is different, itemsets are differents
	//if ((!this->dirty && !other.dirty) && (this->supportValue != other.supportValue))
	if (this->supportValue != other.supportValue)
		return false;

	// test each attributeIndex of itemset
	unsigned int i = other.itemsetIndexVector[0];
	unsigned int otherAttributeIndex = BinaryRepresentation::getItemFromKey(i).get()->getAttributeIndex();

	for(auto it2 = this->itemsetIndexVector.begin(); it2 != this->itemsetIndexVector.end(); it2++)
	{
		unsigned int attributeIndex = BinaryRepresentation::getItemFromKey(*it2).get()->getAttributeIndex();
		if (otherAttributeIndex != attributeIndex)
			return false;
	}
	return true;
}

std::string Itemset::toString() const
{
	std::string res = "{";
	for_each(this->itemsetIndexVector.begin(), this->itemsetIndexVector.end(), [&](unsigned int i) {
		Item* item = BinaryRepresentation::getItemFromKey(i).get();
		res += std::to_string(item->getAttributeIndex());
		res += ",";
		});
	res.pop_back();
	res += "}";
	return res;
}

Item* Itemset::getItem(const std::shared_ptr<Itemset>& itemset, unsigned int i)
{
	assert(i < itemset->itemsetIndexVector.size());
	return BinaryRepresentation::getItemFromKey(itemset->itemsetIndexVector[i]).get();
}