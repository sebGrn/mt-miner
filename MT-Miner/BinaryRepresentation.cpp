#include "BinaryRepresentation.h"
#include "Logger.h"
#include "Profiler.h"

/// build binary representation from formal context
BinaryRepresentation::BinaryRepresentation(const FormalContext& context)
{
	this->objectCount = context.getObjectCount();
	this->itemCount = context.getItemCount();
	this->nbItemsetNotAddedFromClone = 0;

	//#pragma omp parallel for
	for (int j = 0; j < this->itemCount; j++)			// 8 on test.txt
	{
		Bitset bitset(this->objectCount);
		
		for (int i = 0; i < this->objectCount; i++)		// 6 on test.txt
		{
			bitset[i] = context.getElement(i, j);
		}

		// set a critical section to allow multiple thread to write in size_tuples vector
		unsigned int currentKey = j + 1; 
		//#pragma omp critical
		this->binaryRepresentation[currentKey] = bitset;		
	}
};

BinaryRepresentation::~BinaryRepresentation()
{
	//std::cout << "nb out " << this->nbItemsetNotAddedFromClone << std::endl;
}


// return true if element is essential
bool BinaryRepresentation::isEssential(const Itemset& itemset)
{
	START_PROFILING(__func__)

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
				unsigned int key2 = itemset[i2];
#ifdef _DEBUG				
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
	
	END_PROFILING(__func__)
	
	return isEssential;
}

unsigned int BinaryRepresentation::computeDisjonctifSupport(const Itemset& pattern) const
{
	START_PROFILING(__func__)

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
	
	END_PROFILING(__func__)

	return disSupp;
};

bool BinaryRepresentation::compareItemsets(const Itemset& itemset1, const Itemset& itemset2) const
{
	START_PROFILING(__func__)

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
	END_PROFILING(__func__)

	return sameItemset;
}


unsigned int BinaryRepresentation::buildCloneList()
{
	START_PROFILING(__func__)
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
	END_PROFILING(__func__)

	return clonedBitsetIndexes.size();
};

bool BinaryRepresentation::containsAClone(const Itemset& itemset)
{
	START_PROFILING(__func__)
	for (auto it = clonedBitsetIndexes.begin(); it != clonedBitsetIndexes.end(); it++)
	{
		// check if 
		if (std::find(itemset.begin(), itemset.end(), it->second) != itemset.end())
		{
			END_PROFILING(__func__)
			this->nbItemsetNotAddedFromClone++;
			return true;
		}
	}
	END_PROFILING(__func__)
	return false;
}

// 
bool BinaryRepresentation::containsOriginals(const Itemset& itemset, std::vector<std::pair<unsigned int, unsigned int>>& originalClonedIndexes) const
{
	START_PROFILING(__func__)
	originalClonedIndexes.clear();
	for (auto it = clonedBitsetIndexes.begin(); it != clonedBitsetIndexes.end(); it++)
	{
		// check if 
		if (std::find(itemset.begin(), itemset.end(), it->first) != itemset.end())
		{
			originalClonedIndexes.push_back(std::pair<unsigned int, unsigned int>(it->first, it->second));
		}
	}
	END_PROFILING(__func__)
	return !originalClonedIndexes.empty();
}
