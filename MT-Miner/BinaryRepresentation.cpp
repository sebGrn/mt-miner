
#include <algorithm>
#include <numeric>

#include "BinaryRepresentation.h"
#include "Logger.h"

unsigned int BinaryRepresentation::objectCount = 0;
unsigned int BinaryRepresentation::itemCount = 0;
unsigned int BinaryRepresentation::nbItemsetNotAddedFromClone = 0;
std::unordered_map<unsigned int, std::shared_ptr<Item>> BinaryRepresentation::binaryRepresentationMap;

/// build binary representation from formal context
void BinaryRepresentation::buildFromFormalContext(const FormalContext& context)
{
	BinaryRepresentation::objectCount = context.getObjectCount();	// 800
	BinaryRepresentation::itemCount = context.getItemCount();		// 77
	BinaryRepresentation::nbItemsetNotAddedFromClone = 0;
	BinaryRepresentation::binaryRepresentationMap.clear();
	
	std::shared_ptr<Item> item;
	unsigned int sum = 0;
	for (unsigned int j = 0; j < BinaryRepresentation::itemCount; j++)			// 8 on test.txt
	{
		unsigned int currentKey = j + 1;
		item = std::make_shared<Item>(currentKey, BinaryRepresentation::objectCount);
		// allocate bitset with object count bit (formal context column size)
		for (unsigned int i = 0; i < BinaryRepresentation::objectCount; i++)		// 6 on test.txt
		{
			bool bit = context.getBit(i, j);
			item->staticBitset.set(i, bit);			
			if (bit)
			{
				//item->sparseBitset.set(i, true);
				sum++;
			}
		}

		// set a critical section to allow multiple thread to write in size_tuples vector		
		BinaryRepresentation::binaryRepresentationMap[currentKey] = item;
	}

	unsigned int nbElement = BinaryRepresentation::itemCount * BinaryRepresentation::objectCount;
	double sparsity = (nbElement - sum) / static_cast<double>(nbElement);
	Logger::log((1.0 - sparsity) * 100.0, "% of bits are sets\n");
};


// return true if element is essential
bool BinaryRepresentation::isEssential(std::shared_ptr<Itemset>& itemset)
{
	if (itemset->getItemCount() == 1)
		return true;
	
	bool isEssential = false;
	StaticBitset SumOfN_1Items;
	for (int i1 = 0, n = itemset->getItemCount(); i1 != n; i1++)
	{		
		// dont forget to initialize boolean
		SumOfN_1Items.reset();
		isEssential = false;
	
		for (int i2 = 0; i2 < n; i2++)
		{			
			if (i1 != i2)
			{
				StaticBitset bitset = itemset->getItem(i2)->staticBitset;
				if(!bitset.none())
					SumOfN_1Items = SumOfN_1Items | bitset;
			}
		}

		StaticBitset bitset = itemset->getItem(i1)->staticBitset;
		for (unsigned int i = 0; i < objectCount; i++)
		{
			// compare bit
			bool bit0 = SumOfN_1Items[i];
			bool bit1 = bitset[i];
			if (!bit0 && bit1)
			{
				// this bitset is essential, check with next bitset
				isEssential = true;
				break;
			}
		}

		if (!isEssential)
		{
			// this bitset is not essential, break the main loop and return false
			break;
		}
	}
	return isEssential;
}


unsigned int BinaryRepresentation::buildCloneList()
{
	unsigned int nbClone = 0;
	for (auto it1 = BinaryRepresentation::binaryRepresentationMap.begin(); it1 != BinaryRepresentation::binaryRepresentationMap.end(); it1++)
	{
		// check that it1 is not already a clone
		if (!it1->second->isAClone())
		{
			for (auto it2 = it1; it2 != BinaryRepresentation::binaryRepresentationMap.end(); it2++)
			{
				// check do not test the same bitset
				if (it1 != it2)
				{
					// test if bitsets have the same support
					if (it1->second->staticBitset.count() == it2->second->staticBitset.count())
					{
						// test if binary representation bitsets are equals (it2 is a clone of it1 ?)
						if ((*it1->second) == (*it2->second))
						{
							// bitset it1 is an original and bitset it2 is its clone
							// store cloned index from it2 into it1
							it1->second->addClone(it2->second);
							// set it2 as a clone
							it2->second->setClone();
							// inc nb clone
							nbClone++;
						}
					}
				}
			}
		}
	}
	return nbClone;
};

void BinaryRepresentation::serialize(const std::string& outputile)
{
	std::ofstream fileStream = std::ofstream(outputile, std::ofstream::out);
	for (auto it = binaryRepresentationMap.begin(); it != binaryRepresentationMap.end(); it++)
	{
		StaticBitset bitset = it->second->staticBitset;
		//for (int i = 0, n = bitset.size(); i < n; i++)
		for (int i = 0, n = 32; i < n; i++)
		{
			bool bit = bitset[i];
			fileStream << bit ? "1" : "0";
			fileStream << ";";
		}
		fileStream << std::endl;
	}
	fileStream.close();
};