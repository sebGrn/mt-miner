
#include <algorithm>
#include <numeric>

#include "BinaryRepresentation.h"
#include "Logger.h"
#include "Profiler.h"

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
				item->sparseBitset.set(i, true);
				sum++;
			}
		}

		// set a critical section to allow multiple thread to write in size_tuples vector		
		BinaryRepresentation::binaryRepresentationMap[currentKey] = item;
	}

	unsigned int nbElement = BinaryRepresentation::itemCount * BinaryRepresentation::objectCount;
	double sparsity = (nbElement - sum) / static_cast<double>(nbElement);
	std::cout << RED << "sparsity " << (1.0 - sparsity) * 100.0 << "% of bits are sets" << std::endl;
};

// return true if element is essential
/*template <class T>
bool BinaryRepresentation<T>::isEssential(Itemset& itemset)
{
	if (itemset.is_essential_computed)
		return itemset.is_essential;
		
	if (itemset.itemset_list.size() == 1)
	{
		itemset.is_essential_computed = true;
		itemset.is_essential = true;
	}
	else
	{
		// call compute disjonctif support to get or_value of itemset
		//BinaryRepresentation<T>::computeDisjonctifSupport(itemset);

		// all bitsets have the same size
		unsigned int bitset_size = getBitsetFromKey(itemset.itemset_list[0]).size();

		// compute sparse bitset of all items from itemset
		// check if sparse index has a size of 1 and if set indexes are uniques between all sparces indexes
		// store sparse bitset of itemset
		// loop on each item 
		for (int i = 0, n = static_cast<int>(itemset.itemset_list.size()); i != n; i++)
		{
			// get bitset from binary representation
			unsigned int columnKey = itemset.itemset_list[i];
			T bitset = BinaryRepresentation<T>::getBitsetFromKey(columnKey);
			// check if bitset is not 0
			if (bitset.valid())
			{
				// convert bitset to sparse bitset and store it
				SparseIndexBitset sparseBitset(bitset);

			}
		}



		//T xor_bitset(bitset_size);
		std::vector<unsigned int> base_indexes;
		// loop on each item 
		for (int i = 0, n = static_cast<int>(itemset.itemset_list.size()); i != n; i++)
		{
			// get bitset from binary representation
			unsigned int columnKey = pattern.itemset_list[i];
			T bitset = BinaryRepresentation<T>::getBitsetFromKey(columnKey);
			// check if bitset is not 0
			if (bitset.valid())
			{
				// loop on all bit from the current bitset
				for (int j = 0; j < bitset_size; j++)
				{
					// check if current bit has not been already added
					if (std::find(base_indexes.begin(), base_indexes.end(), j) != base_indexes.end())
					{
						// get bit value
						bool base_bit = bitset.get(j);
						if (base_bit)
						{
							// if bit value is set, store its indexes 
							base_indexes.push_back(i);
							// loop on others bits from the current bitset and check that other bits are not set
							for (int k = 0; k < bitset_size; k++)
							{
								if (k != j)
								{
									bool test_bit = bitset.get(j);
									if (test_bit)
									{
										// bit is set, this transaction is a noise, go next
										break;
									}
								}
							}
						}
					}
				}
			}
		}
	}
	return itemset.is_essential;
}*/

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
				unsigned int key2 = itemset->getItem(i2)->attributeIndex;
				StaticBitset bitset = getItemFromKey(key2)->staticBitset;
				if(!bitset.none())
					SumOfN_1Items = SumOfN_1Items | bitset;
			}
		}

		unsigned int key1 = itemset->getItem(i1)->attributeIndex;
		StaticBitset bitset = getItemFromKey(key1)->staticBitset;
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

unsigned int BinaryRepresentation::computeDisjonctifSupport(std::shared_ptr<Itemset>& pattern)
{
	// check if OR operation has already been computed for this itemset
	if (pattern->dirty)
	{
		// all bitsets have the same size
		StaticBitset SumOfN_1Items;
		for (size_t i = 0, n = pattern->getItemCount(); i < n; i++)
		{
			unsigned int columnKey = pattern->getItem(i)->attributeIndex;
			StaticBitset bitset = BinaryRepresentation::getItemFromKey(columnKey)->staticBitset;
			if(!bitset.none())
				SumOfN_1Items = SumOfN_1Items | bitset;
		}
		unsigned int disSupp = SumOfN_1Items.count();
		pattern->orSupport = disSupp;
		pattern->dirty = false;
		pattern->orValue = SumOfN_1Items;
	}
	return pattern->orSupport;
};

//template <class T>
//bool BinaryRepresentation<T>::compareItemsets(Itemset& itemset1, Itemset& itemset2)
//{
//	bool sameItemset = true;
//	unsigned int supp1 = computeDisjonctifSupport(itemset1);
//	unsigned int supp2 = computeDisjonctifSupport(itemset2);
//	if (supp1 != supp2)
//		sameItemset = false;
//	else
//	{
//		for (size_t i = 0, n = itemset1.itemset_list.size(); i< n; i++)
//		{
//			assert(i < itemset2.itemset_list.size());
//			unsigned int columnKey_itemset1 = itemset1.itemset_list[i];
//			unsigned int columnKey_itemset2 = itemset2.itemset_list[i];
//			T bitset1 = getBitsetFromKey(columnKey_itemset1);
//			T bitset2 = getBitsetFromKey(columnKey_itemset2);
//			return bitset1 == bitset2;
//		}
//	}
//	return sameItemset;
//}

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

//bool BinaryRepresentation::containsAClone(const std::shared_ptr<Itemset>& itemset)
//{	
//	for (auto elt : itemset->itemset)
//	{
//		if (elt->isAClone())
//			return true;
//	}
//	return false;
//}

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