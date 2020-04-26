#include "BinaryRepresentation.h"
#include "Logger.h"

/// build binary representation from formal context
BinaryRepresentation::BinaryRepresentation(const FormalContext& context)
{
	this->isEssentialDuration = 0;
	this->objectCount = context.getObjectCount();
	this->itemCount = context.getItemCount();

	//#pragma omp parallel for
	for (int j = 0; j < this->itemCount; j++)			// 8 on test.txt
	{
		Bitset bitset(this->objectCount);
		
		//#pragma omp parallel for
		for (int i = 0; i < this->objectCount; i++)	// 6 on test.txt
		{
			//#pragma omp critical
			bitset[i] = context.getElement(i, j);
		}

		// set a critical section to allow multiple thread to write in size_tuples vector
		//#pragma omp critical
		{
			unsigned int currentKey = j + 1;
			this->binaryRepresentation[currentKey] = bitset;
		}
	}
};

//bool BinaryRepresentation::checkOneItem(int itemBar, const Utils::Itemset& itemsOfpattern) const
//{
//	Bitset SumOfN_1Items(this->objectCount);
//	for (auto it = itemsOfpattern.begin(); it != itemsOfpattern.end(); it++)
//	{
//		if (*it != itemBar)
//		{
//#ifdef _DEBUG
//			Bitset bitset = this->getBitset(*it);
//			for (int j = 0; j < this->objectCount; j++)
//			{
//				SumOfN_1Items[j] = SumOfN_1Items[j] || bitset[j];
//			}
//#else
//			SumOfN_1Items = SumOfN_1Items | this->getBitset(elt);
//#endif
//		}
//	}
//
//	Bitset bitset = this->getBitset(itemBar);
//	bool res = false;
//	for (int i = 0; i < this->objectCount; i++)
//	{
//		if (SumOfN_1Items[i] == false && bitset[i] == true)
//		{
//			res = true;
//			break;
//		}
//	}
//	return res;
//}

// return true if element is essential
bool BinaryRepresentation::isEssential(const Utils::Itemset& itemset)
{
	auto beginTime = std::chrono::system_clock::now();

	if (itemset.size() == 1)
		return true;

	bool isEssential = false;
	for (int i1 = 0; i1 != static_cast<int>(itemset.size()); i1++)
	{
		Bitset SumOfN_1Items(this->objectCount);
		
		// dont forget to initialize boolean
		isEssential = false;
	
		//#pragma omp parallel for
		for (int i2 = 0; i2 < static_cast<int>(itemset.size()); i2++)
		{			
			if (i1 != i2)
			{
#ifdef _DEBUG
				unsigned int key2 = itemset[i2];
				Bitset bitset = this->getBitset(key2);
				for (int j = 0; j < this->objectCount; j++)
				{
					//#pragma omp critical
					SumOfN_1Items[j] = SumOfN_1Items[j] || bitset[j];
				}
#else
				//#pragma omp critical
				SumOfN_1Items = SumOfN_1Items | this->getBitset(key2);
#endif
			}
		}

		unsigned int key1 = itemset[i1];
		Bitset bitset = this->getBitset(key1);

		//#pragma omp parallel for
		for (int i = 0; i < this->objectCount; i++)
		{
			if (SumOfN_1Items[i] == false && bitset[i] == true)
			{
				// this bitset is essential, check we next bitset
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
	
	this->isEssentialDuration += std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - beginTime).count();
	
	return isEssential;
/*
	// all bitsets from itemset are essentials
	//return isEssential;

	bool isEssential2 = true;
	for (unsigned int i = 0; i < itemset.size(); i++)
	{
		if (!checkOneItem(itemset[i], itemset))
		{
			isEssential2 = false;
			break;
		}
	}

	if (isEssential != isEssential2)
		int k = 0;
	return isEssential2;
*/
}

unsigned int BinaryRepresentation::computeDisjonctifSupport(const Utils::Itemset& pattern) const
{
	Bitset SumOfN_1Items(this->objectCount);

	//#pragma omp parallel for
	for (int i = 0; i < pattern.size(); i++)
	{
		unsigned int columnKey = pattern[i];
		Bitset bitset = this->getBitset(columnKey);
#ifdef _DEBUG
		for (int j = 0; j < this->objectCount; j++)
		{
			//#pragma omp critical
			SumOfN_1Items[j] = SumOfN_1Items[j] || bitset[j];			
		}
#else
		//#pragma omp critical
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
		//#pragma omp parallel for
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
					Logger::log("** clone detected at index <", std::to_string(it1->first), ", ", std::to_string(it2->first), "> **\n");
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
