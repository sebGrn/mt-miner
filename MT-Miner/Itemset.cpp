#include "Itemset.h"
#include <unordered_map>
#include <bit>
#include "BinaryRepresentation.h"

Itemset::ItemsetType Itemset::itemsetType = Itemset::ItemsetType::DISJUNCTIVE;

Itemset::Itemset()
{
	this->supportBitset = std::make_unique<StaticBitset>();
	this->supportValue = 0;
	this->dirty = true;
	this->hasClone = false;
}

Itemset::Itemset(Item* item)
{
	assert(this->itemset.empty());
	this->supportBitset = std::make_unique<StaticBitset>(*item->staticBitset);
	this->supportValue = item->count();
	this->dirty = false;
	this->hasClone = false;
	if (item->isAClone())
		this->hasClone = true;

	this->itemset.push_back(item);
}

Itemset::Itemset(const std::shared_ptr<Itemset> itemset)
{
	assert(this->itemset.empty());
	this->supportBitset = std::make_unique<StaticBitset>(*itemset->supportBitset);
	this->supportValue = itemset->supportValue;
	this->dirty = itemset->dirty;
	this->hasClone = itemset->hasClone;
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

void Itemset::combineItemset(const Itemset* itemset_right)
{
	// "1" + "2" => "12"
	// "71" + "72" => "712"
	// we can always add the last one
	auto it_item = itemset_right->itemset.end() - 1;
	this->itemset.push_back(*it_item);

	// update support
	if (itemsetType == CONSJONCTIVE)
		(*this->supportBitset) = (*(*it_item)->staticBitset) & (*this->supportBitset);
	else
		(*this->supportBitset) = (*(*it_item)->staticBitset) | (*this->supportBitset);

	// update clone status
	if ((*it_item)->isAClone())
		this->hasClone = true;

	// update support
	this->supportValue = (*this->supportBitset).count();
	// combined item set is not dirty, all supportBitsets have been computed
	this->dirty = false;
};

/*
bool Itemset::computeIsEssential()
{
	/// d'abord tester le XOR sur la combinaison des itemset , si différent de 1, on ne teste pas la suite
	/// isEssential, utiliser les anciens itemset
	if (this->getItemCount() == 1)
	{
		return true;
	}
	else
	{
		bool isEssential = false;
		StaticBitset SumOfN_1Items;
		for (int i = 0, n = this->getItemCount(); i != n; i++)
		{
			// dont forget to initialize boolean
			if (itemsetType == CONSJONCTIVE)
				SumOfN_1Items.set();
			else
				SumOfN_1Items.reset();
			isEssential = false;

			for (int j = 0; j < n; j++)
			{
				if (i != j)
				{
					//StaticBitset bitset = *BinaryRepresentation::getItemFromKey(j)->staticBitset;
					StaticBitset bitset = *this->itemset[j]->staticBitset;
					if (itemsetType == CONSJONCTIVE)
					{
						if (!bitset.none())
							SumOfN_1Items = SumOfN_1Items & bitset;
					}
					else
					{
						if (!bitset.none())
							SumOfN_1Items = SumOfN_1Items | bitset;
					}
				}
			}

			//StaticBitset bitset_tmp = *BinaryRepresentation::getItemFromKey(i)->staticBitset;
			StaticBitset bitset = *this->itemset[i]->staticBitset;

			if (bitset.count())
			{
				if (itemsetType == CONSJONCTIVE)
				{
					StaticBitset res = bitset ^ SumOfN_1Items;
					if (res.count())
					{
						StaticBitset res2 = SumOfN_1Items & res;
						if (res2.count())
						{
							isEssential = true;
						}
					}
				}
				else
				{
					// disjonctive (OR)
					//bool b1 = false;
					StaticBitset res = bitset ^ SumOfN_1Items;					
					if (res.count())
					{
						StaticBitset res2 = bitset & res;
						if (res2.count())
						{
							isEssential = true;
						}
					}
				}
			}

			// one item is not essential, we can return  
			if (!isEssential)
			{
				// this bitset is not essential, break the main loop and return false
				return false;
			}
		}
		return isEssential;
	}
}
*/
// itemset is essential if and only if we have this pattern
// 0 1
// 1 0
// or
// 1 0 0
// 0 1 0
// 0 0 1
bool Itemset::computeIsEssential(const std::shared_ptr<Itemset>& left, const std::shared_ptr<Itemset>& right)
{
	// first test : or(left) xor or(right)
	{
		StaticBitset tmp_xor = (*left->supportBitset) ^ (*right->supportBitset);
		if (!tmp_xor.count())
		{
			// all bits from left and right are "1", this is not essential
			// we dont have here a potentiel candidate for is essential
			//std::cout << "OPTIMIZED XOR 1" << std::endl;
			return false;
		}
	}

	auto it_item = right->itemset.end() - 1;

	// second test support of or(left) xor right
	{		
		// if support < left.size() + 1, combined itemset wont be essential
		StaticBitset res = (*left->supportBitset) ^ (*(*it_item)->staticBitset);
		unsigned int support_res = res.count();
		if (support_res < (left->getItemCount() + 1))
		{
			//std::cout << "OPTIMIZED XOR 2" << std::endl;
			return false;
		}
		else if (support_res == (left->getItemCount() + 1))
		{
			//std::cout << "OPTIMIZED XOR 3" << std::endl;
			return true;
		}
	}

	// compute if all left itemsets is still essential after adding itemsetToCombineIt
	{
		bool isEssential = false;

		// create another list to compute essentiality
		std::vector<Item*> itemsetList;
		std::copy(left->itemset.begin(), left->itemset.end(), std::back_inserter(itemsetList));
		itemsetList.push_back(*it_item);

		// compute essentiality 
		StaticBitset SumOfN_1Items;
		for (int i = 0, n = itemsetList.size(); i != n; i++)
		{
			// dont forget to initialize boolean
			if (itemsetType == CONSJONCTIVE)
				SumOfN_1Items.set();
			else
				SumOfN_1Items.reset();
			isEssential = false;

			for (int j = 0; j < n; j++)
			{
				if (i != j)
				{
					//StaticBitset bitset = *BinaryRepresentation::getItemFromKey(j)->staticBitset;
					StaticBitset bitset = *itemsetList[j]->staticBitset;
					if (itemsetType == CONSJONCTIVE)
					{
						if (!bitset.none())
							SumOfN_1Items = SumOfN_1Items & bitset;
					}
					else
					{
						if (!bitset.none())
							SumOfN_1Items = SumOfN_1Items | bitset;
					}
				}
			}

			//StaticBitset bitset_tmp = *BinaryRepresentation::getItemFromKey(i)->staticBitset;
			StaticBitset bitset = *itemsetList[i]->staticBitset;
			if (bitset.count())
			{
				if (itemsetType == CONSJONCTIVE)
				{
					StaticBitset res = bitset ^ SumOfN_1Items;
					if (res.count())
					{
						StaticBitset res2 = SumOfN_1Items & res;
						if (res2.count())
						{
							isEssential = true;
						}
					}
				}
				else
				{
					// disjonctive (OR)
					//bool b1 = false;
					StaticBitset res = bitset ^ SumOfN_1Items;
					if (res.count())
					{
						StaticBitset res2 = bitset & res;
						if (res2.count())
						{
							isEssential = true;
						}
					}
				}
			}

			// one combined item is not essential, we can return  
			if (!isEssential)
			{
				// this bitset is not essential, break the main loop and return false
				return false;
			}
		}
		return isEssential;
	}
}

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

void Itemset::writeToBinaryFile(std::ofstream& output)
{
	//std::cout << "writing into " << this->toString() << std::endl;
	for_each(this->itemset.begin(), this->itemset.end(), [&output] (Item* item) {
		//std::string tmp = item->staticBitset->to_string();
		output << item->staticBitset->to_string() << ",";
	});
}

void Itemset::readFromBinaryFile(std::ifstream& input)
{
	unsigned long n;
	input.read(reinterpret_cast<char*>(&n), sizeof(n));
	StaticBitset bitset = n;
	int k = 0;
}

void Itemset::copyRightIntoLeft(Itemset& left, const std::shared_ptr<Itemset>& right)
{
	left.itemset.clear();

	for (auto it = right->itemset.begin(); it != right->itemset.end(); it++)
		left.itemset.push_back(*it);

	left.dirty = right->dirty;
	*(left.supportBitset) = *(right->supportBitset);
	left.supportValue = right->supportValue;
	left.hasClone = right->hasClone;
}
