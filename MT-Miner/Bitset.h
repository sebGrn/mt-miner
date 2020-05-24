#pragma once
#include <boost/dynamic_bitset.hpp>
#include <bitset>
#include <cassert>

//#define ITEM_COUNT 77
//#define OBJECT_COUNT 800

#define ITEM_COUNT 77

#define OBJECT_COUNT_10		10
#define OBJECT_COUNT_100	100
#define OBJECT_COUNT_1000	1000
#define OBJECT_COUNT_4096	4096

#define OBJECT_COUNT		OBJECT_COUNT_1000

/*#ifdef _DEBUG
// use vector of bool to allow vector debugging
//typedef std::vector<bool> Bitset;
typedef std::bitset<800> Bitset;
#else
// use dynamic bitset for speedup
//typedef boost::dynamic_bitset<> Bitset;
//#define Bitset std::bitset<78>
//typedef std::bitset<800> Bitset;
#endif
*/


class Bitset
{
private:	
	unsigned int bitset_size;

	// use block of 64 bits (unsigned long long) to represent a bitset
	// https://fr.cppreference.com/w/cpp/language/types
	std::vector<unsigned long long> bitset_value;

	const unsigned int memory_size;

public:
	Bitset() : memory_size(64)
	{
		this->bitset_size = 0;
	};
		
	Bitset(unsigned int bitsetSize) : memory_size(64)
	{
		this->bitset_size = bitsetSize;
		unsigned int n = (bitsetSize / memory_size) + 1;
		this->bitset_value.resize(n);
		std::for_each(this->bitset_value.begin(), this->bitset_value.end(), [](unsigned long long& v) { v = 0ULL; });
	};

	Bitset(const Bitset& bitset) : memory_size(64)
	{
		this->bitset_size = bitset.size();
		std::copy(bitset.bitset_value.begin(), bitset.bitset_value.end(), std::back_inserter(this->bitset_value));
	};

	void reset(bool b = false)
	{
		std::for_each(this->bitset_value.begin(), this->bitset_value.end(), [&b](unsigned long long& v) { v = (b ? 1ULL : 0ULL); });
	};

	void set(unsigned int iAttribute, bool b = true)
	{
		unsigned int iArray = iAttribute / memory_size;
		unsigned int iBit = iAttribute % memory_size;
		assert(iArray < this->bitset_value.size());
		unsigned long long number = this->bitset_value[iArray];
		number |= (b ? 1ULL : 0ULL) << iBit;
		this->bitset_value[iArray] = number;
	};

	bool get(unsigned int iAttribute) const
	{
		assert(iAttribute < size());
		unsigned int iArray = iAttribute / memory_size;
		unsigned int iBit = iAttribute % memory_size;
		assert(iArray < this->bitset_value.size());
		unsigned long long number = this->bitset_value[iArray];
		bool bit = (number >> iBit) & 1ULL;
		return bit;
	};

	unsigned int size() const
	{
		return this->bitset_size;
	};

	unsigned int count() const
	{
		unsigned int count(0);
		std::for_each(this->bitset_value.begin(), this->bitset_value.end(), [&count, this](unsigned long long v) {
			// Brian Kernighan’s Algorithm
			// https://www.geeksforgeeks.org/count-set-bits-in-an-integer/
			unsigned long long n(v);
			while (n)
			{
				n &= (n - 1);
				count++;
			}
		});
		return count;
	};

	Bitset operator | (const Bitset& b)
	{
		Bitset bitset(b.size());
		std::transform(b.bitset_value.begin(), b.bitset_value.end(), this->bitset_value.begin(), bitset.bitset_value.begin(), [](unsigned long long v0, unsigned long long v1) {
			unsigned long long v(v0);
			v |= v1;
			return v;
		});
		return bitset;
	};

	Bitset operator & (const Bitset& b)
	{
		Bitset bitset(b.size());
		std::transform(b.bitset_value.begin(), b.bitset_value.end(), this->bitset_value.begin(), bitset.bitset_value.begin(), [](unsigned long long v0, unsigned long long v1) {
			unsigned long long v(v0);
			v &= v1;
			return v;
		});
		return bitset;
	};

	bool operator == (const Bitset& a)
	{
		if (a.size() != this->bitset_size)
			return false;

		unsigned int n = (this->bitset_size / memory_size) + 1;
		bool equal = true;
		for (unsigned int i = 0; i < n; i++)
		{
			if (a.bitset_value[i] != this->bitset_value[i])
				return false;
		}
		return equal;
	};

	// copy assignment
	Bitset& operator=(const Bitset& other)
	{
		if (this != &other)
		{
			this->bitset_value.clear();
			this->bitset_size = other.size();
			std::copy(other.bitset_value.begin(), other.bitset_value.end(), std::back_inserter(this->bitset_value));
		}
		return *this;
	}
};