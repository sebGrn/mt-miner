#include "Itemset.h"
#include <unordered_map>
#include "BinaryRepresentation.h"

Itemset::ItemsetType Itemset::itemsetType = Itemset::ItemsetType::DISJUNCTIVE;

Itemset::Itemset()
{
	this->value = std::make_unique<StaticBitset>();
	this->support = 0;
	this->dirty = true;
	this->hasClone = false;
}

Itemset::Itemset(Item* item)
{
	assert(this->itemset.empty());
	this->value = std::make_unique<StaticBitset>(*item->staticBitset);
	this->support = item->count();
	this->dirty = false;
	this->hasClone = false;
	if (item->isAClone())
		this->hasClone = true;

	this->itemset.push_back(item);
}

Itemset::Itemset(const std::shared_ptr<Itemset> itemset)
{
	assert(this->itemset.empty());
	this->value = std::make_unique<StaticBitset>(*itemset->value);
	this->support = itemset->support;
	this->dirty = itemset->dirty;
	this->hasClone = itemset->hasClone;
	std::copy(itemset->itemset.begin(), itemset->itemset.end(), std::back_inserter(this->itemset));	
}

Itemset::~Itemset()
{
	this->itemset.clear();
}

// make a copy of currentItemset and replance ith item by clone item
std::shared_ptr<Itemset> Itemset::createAndReplaceItem(unsigned int iToReplace, Item* itemToReplace)
{
	try
	{
		std::shared_ptr<Itemset> clonedItemset(new Itemset());
		if (clonedItemset)
		{
			(*clonedItemset->value) = (*this->value);
			clonedItemset->support = this->support;
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

			// update support
			if (itemsetType == CONSJONCTIVE)
				(*this->value) = (*(*it_item)->staticBitset) & (*this->value);
			else
				(*this->value) = (*(*it_item)->staticBitset) | (*this->value);

			// update clone status
			if ((*it_item)->isAClone())
				this->hasClone = true;
		}

		// update support
		this->support = (*this->value).count();
		// combined item set is not dirty, all values have been computed
		this->dirty = false;
	}
};

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
			if(itemsetType == CONSJONCTIVE)
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
				for (unsigned int k = 0; k < objectCount; k++)
				{
					// compare bit
					if (itemsetType == CONSJONCTIVE)
					{
						if (!bitset.test(k) && SumOfN_1Items.test(k))
						{
							// this bitset is essential, check with next bitset
							isEssential = true;
							break;
						}
					}
					else
					{
						if (bitset.test(k) && !SumOfN_1Items.test(k))
						{
							// this bitset is essential, check with next bitset
							isEssential = true;
							break;
						}
					}
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

unsigned int Itemset::computeSupport(const Itemset& left, const std::shared_ptr<Itemset> right)
{
	StaticBitset* res = new StaticBitset();
	if (itemsetType == CONSJONCTIVE)
		(*res) = (*left.value) & (*right->value);
	else
		(*res) = (*left.value) | (*right->value);
	unsigned int r = res->count();
	delete res;
	return r;
}

bool Itemset::operator==(const Itemset& other)
{
	// first test if support is different, itemsets are differents
	if ((!this->dirty && !other.dirty) && (this->support != other.support))
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
		}
	}

	// dont need to loop on all items, just combine itemset properties
	if(itemset_left.itemsetType == CONSJONCTIVE)
		*(itemset_left.value) = (*itemset_left.value) & (*itemset_right->value);
	else
		*(itemset_left.value) = (*itemset_left.value) | (*itemset_right->value);		
	itemset_left.support = (*itemset_left.value).count();
	itemset_left.hasClone = itemset_left.hasClone | itemset_right->hasClone;
	itemset_left.dirty = false;
}

void Itemset::copyRightIntoLeft(Itemset& left, const std::shared_ptr<Itemset> right)
{
	left.itemset.clear();

	for (auto it = right->itemset.begin(); it != right->itemset.end(); it++)
		left.itemset.push_back(*it);

	left.dirty = right->dirty;
	*(left.value) = *(right->value);
	left.support = right->support;
	left.hasClone = right->hasClone;
}
