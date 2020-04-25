#include "BinaryRepresentation.h"
#include "Logger.h"

/// build binary representation from formal context
BinaryRepresentation::BinaryRepresentation(const FormalContext& context)
{
	this->objectCount = context.getObjectCount();
	this->itemCount = context.getItemCount();

	for (unsigned int j = 0; j < this->itemCount; j++)			// 8 on test.txt
	{
		Bitset bitset(this->objectCount);
		for (unsigned int i = 0; i < this->objectCount; i++)	// 6 on test.txt
		{
			bitset[i] = context.getElement(i, j);
		}
		unsigned int currentKey = j + 1;
		this->binaryRepresentation[currentKey] = bitset;
	}
};

bool BinaryRepresentation::checkOneItem(int itemBar, const Utils::Itemset& itemsOfpattern) const
{
	Bitset SumOfN_1Items(this->objectCount);
	for (auto it = itemsOfpattern.begin(); it != itemsOfpattern.end(); it++)
	{
		if (*it != itemBar)
		{
#ifdef _DEBUG
			Bitset bitset = this->getBitset(*it);
			for (int j = 0; j < this->objectCount; j++)
			{
				SumOfN_1Items[j] = SumOfN_1Items[j] || bitset[j];
			}
#else
			SumOfN_1Items = SumOfN_1Items | this->getBitset(elt);
#endif
		}
	}

	Bitset bitset = this->getBitset(itemBar);
	bool res = false;
	for (int i = 0; i < this->objectCount; i++)
	{
		if (SumOfN_1Items[i] == false && bitset[i] == true)
		{
			res = true;
			break;
		}
	}
	return res;
}

// return true if element is essential
bool BinaryRepresentation::isEssential(const Utils::Itemset& itemset) const
{
	if (itemset.size() == 1)
		return true;

	bool result = false;
	for (auto it1 = itemset.begin(); it1 != itemset.end(); it1++)
	{
		Bitset SumOfN_1Items(this->objectCount);
		for (auto it2 = itemset.begin(); it2 != itemset.end(); it2++)
		{
			if (it1 != it2)
			{
#ifdef _DEBUG
				Bitset bitset = this->getBitset(*it2);
				for (int j = 0; j < this->objectCount; j++)
				{
					SumOfN_1Items[j] = SumOfN_1Items[j] || bitset[j];
				}
#else
				SumOfN_1Items = SumOfN_1Items | this->getBitset(elt); 
#endif
			}
		}
		Bitset bitset = this->getBitset(*it1);
		for (int i = 0; i < this->objectCount; i++)
		{
			if (SumOfN_1Items[i] == false && bitset[i] == true)
			{
				result = true;
				break;
			}
		}

		if (!result)
			break;
	}
	return result;

	/*for (unsigned int i = 0; i < itemset.size(); i++)
	{
		if (!checkOneItem(itemset[i], itemset))
			return false;
	}
	return true;*/
}

unsigned int BinaryRepresentation::computeDisjonctifSupport(const Utils::Itemset& pattern) const
{
	Bitset SumOfN_1Items(this->objectCount);
	for (int i = 0; i < pattern.size(); i++)
	{
		unsigned int columnKey = pattern[i];
		Bitset bitset = this->getBitset(columnKey);
#ifdef _DEBUG
		for (int j = 0; j < this->objectCount; j++)
		{
			SumOfN_1Items[j] = SumOfN_1Items[j] || bitset[j];
		}
#else
		SumOfN_1Items = SumOfN_1Items | getBitset(columnKey);
#endif
	}

	unsigned int disSupp = 0;
#ifdef _DEBUG
	for (int i = 0; i < this->objectCount; i++)
	{
		if (SumOfN_1Items[i] == 1)
			disSupp++;
	}
#else
	disSupp = SumOfN_1Items.count();
#endif
	return disSupp;
};

bool BinaryRepresentation::compareItemsets(const Utils::Itemset& itemset1, const Utils::Itemset& itemset2) const
{
	bool sameItemset = true;
	unsigned int supp1 = computeDisjonctifSupport(itemset1);
	unsigned int supp2 = computeDisjonctifSupport(itemset2);
	if (supp1 != supp2)
		sameItemset = false;
	else
	{
		for (int i = 0; i < itemset1.size(); i++)
		{
			unsigned int columnKey_itemset1 = itemset1[i];
			unsigned int columnKey_itemset2 = itemset2[i];

			Bitset bitset1 = this->getBitset(columnKey_itemset1);
			Bitset bitset2 = this->getBitset(columnKey_itemset2);
#ifdef _DEBUG
			Bitset result;
			for (int j = 0; j < this->objectCount; j++)
			{
				result.push_back(bitset1[j] && bitset2[j]);
			}
			sameItemset = ((result == bitset1) && (result == bitset2));
#else
			Bitset result = bitset1 & bitset2;
			sameItemset = ((result == bitset1) && (result == bitset2));
#endif			
		}
	}
	return sameItemset;
}

void BinaryRepresentation::buildCloneList()
{
	for (auto it1 = this->binaryRepresentation.begin(); it1 != this->binaryRepresentation.end(); it1++)
	{
		for (auto it2 = it1; it2 != this->binaryRepresentation.end(); it2++)
		{
			if (it1 != it2)
			{
				if (it1->second == it2->second)
				{
					// push a clone <original index, clone index>
					clonedBitsetIndexes.push_back(std::pair<unsigned int, unsigned int>(it1->first, it2->first));
					Logger::log("** add clone at index <", std::to_string(it1->first), ", ", std::to_string(it2->first), ">\n");
				}
			}
		}
	}
};

bool BinaryRepresentation::containsAClone(const Utils::Itemset& itemset) const
{
	for (auto it = clonedBitsetIndexes.begin(); it != clonedBitsetIndexes.end(); it++)
	{
		// check if 
		if (std::find(itemset.begin(), itemset.end(), it->second) != itemset.end())
		{
			return true;
		}
	}
	return false;
}

// 
bool BinaryRepresentation::containsOriginals(const Utils::Itemset& itemset, std::vector<std::pair<unsigned int, unsigned int>>& originalClonedIndexes) const
{
	originalClonedIndexes.clear();
	for (auto it = clonedBitsetIndexes.begin(); it != clonedBitsetIndexes.end(); it++)
	{
		// check if 
		if (std::find(itemset.begin(), itemset.end(), it->first) != itemset.end())
		{
			originalClonedIndexes.push_back(std::pair<unsigned int, unsigned int>(it->first, it->second));
		}
	}	
	return !originalClonedIndexes.empty();
}
