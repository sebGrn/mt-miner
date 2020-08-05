#pragma once
#include <cassert>
#include <algorithm>
#include <list>
#include <deque>
#include <vector>
#include <bitset>

/**
 * Static Bitset implementation
 * Use static standard bitset 
  */
class StaticBitset
{
private:
	// value of the bitset
	std::bitset<1000> bitset_value;
	// true if this bitset is an original
	//bool isOriginal;
	// true if this bitset is a clone
	//bool isClone;
	// if this bitset is an original, store its clone index from binaryRepresentation map
	//std::vector<unsigned int> cloneIndexes;

	// static bitset size
	unsigned int bitset_size;

	// true if bitset has been modified, we need to compute count_value 
	bool modified;
	unsigned int count_value;

public:
	StaticBitset(unsigned int size) : bitset_size(size)//, isOriginal(false), isClone(false)
	{
		this->modified = true;
		this->count_value = 0;
	}

	StaticBitset()// : isOriginal(false), isClone(false)
	{
		this->modified = true;
		this->count_value = this->bitset_size = 0;
	}

	~StaticBitset() {}

	//void setAsAnOriginal(unsigned int cloneIndex)
	//{
	//	this->isOriginal = true;
	//	this->cloneIndexes.push_back(cloneIndex);
	//}

	//void setAsAClone()
	//{
	//	this->isClone = true;
	//}

	//bool isAnOriginal()
	//{
	//	return this->isOriginal;
	//}

	//unsigned int getCloneIndexesCount()
	//{
	//	return static_cast<unsigned int>(this->cloneIndexes.size());
	//}

	//unsigned int getCloneIndex(unsigned int i)
	//{
	//	return this->cloneIndexes[i];
	//}

	//bool isAClone()
	//{
	//	return this->isClone;
	//}

	void set(unsigned int i, bool b = true)
	{
		this->modified = true;
		this->bitset_value[i] = b;
	}

	bool get(unsigned int i) const
	{
		return this->bitset_value[i];
	}

	void reset()
	{
		this->bitset_value.reset();
	}

	unsigned int size() const
	{
		return bitset_size;
	}

	bool valid() const
	{
		// return true if none of the bits are set to true
		return !this->bitset_value.none();
	}

	unsigned int count()
	{
		if (this->modified)
		{
			this->count_value = static_cast<unsigned int>(this->bitset_value.count());
			this->modified = false;
		}
		return this->count_value;
	}

	StaticBitset operator|(const StaticBitset& other) const
	{
		StaticBitset res(this->bitset_size);
		res.modified = true;
		res.bitset_value = this->bitset_value | other.bitset_value;
		return res;
	}

	StaticBitset& operator=(const StaticBitset& other)
	{
		if (this != &other)
		{
			this->bitset_size = other.size();
			this->bitset_value = other.bitset_value;
			this->count_value = other.count_value;
			this->modified = other.modified;
			//std::copy(other.cloneIndexes.begin(), other.cloneIndexes.end(), std::back_inserter(this->cloneIndexes));
		}
		return *this;

	}

	bool operator==(const StaticBitset& other)
	{
		if (other.bitset_size != this->bitset_size)
			return false;
		return other.bitset_value == this->bitset_value;
	}
};