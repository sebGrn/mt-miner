
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
			item->set(i, bit);			
			if (bit)
			{
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
					if (it1->second->count() == it2->second->count())
					{
						// test if binary representation bitsets are equals (it2 is a clone of it1 ?)
						if ((*it1->second) == (*it2->second))
						{
							// bitset it1 is an original and bitset it2 is its clone
							// store cloned index from it2 into it1
							it1->second->addClone(it2->second.get());
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
		//StaticBitset* bitset = it->second->staticBitset;
		//for (int i = 0, n = bitset.size(); i < n; i++)
		for (int i = 0, n = BITSET_SIZE; i < n; i++)
		{
			bool bit = it->second->get(i);
			fileStream << bit ? "1" : "0";
			fileStream << ";";
		}
		fileStream << std::endl;
	}
	fileStream.close();
};