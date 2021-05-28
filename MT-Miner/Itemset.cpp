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
bool Itemset::computeIsEssential(const std::shared_ptr<Itemset>& left, const std::shared_ptr<Itemset> right)
{
	StaticBitset tmp_xor = (*left->supportBitset) ^ (*right->supportBitset);
	if (!tmp_xor.count())
	{
		// all bits from left and right are "1", this is not essential
		// we dont have here a potentiel candidate for is essential
		return false;
	}
	else
	{
		bool isEssential = false;
		// create another list to compute essentiality
		std::vector<Item*> itemsetList;
		std::copy(left->itemset.begin(), left->itemset.end(), std::back_inserter(itemsetList));
		itemsetList.push_back(*(right->itemset.end() - 1));
		
		// compute if all left itemsets is still essential after adding itemsetToCombineIt
		{
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

				// one item is not essential, we can return  
				if (!isEssential)
				{
					// this bitset is not essential, break the main loop and return false
					isEssential = false;
					break;
				}
			}
		}
		return isEssential;
	}
			


			// we know that left itemset are already essentials

			/*for (unsigned int i = BITSET_SIZE; i--; )
			{
				bool leftBit = tmp_xor[i];
				bool rightBit = (*(*itemsetToCombineIt)->staticBitset)[i];

				// first essentiality test
				if ((leftBit == false) && (rightBit == true))
				{
					isEssential = true;
					break;
				}
			}

			if(isEssential)
			{ 

				// didnt find duplicates, we can add the item at the end of the list
				combinedItemset->itemset.push_back((*itemsetToCombineIt));

				// update support
				if (itemsetType == CONSJONCTIVE)
					(*combinedItemset->supportBitset) = (*(*itemsetToCombineIt)->staticBitset) & (*combinedItemset->supportBitset);
				else
					(*combinedItemset->supportBitset) = (*(*itemsetToCombineIt)->staticBitset) | (*combinedItemset->supportBitset);

				// update clone status
				if ((*itemsetToCombineIt)->isAClone())
					combinedItemset->hasClone = true;

				return true;
			}*/
		//}


		
		
		// test essentiality on (*it_item) and left itemset
		/*unsigned int nbEssentialCandidate = 0;
		for (unsigned int i = BITSET_SIZE; i--; )
		{
			// if sum ith line of 1st itemset is '0' and ith line is '1', we may have an essential combined itemset
			// if not the combined itemset is not essential
			bool sumOfLeftItemset = (*left->supportBitset)[i];
			bool rightsupportBitset = (*(*it_item)->staticBitset)[i];
			if (((sumOfLeftItemset == false) && (rightsupportBitset == true)) == false)
			{
				return false;
			}
			//if (((sumOfLeftItemset == true) && (rightsupportBitset == false)) == true)
			//{
			//	nbEssentialCandidate++;
			//}
			//else
			//{
			//	return false;
			//}
		}*/
		//if (nbEssentialCandidate < right->getItemCount())
		//{
		//	return false;
		//}
	
		// didnt find duplicates, we can add the item at the end of the list
		/*combinedItemset->itemset.push_back((*it_item));
	
		// update support
		if (itemsetType == CONSJONCTIVE)
			(*combinedItemset->supportBitset) = (*(*it_item)->staticBitset) & (*combinedItemset->supportBitset);
		else
			(*combinedItemset->supportBitset) = (*(*it_item)->staticBitset) | (*combinedItemset->supportBitset);
	
		// update clone status
		if ((*it_item)->isAClone())
			combinedItemset->hasClone = true;*/
	
	//}
	
}






		/*
		// left itemset is essential
		// right itemset is essential
		// we can evaluate minimum adn on their supportBitsets
		bool candidate_l = false;
		bool candidate_r = false;
		for (unsigned int i = BinaryRepresentation::getObjectCount(); i--; )
		{
			if (!candidate_l && tmp_xor[i] && (*left->supportBitset)[i])
			{
				candidate_l = true;
			}
			if (!candidate_r && tmp_xor[i] && (*right->supportBitset)[i])
			{
				candidate_r = true;
			}
			if (candidate_l && candidate_r)
				return true;
		}*/

		//StaticBitset tmp_and = tmp_xor & (*right->supportBitset);
		//if (tmp_and.count())
		//{
		//	return true;
		//}		

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

void Itemset::combineRightIntoLeft(Itemset& itemset_left, const std::shared_ptr<Itemset>& itemset_right)
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
		*(itemset_left.supportBitset) = (*itemset_left.supportBitset) & (*itemset_right->supportBitset);
	else
		*(itemset_left.supportBitset) = (*itemset_left.supportBitset) | (*itemset_right->supportBitset);		
	itemset_left.supportValue = (*itemset_left.supportBitset).count();
	itemset_left.hasClone = itemset_left.hasClone | itemset_right->hasClone;
	itemset_left.dirty = false;
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
