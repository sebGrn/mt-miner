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
/*class StaticBitset
{
private:
	// value of the bitset
	std::bitset<1000> bitset_value;

	// static bitset size
	unsigned int bitset_size;

	// true if bitset has been modified, we need to compute count_value 
	bool modified;
	unsigned int count_value;

public:
	StaticBitset(unsigned int size) : bitset_size(size)
	{
		this->modified = true;
		this->count_value = 0;
	}

	~StaticBitset() {}

	void set(unsigned int i, bool b = true);
	bool get(unsigned int i) const;
	void reset();
	unsigned int size() const;
	bool valid() const;
	unsigned int count();
	StaticBitset operator|(const StaticBitset& other) const;
	StaticBitset& operator=(const StaticBitset& other);
	bool operator==(const StaticBitset& other);
};

// ---------------------------------------------------------------------------------------- //
// ---------------------------------------------------------------------------------------- //

inline void StaticBitset::set(unsigned int i, bool b)
{
	this->modified = true;
	this->bitset_value[i] = b;
}

inline bool StaticBitset::get(unsigned int i) const
{
	return this->bitset_value[i];
}

inline void StaticBitset::reset()
{
	this->bitset_value.reset();
}

inline unsigned int StaticBitset::size() const
{
	return bitset_size;
}

inline bool StaticBitset::valid() const
{
	// return true if none of the bits are set to true
	return !this->bitset_value.none();
}

inline unsigned int StaticBitset::count()
{
	if (this->modified)
	{
		this->count_value = static_cast<unsigned int>(this->bitset_value.count());
		this->modified = false;
	}
	return this->count_value;
}

inline StaticBitset StaticBitset::operator|(const StaticBitset& other) const
{
	StaticBitset res(this->bitset_size);
	res.modified = true;
	res.bitset_value = this->bitset_value | other.bitset_value;
	return res;
}

inline StaticBitset& StaticBitset::operator=(const StaticBitset& other)
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

inline bool StaticBitset::operator==(const StaticBitset& other)
{
	if (other.bitset_size != this->bitset_size)
		return false;
	return other.bitset_value == this->bitset_value;
}*/