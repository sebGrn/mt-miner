
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

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- //

CustomBitset::CustomBitset(unsigned int bitsetSize) : memory_size(64)
{
	this->modified = true;
	this->bitset_size = bitsetSize;
	unsigned int n = (bitsetSize / memory_size) + 1;
	this->bitset_value.resize(n);
	std::for_each(this->bitset_value.begin(), this->bitset_value.end(), [](unsigned long int& v) { v = 0ULL; });
};

//CustomBitset::CustomBitset(const CustomBitset& bitset) : memory_size(64)
//{
//	this->bitset_size = bitset.size();
//	std::copy(bitset.bitset_value.begin(), bitset.bitset_value.end(), std::back_inserter(this->bitset_value));
//};

CustomBitset::~CustomBitset()
{
}

unsigned int CustomBitset::size() const
{
	return this->bitset_size;
}

void CustomBitset::reset(bool b)
{
	this->modified = true;
	std::for_each(this->bitset_value.begin(), this->bitset_value.end(), [&b](unsigned long int& v) { v = (b ? 1ULL : 0ULL); });
};

void CustomBitset::set(unsigned int iAttribute, bool b)
{
	this->modified = true;
	unsigned int iArray = iAttribute / memory_size;
	unsigned int iBit = iAttribute % memory_size;
	assert(iArray < this->bitset_value.size());
	unsigned long int number = this->bitset_value[iArray];
	number |= (b ? 1UL : 0UL) << iBit;
	this->bitset_value[iArray] = number;
};

bool CustomBitset::get(unsigned int iAttribute) const
{
	assert(iAttribute < size());
	unsigned int iArray = iAttribute / memory_size;
	unsigned int iBit = iAttribute % memory_size;
	assert(iArray < this->bitset_value.size());
	unsigned long int number = this->bitset_value[iArray];
	bool bit = (number >> iBit) & 1UL;
	return bit;
};


unsigned int CustomBitset::count()
{
	if (this->modified)
	{
		this->count_value = 0;
		std::for_each(this->bitset_value.begin(), this->bitset_value.end(), [this](unsigned long& v) {
			// Brian Kernighan’s Algorithm
			// https://www.geeksforgeeks.org/count-set-bits-in-an-integer/
			unsigned long n(v);
			while (n)
			{
				n &= (n - 1);
				this->count_value++;
			}
		});
		this->modified = false;
	}
	return this->count_value;
};

CustomBitset& CustomBitset::operator|(const CustomBitset& other)
{
	if (other.bitset_size != this->bitset_size)
		return *this;
	std::transform(other.bitset_value.begin(), other.bitset_value.end(), this->bitset_value.begin(), this->bitset_value.begin(), [](unsigned long int v0, unsigned long int v1) { return v0 | v1; });
	return *this;
};

CustomBitset& CustomBitset::operator&(const CustomBitset& other)
{
	std::transform(other.bitset_value.begin(), other.bitset_value.end(), this->bitset_value.begin(), this->bitset_value.begin(), [](unsigned long int v0, unsigned long int v1) { return v0 & v1;	});
	return *this;
};

bool CustomBitset::operator==(const CustomBitset& other)
{ 
	if (other.bitset_size != this->bitset_size)
		return false;
	return std::equal(other.bitset_value.begin(), other.bitset_value.end(), this->bitset_value.begin());
};

// copy assignment
CustomBitset& CustomBitset::operator=(const CustomBitset& other)
{
	if (this != &other)
	{
		this->bitset_value.clear();
		this->bitset_size = other.size();
		std::copy(other.bitset_value.begin(), other.bitset_value.end(), std::back_inserter(this->bitset_value));
	}
	return *this;
}
