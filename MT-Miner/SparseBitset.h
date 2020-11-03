#pragma once

#include <list>
#include <deque>

/**
* Sparse Bitset implementation
* Use sparsity property of bitsets
* Stores a vector of indexes
* an index is a reference to the ith element corresponding to set bit from the original bitset
*/
class SparseBitset
{
public:
	std::list<unsigned int> bitset_value;
	//std::deque<unsigned int> bitset_value;

public:
	SparseBitset()
	{
	};

	SparseBitset(const SparseBitset& bitset)
	{
		this->bitset_value = bitset.bitset_value;
	};

	//void set(const StaticBitset& bitset)
	//{
	//	this->bitset_value.clear();
	//	for (unsigned int i = 0; i < bitset.size(); i++)
	//	{
	//		bool bit = bitset[i];
	//		if (bit)
	//			this->bitset_value.emplace_back(i);
	//	}
	//	this->bitset_value.sort();
	//};

	~SparseBitset()
	{};

	void reset();
	void set(unsigned int iAttribute);
	bool get(unsigned int iAttribute) const;
	unsigned int count() const;
	
};

inline void SparseBitset::reset()
{
	this->bitset_value.clear();
};

inline void SparseBitset::set(unsigned int iAttribute)
{
	this->bitset_value.emplace_back(iAttribute);

	//auto it = std::find(this->bitset_value.begin(), this->bitset_value.end(), iAttribute);
	//if (it == this->bitset_value.end())
	//{
	//	// didnt found the index in the array, just add it if it's true
	//	if (b)
	//		this->bitset_value.emplace_back(iAttribute);
	//}
	//else
	//{
	//	// index is preset, modify the value or remove it
	//	if (b)
	//		*it = iAttribute;
	//	else
	//		this->bitset_value.erase(it);
	//}
	//this->bitset_value.sort();
};

inline bool SparseBitset::get(unsigned int iAttribute) const
{
	auto it = std::find(this->bitset_value.begin(), this->bitset_value.end(), iAttribute);
	return (it != this->bitset_value.end());
};

inline unsigned int SparseBitset::count() const
{
	return static_cast<unsigned int>(this->bitset_value.size());
};

//SparseBitset SparseBitset::operator|(const SparseBitset& other) const
//{
//	//assert(this->bitset_size == b.bitset_size);
//	//this->bitset_value.insert(this->bitset_value.end(), a.bitset_value.begin(), a.bitset_value.end());
//	SparseBitset res;
//	res.bitset_size = other.bitset_size;
//	res.bitset_value = other.bitset_value;
//	std::list<unsigned int> test = this->bitset_value;
//	res.bitset_value.merge(test);
//	res.bitset_value.merge(this->bitset_value);
//	res.bitset_value.unique();
//	//this->bitset_value.sort();
//	return *this;
//}

//SparseBitset& SparseBitset::bitset_and(const SparseBitset& b)
//{
//	//assert(this->bitset_size == b.bitset_size);
//	const SparseBitset& a = dynamic_cast<const SparseBitset&>(b);
//	SparseBitset& c = const_cast<SparseBitset&>(a);
//	//this->bitset_value.merge(c.bitset_value);
//	// from https://stackoverflow.com/questions/10806347/remove-all-the-elements-that-occur-in-1-list-from-another-c
//	this->bitset_value.erase(std::remove_if(this->bitset_value.begin(), this->bitset_value.end(),
//		std::bind(std::equal_to<std::list<unsigned int>::iterator>(), c.bitset_value.end(),
//			std::bind(std::find<std::list<unsigned int>::iterator, unsigned int>, c.bitset_value.begin(), c.bitset_value.end(), std::placeholders::_1))),
//		this->bitset_value.end());
//	return *this;
//}

//bool SparseBitset::bitset_compare(const Bitset& b) const
//{
//	if (this->bitset_size != b.size())
//		return false;
//	const SparseBitset& a = dynamic_cast<const SparseBitset&>(b);
//	if (this->bitset_value.size() != a.bitset_value.size())
//		return false;
//	return this->bitset_value == a.bitset_value;
//}

//SparseBitset& SparseBitset::operator=(const SparseBitset& other)
//{
//	this->bitset_size = other.bitset_size;
//	this->bitset_value = other.bitset_value;
//	return *this;
//}

