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

//typedef std::bitset<800> Static800Bitset;

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
#define SIZE_0	100
#define SIZE_1	1000
#define SIZE_2	2500
#define SIZE_3	5000
#define SIZE_4	10000
#define SIZE_5	25000
#define SIZE_6	50000

//#define MAP_BITSET(i)  std::bitset<SIZE_#i#>
class Static800Bitset 
{
private:
	std::bitset<800> bitset_value;

public:
	Static800Bitset() : bitset_value()
	{
		
	};

	Static800Bitset(unsigned int bitsetSize) : bitset_value(bitsetSize)
	{};

	~Static800Bitset()
	{};

	void reset(bool b = false)
	{
		this->bitset_value.reset(b);
	}

	bool get(unsigned int iAttribute) const
	{
		return this->bitset_value[iAttribute];
	}
};

class VariantBitset
{

private:
	unsigned int bitset_size;
	
	const std::vector<unsigned int> variant_size = { SIZE_0, SIZE_1, SIZE_2, SIZE_3, SIZE_4, SIZE_5, SIZE_6 };
	
	std::variant<std::bitset<SIZE_0>, std::bitset<SIZE_1>, std::bitset<SIZE_2>, std::bitset<SIZE_3>, std::bitset<SIZE_4>, std::bitset<SIZE_5>, std::bitset<SIZE_6>> bitset_value;

public:
	VariantBitset()
	{
		this->bitset_size = 0;
	};

	VariantBitset(unsigned int bitsetSize)
	{
		this->bitset_size = bitsetSize;

		//for(int i = 0; i < variant_size.size(); i++)
		//	if (this->bitset_size < variant_size[i])
		//		this->bitset_value = MAP_BITSET(i)();

		if (this->bitset_size < variant_size[0])			this->bitset_value = std::bitset<SIZE_0>();
		else if (this->bitset_size < variant_size[1])		this->bitset_value = std::bitset<SIZE_1>();
		else if (this->bitset_size < variant_size[2])		this->bitset_value = std::bitset<SIZE_2>();
		else if (this->bitset_size < variant_size[3])		this->bitset_value = std::bitset<SIZE_3>();
		else if (this->bitset_size < variant_size[4])		this->bitset_value = std::bitset<SIZE_4>();
		else if (this->bitset_size < variant_size[5])		this->bitset_value = std::bitset<SIZE_5>();
		else if (this->bitset_size < variant_size[6])		this->bitset_value = std::bitset<SIZE_6>();
	};

	~VariantBitset()
	{};

	void reset(bool b = false)
	{
		if (this->bitset_size < variant_size[0])			this->bitset_value = std::get<std::bitset<SIZE_0>>(this->bitset_value).reset();
		else if (this->bitset_size < variant_size[1])		this->bitset_value = std::get<std::bitset<SIZE_1>>(this->bitset_value).reset();
		else if (this->bitset_size < variant_size[2])		this->bitset_value = std::get<std::bitset<SIZE_2>>(this->bitset_value).reset();
		else if (this->bitset_size < variant_size[3])		this->bitset_value = std::get<std::bitset<SIZE_3>>(this->bitset_value).reset();
		else if (this->bitset_size < variant_size[4])		this->bitset_value = std::get<std::bitset<SIZE_4>>(this->bitset_value).reset();
		else if (this->bitset_size < variant_size[5])		this->bitset_value = std::get<std::bitset<SIZE_5>>(this->bitset_value).reset();
		else if (this->bitset_size < variant_size[6])		this->bitset_value = std::get<std::bitset<SIZE_6>>(this->bitset_value).reset();
	};

	void set(unsigned int iAttribute, bool b = true)
	{
		if (this->bitset_size < variant_size[0])			this->bitset_value = std::get<std::bitset<SIZE_0>>(this->bitset_value).set(iAttribute, b);
		else if (this->bitset_size < variant_size[1])		this->bitset_value = std::get<std::bitset<SIZE_1>>(this->bitset_value).set(iAttribute, b);
		else if (this->bitset_size < variant_size[2])		this->bitset_value = std::get<std::bitset<SIZE_2>>(this->bitset_value).set(iAttribute, b);
		else if (this->bitset_size < variant_size[3])		this->bitset_value = std::get<std::bitset<SIZE_3>>(this->bitset_value).set(iAttribute, b);
		else if (this->bitset_size < variant_size[4])		this->bitset_value = std::get<std::bitset<SIZE_4>>(this->bitset_value).set(iAttribute, b);
		else if (this->bitset_size < variant_size[5])		this->bitset_value = std::get<std::bitset<SIZE_5>>(this->bitset_value).set(iAttribute, b);
		else if (this->bitset_size < variant_size[6])		this->bitset_value = std::get<std::bitset<SIZE_6>>(this->bitset_value).set(iAttribute, b);
	}

	bool get(unsigned int iAttribute) const
	{
		if (this->bitset_size < variant_size[0])			return std::get<std::bitset<SIZE_0>>(this->bitset_value)[iAttribute];
		else if (this->bitset_size < variant_size[1])		return std::get<std::bitset<SIZE_1>>(this->bitset_value)[iAttribute];
		else if (this->bitset_size < variant_size[2])		return std::get<std::bitset<SIZE_2>>(this->bitset_value)[iAttribute];
		else if (this->bitset_size < variant_size[3])		return std::get<std::bitset<SIZE_3>>(this->bitset_value)[iAttribute];
		else if (this->bitset_size < variant_size[4])		return std::get<std::bitset<SIZE_4>>(this->bitset_value)[iAttribute];
		else if (this->bitset_size < variant_size[5])		return std::get<std::bitset<SIZE_5>>(this->bitset_value)[iAttribute];
		else if (this->bitset_size < variant_size[6])		return std::get<std::bitset<SIZE_6>>(this->bitset_value)[iAttribute];
		return false;
	}

	unsigned int size() const
	{
		return this->bitset_size;
	};

	unsigned int count() const
	{
		if (this->bitset_size < variant_size[0])		{ std::bitset<SIZE_0> x = std::get<std::bitset<SIZE_0>>(this->bitset_value); return static_cast<unsigned int>(x.count()); }
		else if (this->bitset_size < variant_size[1])	{ std::bitset<SIZE_1> x = std::get<std::bitset<SIZE_1>>(this->bitset_value); return static_cast<unsigned int>(x.count()); }
		else if (this->bitset_size < variant_size[2])	{ std::bitset<SIZE_2> x = std::get<std::bitset<SIZE_2>>(this->bitset_value); return static_cast<unsigned int>(x.count()); }
		else if (this->bitset_size < variant_size[3])	{ std::bitset<SIZE_3> x = std::get<std::bitset<SIZE_3>>(this->bitset_value); return static_cast<unsigned int>(x.count()); }
		else if (this->bitset_size < variant_size[4])	{ std::bitset<SIZE_4> x = std::get<std::bitset<SIZE_4>>(this->bitset_value); return static_cast<unsigned int>(x.count()); }
		else if (this->bitset_size < variant_size[5])	{ std::bitset<SIZE_5> x = std::get<std::bitset<SIZE_5>>(this->bitset_value); return static_cast<unsigned int>(x.count()); }
		else if (this->bitset_size < variant_size[6])	{ std::bitset<SIZE_6> x = std::get<std::bitset<SIZE_6>>(this->bitset_value); return static_cast<unsigned int>(x.count()); }
		return 0;
	}

	VariantBitset operator | (const VariantBitset& b)
	{
		if (this->bitset_size < variant_size[0])	  { std::bitset<SIZE_0> x = std::get<std::bitset<SIZE_0>>(b.bitset_value); x |= std::get<std::bitset<SIZE_0>>(this->bitset_value); VariantBitset res(b.bitset_size); res.bitset_value = x; return res;	}
		else if (this->bitset_size < variant_size[1]) 
		{
			std::bitset<SIZE_1> x = std::get<std::bitset<SIZE_1>>(b.bitset_value); 
			x |= std::get<std::bitset<SIZE_1>>(this->bitset_value); 			
			VariantBitset res(b.bitset_size); 
			res.bitset_value = x; 
			return res; 
		}
		else if (this->bitset_size < variant_size[2]) { std::bitset<SIZE_2> x = std::get<std::bitset<SIZE_2>>(b.bitset_value); x |= std::get<std::bitset<SIZE_2>>(this->bitset_value); VariantBitset res(b.bitset_size); res.bitset_value = x; return res; }
		else if (this->bitset_size < variant_size[3]) { std::bitset<SIZE_3> x = std::get<std::bitset<SIZE_3>>(b.bitset_value); x |= std::get<std::bitset<SIZE_3>>(this->bitset_value); VariantBitset res(b.bitset_size); res.bitset_value = x; return res; }
		else if (this->bitset_size < variant_size[4]) { std::bitset<SIZE_4> x = std::get<std::bitset<SIZE_4>>(b.bitset_value); x |= std::get<std::bitset<SIZE_4>>(this->bitset_value); VariantBitset res(b.bitset_size); res.bitset_value = x; return res; }
		else if (this->bitset_size < variant_size[5]) { std::bitset<SIZE_5> x = std::get<std::bitset<SIZE_5>>(b.bitset_value); x |= std::get<std::bitset<SIZE_5>>(this->bitset_value); VariantBitset res(b.bitset_size); res.bitset_value = x; return res; }
		else if (this->bitset_size < variant_size[6]) { std::bitset<SIZE_6> x = std::get<std::bitset<SIZE_6>>(b.bitset_value); x |= std::get<std::bitset<SIZE_6>>(this->bitset_value); VariantBitset res(b.bitset_size); res.bitset_value = x; return res; }
		return *this;
	};

	VariantBitset operator & (const VariantBitset& b)
	{
		//this->bitset_value = this->bitset_value & b.bitset_value;
		if (this->bitset_size < variant_size[0])	  { std::bitset<SIZE_0> x = std::get<std::bitset<SIZE_0>>(b.bitset_value); x &= std::get<std::bitset<SIZE_0>>(this->bitset_value); VariantBitset res(b.bitset_size); res.bitset_value = x; return res;	}
		else if (this->bitset_size < variant_size[1]) { std::bitset<SIZE_1> x = std::get<std::bitset<SIZE_1>>(b.bitset_value); x &= std::get<std::bitset<SIZE_1>>(this->bitset_value); VariantBitset res(b.bitset_size); res.bitset_value = x; return res; }
		else if (this->bitset_size < variant_size[2]) { std::bitset<SIZE_2> x = std::get<std::bitset<SIZE_2>>(b.bitset_value); x &= std::get<std::bitset<SIZE_2>>(this->bitset_value); VariantBitset res(b.bitset_size); res.bitset_value = x; return res; }
		else if (this->bitset_size < variant_size[3]) { std::bitset<SIZE_3> x = std::get<std::bitset<SIZE_3>>(b.bitset_value); x &= std::get<std::bitset<SIZE_3>>(this->bitset_value); VariantBitset res(b.bitset_size); res.bitset_value = x; return res; }
		else if (this->bitset_size < variant_size[4]) { std::bitset<SIZE_4> x = std::get<std::bitset<SIZE_4>>(b.bitset_value); x &= std::get<std::bitset<SIZE_4>>(this->bitset_value); VariantBitset res(b.bitset_size); res.bitset_value = x; return res; }
		else if (this->bitset_size < variant_size[5]) { std::bitset<SIZE_5> x = std::get<std::bitset<SIZE_5>>(b.bitset_value); x &= std::get<std::bitset<SIZE_5>>(this->bitset_value); VariantBitset res(b.bitset_size); res.bitset_value = x; return res; }
		else if (this->bitset_size < variant_size[6]) { std::bitset<SIZE_6> x = std::get<std::bitset<SIZE_6>>(b.bitset_value); x &= std::get<std::bitset<SIZE_6>>(this->bitset_value); VariantBitset res(b.bitset_size); res.bitset_value = x; return res; }
		return *this;
	};

	bool operator == (const VariantBitset& a)
	{
		return a.bitset_value == this->bitset_value;
	};

	// copy assignment
	VariantBitset& operator=(const VariantBitset& a)
	{
		if (this != &a)
		{
			this->bitset_size = a.bitset_size;
			this->bitset_value = a.bitset_value;
		}
		return *this;
	}
};

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- //
// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- //
// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- //

class CustomBitset
{
private:	
	unsigned int bitset_size;

	// use block of 64 bits (unsigned long long) to represent a bitset
	// https://fr.cppreference.com/w/cpp/language/types
	std::vector<unsigned long long> bitset_value;

	const unsigned int memory_size;

public:
	CustomBitset() : memory_size(64)
	{
		this->bitset_size = 0;
	};
		
	CustomBitset(unsigned int bitsetSize) : memory_size(64)
	{
		this->bitset_size = bitsetSize;
		unsigned int n = (bitsetSize / memory_size) + 1;
		this->bitset_value.resize(n);
		std::for_each(this->bitset_value.begin(), this->bitset_value.end(), [](unsigned long long& v) { v = 0ULL; });
	};

	CustomBitset(const CustomBitset& bitset) : memory_size(64)
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

	CustomBitset operator | (const CustomBitset& b)
	{
		CustomBitset bitset(b.size());
		std::transform(b.bitset_value.begin(), b.bitset_value.end(), this->bitset_value.begin(), bitset.bitset_value.begin(), [](unsigned long long v0, unsigned long long v1) {
			unsigned long long v(v0);
			v |= v1;
			return v;
		});
		return bitset;
	};

	CustomBitset operator & (const CustomBitset& b)
	{
		CustomBitset bitset(b.size());
		std::transform(b.bitset_value.begin(), b.bitset_value.end(), this->bitset_value.begin(), bitset.bitset_value.begin(), [](unsigned long long v0, unsigned long long v1) {
			unsigned long long v(v0);
			v &= v1;
			return v;
		});
		return bitset;
	};

	bool operator == (const CustomBitset& a)
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
	CustomBitset& operator=(const CustomBitset& other)
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