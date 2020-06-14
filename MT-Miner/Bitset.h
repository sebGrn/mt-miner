#pragma once
#include <cassert>
#include <algorithm>
#include <list>

/**
* Sparse Bitset implementation
* Use sparsity property of bitsets
* Stores a vector of indexes
* an index is a reference to the ith element corresponding to set bit from the original bitset
*/ 
class SparseIndexBitset
{
private:
	unsigned int bitset_size;
	std::list<unsigned int> bitset_value;

public:
	SparseIndexBitset();
	SparseIndexBitset(unsigned int bitsetSize);
	SparseIndexBitset(const SparseIndexBitset& bitset);
	~SparseIndexBitset();

	void reset(bool b = false);
	void set(unsigned int iAttribute, bool b = true);
	bool get(unsigned int iAttribute) const;
	unsigned int size() const;
	//unsigned int count() const;
	//Bitset& bitset_or(const Bitset& b) override;
	//Bitset& bitset_and(const Bitset& b) override;
	//bool bitset_compare(const Bitset& b) const override;
	//SparseIndexBitset& operator=(const SparseIndexBitset& other);
};

