
#include <algorithm>
#include <numeric>

#include "BinaryRepresentation.h"
#include "Logger.h"
#include "Profiler.h"

template <class T>
unsigned int BinaryRepresentation<T>::objectCount = 0;
template <class T>
unsigned int BinaryRepresentation<T>::itemCount = 0;
template <class T>
unsigned int BinaryRepresentation<T>::nbItemsetNotAddedFromClone = 0;
template <class T>
std::unordered_map<unsigned int, T> BinaryRepresentation<T>::binaryRepresentation;
//template <class T>
//std::vector<std::pair<unsigned int, unsigned int>> BinaryRepresentation<T>::clonedBitsetIndexes;

/// build binary representation from formal context
template <class T>
void BinaryRepresentation<T>::buildFromFormalContext(const FormalContext& context)
{
	objectCount = context.getObjectCount();	// 800
	itemCount = context.getItemCount();		// 77
	nbItemsetNotAddedFromClone = 0;
	binaryRepresentation.clear();

	unsigned int sum = 0;
	T bitset;
	for (unsigned int j = 0; j < itemCount; j++)			// 8 on test.txt
	{
		bitset = 0;
		// allocate bitset with object count bit (formal context column size)
		for (unsigned int i = 0; i < objectCount; i++)		// 6 on test.txt
		{
			bool bit = context.getBit(i, j);
			//bitset |= (bit ? 1UL : 0UL) << i;
			bitset.set(i, bit);
			if (bit)
				sum++;
		}

		// set a critical section to allow multiple thread to write in size_tuples vector
		unsigned int currentKey = j + 1;
		binaryRepresentation[currentKey] = bitset;
	}

	unsigned int nbElement = itemCount * objectCount;
	double sparsity = (nbElement - sum) / static_cast<double>(nbElement);
	std::cout << RED << "sparsity " << (1.0 - sparsity) * 100.0 << "% of bits are sets" << std::endl;
};

// return true if element is essential
template <class T>
bool BinaryRepresentation<T>::isEssential(Itemset& itemset)
{
	if (itemset.itemset_list.size() == 1)
		return true;

	bool isEssential = false;
	for (int i1 = 0, n = static_cast<int>(itemset.itemset_list.size()); i1 != n; i1++)
	{
		T SumOfN_1Items;
		
		// dont forget to initialize boolean
		isEssential = false;
	
		for (int i2 = 0; i2 < n; i2++)
		{			
			if (i1 != i2)
			{
				unsigned int key2 = itemset.itemset_list[i2];
				T bitset = getBitsetFromKey(key2);
				if(bitset.valid())
					SumOfN_1Items = SumOfN_1Items | bitset;
			}
		}

		unsigned int key1 = itemset.itemset_list[i1];
		T bitset = getBitsetFromKey(key1);
		for (unsigned int i = 0; i < objectCount; i++)
		{
			// compare bit
			bool bit0 = SumOfN_1Items.get(i);
			bool bit1 = bitset.get(i);
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

template <class T>
unsigned int BinaryRepresentation<T>::computeDisjonctifSupport(Itemset& pattern)
{
	// check if OR operation has already been computed for this itemset
	if (!pattern.computed)
	{
		T SumOfN_1Items;
		for (size_t i = 0, n = pattern.itemset_list.size(); i < n; i++)
		{
			unsigned int columnKey = pattern.itemset_list[i];
			T bitset = getBitsetFromKey(columnKey);
			if(bitset.valid())
				SumOfN_1Items = SumOfN_1Items | bitset;
		}
		unsigned int disSupp = SumOfN_1Items.count();
		pattern.bitset_count = disSupp;
		pattern.computed = true;
		pattern.or_value = SumOfN_1Items.data();
	}
	return pattern.bitset_count;
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

template <class T>
unsigned int BinaryRepresentation<T>::buildCloneList()
{
	unsigned int nbClone = 0;
	for (auto it1 = binaryRepresentation.begin(); it1 != binaryRepresentation.end(); it1++)
	{
		for (auto it2 = it1; it2 != binaryRepresentation.end(); it2++)
		{
			// check do not test the same bitset
			if (it1 != it2)
			{
				// test if binary representation bitsets are equal (it2 is a clone of it1 ?)
				if (it1->second == it2->second)
				{
					// check that original (it1->first) is not already registered as a clone (second) in clonedBitsetIndexes
					if (!it1->second.isAClone())
					{
						// bitset it1 is an original and bitset it2 is its clone
						// store cloned index from it2 into it1
						it1->second.setAsAnOriginal(it2->first);
						// set it2 as a clone
						it2->second.setAsAClone();
						// inc nb clone
						nbClone++;
					}
				}
			}
		}
	}
	return nbClone;
	
};

template <class T>
bool BinaryRepresentation<T>::containsAClone(const Itemset& itemset)
{
	for (auto index : itemset.itemset_list)
	{
		T bitset = getBitsetFromKey(index);
		if (bitset.isAClone())
			return true;
	}

	//for (auto it = clonedBitsetIndexes.begin(); it != clonedBitsetIndexes.end(); it++)
	//{
	//	// check if 
	//	if (std::find(itemset.itemset_list.begin(), itemset.itemset_list.end(), it->second) != itemset.itemset_list.end())
	//	{
	//		nbItemsetNotAddedFromClone++;
	//		return true;
	//	}
	//}
	return false;
}

template <class T>
bool BinaryRepresentation<T>::containsOriginals(const Itemset& itemset, std::vector<std::pair<unsigned int, unsigned int>>& originalClonedIndexes)
{
	originalClonedIndexes.clear();
	std::for_each(itemset.itemset_list.begin(), itemset.itemset_list.end(), [&originalClonedIndexes](unsigned int index) {
		T bitset = getBitsetFromKey(index);
		if (bitset.isAnOriginal())
		{
			originalClonedIndexes.push_back(std::pair<unsigned int, unsigned int>(index, bitset.getCloneIndex()));
		}
	});

	//originalClonedIndexes.clear();
	//for (auto it = clonedBitsetIndexes.begin(); it != clonedBitsetIndexes.end(); it++)
	//{
	//	// check if 
	//	if (std::find(itemset.itemset_list.begin(), itemset.itemset_list.end(), it->first) != itemset.itemset_list.end())
	//	{
	//		originalClonedIndexes.push_back(std::pair<unsigned int, unsigned int>(it->first, it->second));
	//	}
	//}
	return !originalClonedIndexes.empty();
}



// ------------------------------------------------------------------------------------------------------------------------------------------------ //

// template implementation
//template class BinaryRepresentation<unsigned long>;
template class BinaryRepresentation<ULBitset>;
