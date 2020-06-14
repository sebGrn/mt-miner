#pragma once
#include <cassert>
#include <algorithm>
#include <list>
#include <deque>

#define BITSET_SIZE				32
#define GET_BIT(bitset, i)		(bitset >> i) & 1UL
#define SET_BIT(bitset, bit, i)	(bitset |= (bit ? 1UL : 0UL) << i)
#define COUNT_BIT(bitset)		countBit(bitset)

typedef unsigned long bitset_impl;


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

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- //

/**
 * Custom Bitset implementation
 * Use vector of unsigned long long value (64 bits) as a bitset
  */
class CustomBitset
{
private:
	// use block of 32 bits (unsigned long int) to represent a bitset
	// https://fr.cppreference.com/w/cpp/language/types
	unsigned int bitset_size;
	std::deque<unsigned long int> bitset_value;
	bool modified;
	unsigned int count_value;

	const unsigned int memory_size;

public:
	CustomBitset(unsigned int bitsetSize);
	~CustomBitset();

	void reset(bool b = false);
	void set(unsigned int iAttribute, bool b = true);
	bool get(unsigned int iAttribute) const;
	unsigned int size() const;
	unsigned int count();
	CustomBitset& operator|(const CustomBitset& other);
	CustomBitset& operator&(const CustomBitset& other);
	CustomBitset& operator=(const CustomBitset& other);
	bool operator==(const CustomBitset& other);
};

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- //

class ULBitset
{
protected:
	unsigned long bitset_value;

public:
	ULBitset(unsigned int bitsetSize)
	{
		bitset_value = 0;
	}
	~ULBitset()
	{}
	void set(unsigned int i, bool b = true)
	{
		this->bitset_value |= (b ? 1UL : 0UL) << i;
	}
	bool get(unsigned int i) const
	{
		return (this->bitset_value >> i) & 1UL;
	}
	unsigned int size() const
	{
		return 32;
	}
	unsigned int count()
	{
		unsigned int count(0);
		unsigned long int n(bitset_value);
		while (n)
		{
			n &= (n - 1);
			count++;
		}
		return count;
	}
	ULBitset& operator|(const ULBitset& other)
	{
		this->bitset_value |= other.bitset_value;
		return *this;
	}
	ULBitset& operator&(const ULBitset& other)
	{
		this->bitset_value &= other.bitset_value;
		return *this;
	}
	ULBitset& operator=(const ULBitset& other)
	{
		this->bitset_value = other.bitset_value;
		return *this;
	}
	bool operator==(const ULBitset& other)
	{
		return this->bitset_value == other.bitset_value;
	}

};



static unsigned int countBit(unsigned long bitset)
{
	unsigned int count(0);
	unsigned long int n(bitset);
	while (n)
	{
		n &= (n - 1);
		count++;
	}
	return count;
}