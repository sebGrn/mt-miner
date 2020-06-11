
#include <algorithm>

#include "BinaryRepresentation.h"
#include "Logger.h"
#include "Profiler.h"
#include <numeric>


/// build binary representation from formal context
BinaryRepresentation::BinaryRepresentation(const FormalContext& context)
{
	this->objectCount = context.getObjectCount();	// 800
	this->itemCount = context.getItemCount();		// 77
	this->nbItemsetNotAddedFromClone = 0;

	unsigned int sum = 0;
	unsigned long bitset(0);
	for (unsigned int j = 0; j < this->itemCount; j++)			// 8 on test.txt
	{
		bitset = 0;
		// allocate bitset with object count bit (formal context column size)
		for (unsigned int i = 0; i < this->objectCount; i++)		// 6 on test.txt
		{
			bool bit = context.getBit(i, j);
			bitset |= (bit ? 1UL : 0UL) << i;
			//SET_BIT(bitset, bit, i);
			if (bit)
				sum++;			
		}
		
		// set a critical section to allow multiple thread to write in size_tuples vector
		unsigned int currentKey = j + 1; 		
		this->binaryRepresentation[currentKey] = bitset;
	}

	unsigned int nbElement = this->itemCount * this->objectCount;
	double sparsity = (nbElement - sum) / static_cast<double>(nbElement);
	std::cout << RED << "sparsity " << (1.0 - sparsity) * 100.0 << "% of bits are sets" << std::endl;
};

BinaryRepresentation::~BinaryRepresentation()
{
}

// return true if element is essential
bool BinaryRepresentation::isEssential(const Itemset& itemset)
{
	if (itemset.size() == 1)
		return true;

	bool isEssential = false;
	for (int i1 = 0, n = static_cast<int>(itemset.size()); i1 != n; i1++)
	{
		unsigned long SumOfN_1Items(0);
		
		// dont forget to initialize boolean
		isEssential = false;
	
		for (int i2 = 0; i2 < n; i2++)
		{			
			if (i1 != i2)
			{
				unsigned int key2 = itemset[i2];
				unsigned long bitset = this->getBitsetFromKey(key2);
				SumOfN_1Items |= bitset;
			}
		}

		unsigned int key1 = itemset[i1];
		unsigned long bitset = this->getBitsetFromKey(key1);
		for (unsigned int i = 0; i < this->objectCount; i++)
		{
			// compare bit
			bool bit0 = GET_BIT(SumOfN_1Items, i);
			bool bit1 = GET_BIT(bitset, i);
			if (!bit0 && bit1)
			{
				// this bitset is essential, check with next bitset
				isEssential = true;
				break;
			}
		}

		if (!isEssential)
		{
			// this bitset is not essential, return false
			break;
		}
	}
	return isEssential;
}

unsigned int BinaryRepresentation::computeDisjonctifSupport(const Itemset& pattern) const
{
	unsigned long SumOfN_1Items(0);
	for (size_t i = 0, n = pattern.size(); i < n; i++)
	{
		unsigned int columnKey = pattern[i];
		unsigned long bitset = this->getBitsetFromKey(columnKey);
		SumOfN_1Items |= bitset;
	}

	unsigned int disSupp = COUNT_BIT(SumOfN_1Items);
	return disSupp;
};

bool BinaryRepresentation::compareItemsets(const Itemset& itemset1, const Itemset& itemset2) const
{
	bool sameItemset = true;
	unsigned int supp1 = computeDisjonctifSupport(itemset1);
	unsigned int supp2 = computeDisjonctifSupport(itemset2);
	if (supp1 != supp2)
		sameItemset = false;
	else
	{
		for (size_t i = 0, n = itemset1.size(); i< n; i++)
		{
			assert(i < itemset2.size());
			unsigned int columnKey_itemset1 = itemset1[i];
			unsigned int columnKey_itemset2 = itemset2[i];
			unsigned long bitset1 = this->getBitsetFromKey(columnKey_itemset1);
			unsigned long bitset2 = this->getBitsetFromKey(columnKey_itemset2);
			return bitset1 == bitset2;
		}
	}
	return sameItemset;
}

unsigned int BinaryRepresentation::buildCloneList()
{
	for (auto it1 = this->binaryRepresentation.begin(); it1 != this->binaryRepresentation.end(); it1++)
	{
		for (auto it2 = it1; it2 != this->binaryRepresentation.end(); it2++)
		{
			// check do not test the same bitset
			if (it1 != it2)
			{
				// test if binary representation bitsets are equal (it2 is a clone of it1 ?)
				if (it1->second == it2->second)
				{
					// check that original (it1->first) is not already registered as a clone (second) in clonedBitsetIndexes
					auto it_finder = find_if(clonedBitsetIndexes.begin(), clonedBitsetIndexes.end(), Utils::compare_second_value_of_pair(it1->first));
					if (it_finder == clonedBitsetIndexes.end())
					{
						// push a clone as <original index, clone index>
						clonedBitsetIndexes.push_back(std::pair<unsigned int, unsigned int>(it1->first, it2->first));
						//Logger::log("** clone detected at index <", std::to_string(it1->first), ", ", std::to_string(it2->first), "> **\n");
					}
				}
			}
		}
	}
	return static_cast<unsigned int>(clonedBitsetIndexes.size());
};

bool BinaryRepresentation::containsAClone(const Itemset& itemset)
{
	for (auto it = clonedBitsetIndexes.begin(); it != clonedBitsetIndexes.end(); it++)
	{
		// check if 
		if (std::find(itemset.begin(), itemset.end(), it->second) != itemset.end())
		{
			this->nbItemsetNotAddedFromClone++;
			return true;
		}
	}
	return false;
}

bool BinaryRepresentation::containsOriginals(const Itemset& itemset, std::vector<std::pair<unsigned int, unsigned int>>& originalClonedIndexes) const
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
