
#include "Bitset.h"

SparseIndexBitset::SparseIndexBitset()
{}

SparseIndexBitset::SparseIndexBitset(unsigned int bitsetSize)
{
	// contains original size of bitset
	this->bitset_size = bitsetSize;
}

SparseIndexBitset::SparseIndexBitset(const SparseIndexBitset& bitset)
{
	this->bitset_size = bitset.bitset_size;
	this->bitset_value = bitset.bitset_value;
}

SparseIndexBitset::~SparseIndexBitset()
{}

void SparseIndexBitset::reset(bool b)
{
	this->bitset_value.clear();
	if (b)
	{
		for (unsigned int i = 0; i < this->bitset_size; i++)
			this->bitset_value.push_back(i);
	}
}

void SparseIndexBitset::set(unsigned int iAttribute, bool b)
{
	auto it = std::find(this->bitset_value.begin(), this->bitset_value.end(), iAttribute);
	if (it == this->bitset_value.end())
	{
		// didnt found the index in the array, just add it if it's true
		if (b)
			this->bitset_value.emplace_back(iAttribute);
	}
	else
	{
		// index is preset, modify the value or remove it
		if (b)
			*it = iAttribute;
		else
			this->bitset_value.erase(it);
	}
	this->bitset_value.sort();
}

bool SparseIndexBitset::get(unsigned int iAttribute) const
{
	auto it = std::find(this->bitset_value.begin(), this->bitset_value.end(), iAttribute);
	return (it != this->bitset_value.end());
}

unsigned int SparseIndexBitset::size() const
{
	return this->bitset_size;
}

/*unsigned int SparseIndexBitset::count() const
{
	return static_cast<unsigned int>(this->bitset_value.size());
}

SparseIndexBitset& SparseIndexBitset::bitset_or(const SparseIndexBitset& b)
{
	//assert(this->bitset_size == b.bitset_size);
	const SparseIndexBitset& a = dynamic_cast<const SparseIndexBitset&>(b);
	SparseIndexBitset& c = const_cast<SparseIndexBitset&>(a);
	//this->bitset_value.insert(this->bitset_value.end(), a.bitset_value.begin(), a.bitset_value.end());
	this->bitset_value.merge(c.bitset_value);
	this->bitset_value.unique();
	//this->bitset_value.sort();
	return *this;
}

SparseIndexBitset& SparseIndexBitset::bitset_and(const SparseIndexBitset& b)
{
	//assert(this->bitset_size == b.bitset_size);
	const SparseIndexBitset& a = dynamic_cast<const SparseIndexBitset&>(b);
	SparseIndexBitset& c = const_cast<SparseIndexBitset&>(a);
	//this->bitset_value.merge(c.bitset_value);
	// from https://stackoverflow.com/questions/10806347/remove-all-the-elements-that-occur-in-1-list-from-another-c
	this->bitset_value.erase(std::remove_if(this->bitset_value.begin(), this->bitset_value.end(), 
		std::bind(std::equal_to<std::list<unsigned int>::iterator>(), c.bitset_value.end(), 
			std::bind(std::find<std::list<unsigned int>::iterator, unsigned int>, c.bitset_value.begin(), c.bitset_value.end(), std::placeholders::_1))),
		this->bitset_value.end());
	return *this;
}

bool SparseIndexBitset::bitset_compare(const Bitset& b) const
{
	if (this->bitset_size != b.size())
		return false;
	const SparseIndexBitset& a = dynamic_cast<const SparseIndexBitset&>(b);
	if (this->bitset_value.size() != a.bitset_value.size())
		return false;
	return this->bitset_value == a.bitset_value;
}

SparseIndexBitset& SparseIndexBitset::operator=(const SparseIndexBitset& other)
{
	this->bitset_size = other.bitset_size;
	this->bitset_value = other.bitset_value;
	return *this;
}*/

