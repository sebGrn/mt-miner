
#include "Bitset.h"

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- //
// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- //

template <class T>
inline StaticBitset<T>::StaticBitset() : bitset_value()
{
	this->bitset_size = 0;
};

template <class T>
inline StaticBitset<T>::StaticBitset(unsigned int bitsetSize)
{
	this->bitset_size = bitsetSize;
};

template <class T>
inline StaticBitset<T>::~StaticBitset()
{};

template <class T>
inline Bitset& StaticBitset<T>::bitset_or(const Bitset& b)
{
	const StaticBitset& staticBitset = dynamic_cast<const StaticBitset&>(b);
	this->bitset_value = staticBitset.bitset_value | this->bitset_value;
	return *this;
}

template <class T>
inline Bitset& StaticBitset<T>::bitset_and(const Bitset& b)
{
	const StaticBitset& staticBitset = dynamic_cast<const StaticBitset&>(b);
	this->bitset_value = staticBitset.bitset_value & this->bitset_value;
	return *this;
}

template <class T>
inline bool StaticBitset<T>::bitset_compare(const Bitset& a) const
{
	const StaticBitset& b = dynamic_cast<const StaticBitset&>(a);
	return b.bitset_value == this->bitset_value;
};

template <class T>
inline StaticBitset<T>& StaticBitset<T>::operator=(const StaticBitset& other)
{
	if (this != &other)
	{
		this->bitset_size = dynamic_cast<const StaticBitset&>(other).bitset_size;
		this->bitset_value = dynamic_cast<const StaticBitset&>(other).bitset_value;
	}
	return *this;
}

// template class implementation
template class StaticBitset<std::bitset<SIZE_0>>;
template class StaticBitset<std::bitset<SIZE_1>>;
template class StaticBitset<std::bitset<SIZE_2>>;
template class StaticBitset<std::bitset<SIZE_3>>;
template class StaticBitset<std::bitset<SIZE_4>>;
template class StaticBitset<std::bitset<SIZE_5>>;
template class StaticBitset<std::bitset<SIZE_6>>;

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- //
// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- //

CustomBitset::CustomBitset() : memory_size(64)
{
	this->bitset_size = 0;
};
		
CustomBitset::CustomBitset(unsigned int bitsetSize) : memory_size(64)
{
	this->bitset_size = bitsetSize;
	unsigned int n = (bitsetSize / memory_size) + 1;
	this->bitset_value.resize(n);
	std::for_each(this->bitset_value.begin(), this->bitset_value.end(), [](unsigned long int& v) { v = 0ULL; });
};

CustomBitset::CustomBitset(const CustomBitset& bitset) : memory_size(64)
{
	this->bitset_size = bitset.size();
	std::copy(bitset.bitset_value.begin(), bitset.bitset_value.end(), std::back_inserter(this->bitset_value));
};

CustomBitset::~CustomBitset()
{
}

void CustomBitset::reset(bool b) 
{
	std::for_each(this->bitset_value.begin(), this->bitset_value.end(), [&b](unsigned long int& v) { v = (b ? 1ULL : 0ULL); });
};

void CustomBitset::set(unsigned int iAttribute, bool b) 
{
	unsigned int iArray = iAttribute / memory_size;
	unsigned int iBit = iAttribute % memory_size;
	assert(iArray < this->bitset_value.size());
	unsigned long int number = this->bitset_value[iArray];
	number |= (b ? 1ULL : 0ULL) << iBit;
	this->bitset_value[iArray] = number;
};

bool CustomBitset::get(unsigned int iAttribute) const 
{
	assert(iAttribute < size());
	unsigned int iArray = iAttribute / memory_size;
	unsigned int iBit = iAttribute % memory_size;
	assert(iArray < this->bitset_value.size());
	unsigned long int number = this->bitset_value[iArray];
	bool bit = (number >> iBit) & 1ULL;
	return bit;
};


unsigned int CustomBitset::count() const 
{
	unsigned int count(0);
	std::for_each(this->bitset_value.begin(), this->bitset_value.end(), [&count, this](unsigned long long v) {
		// Brian Kernighan’s Algorithm
		// https://www.geeksforgeeks.org/count-set-bits-in-an-integer/
		unsigned long int n(v);
		while (n)
		{
			n &= (n - 1);
			count++;
		}
	});
	return count;
};

Bitset& CustomBitset::bitset_or(const Bitset& b) 
{
	const CustomBitset& a = dynamic_cast<const CustomBitset&>(b);
	std::transform(a.bitset_value.begin(), a.bitset_value.end(), this->bitset_value.begin(), this->bitset_value.begin(), [](unsigned long int v0, unsigned long int v1) {
		unsigned long int v(v0);
		v |= v1;
		return v;
	});
	return *this;
};

Bitset& CustomBitset::bitset_and(const Bitset& b) 
{
	const CustomBitset& a = dynamic_cast<const CustomBitset&>(b);
	CustomBitset bitset(b.size());
	std::transform(a.bitset_value.begin(), a.bitset_value.end(), this->bitset_value.begin(), this->bitset_value.begin(), [](unsigned long int v0, unsigned long int v1) {
		unsigned long int v(v0);
		v &= v1;
		return v;
	});
	return *this;
};

bool CustomBitset::bitset_compare(const Bitset& b) const 
{
	if (b.size() != this->bitset_size)
		return false;

	const CustomBitset& a = dynamic_cast<const CustomBitset&>(b);
	unsigned int n = (this->bitset_size / memory_size) + 1;
	bool equal = true;
	return std::equal(a.bitset_value.begin(), a.bitset_value.end(), this->bitset_value.begin());
};

// copy assignment
CustomBitset& CustomBitset::operator=(const CustomBitset& other)
{
	if (this != &other)
	{
		const CustomBitset& a = dynamic_cast<const CustomBitset&>(other);
		this->bitset_value.clear();
		this->bitset_size = a.size();
		std::copy(a.bitset_value.begin(), a.bitset_value.end(), std::back_inserter(this->bitset_value));
	}
	return *this;
}

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- //
// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- //

DynamicBitset::DynamicBitset()
{
	this->bitset_size = 0;
};

DynamicBitset::DynamicBitset(unsigned int bitsetSize) : bitset_value(bitset_size)
{
	this->bitset_size = bitsetSize;		
};

DynamicBitset::DynamicBitset(const DynamicBitset& bitset)
{
	this->bitset_size = bitset.size();
	this->bitset_value = bitset.bitset_value;
};

DynamicBitset::~DynamicBitset()
{};

Bitset& DynamicBitset::bitset_or(const Bitset& b) 
{
	const DynamicBitset& a = dynamic_cast<const DynamicBitset&>(b);
	this->bitset_value = this->bitset_value | a.bitset_value;
	return *this;
};

Bitset& DynamicBitset::bitset_and(const Bitset& b) 
{
	const DynamicBitset& a = dynamic_cast<const DynamicBitset&>(b);
	this->bitset_value = this->bitset_value & a.bitset_value;
	return *this;
};

bool DynamicBitset::bitset_compare(const Bitset& b) const 
{
	if (b.size() != this->bitset_size)
		return false;

	const DynamicBitset& a = dynamic_cast<const DynamicBitset&>(b);
	return a.bitset_value == this->bitset_value;
};

// copy assignment
DynamicBitset& DynamicBitset::operator=(const DynamicBitset& other)
{
	if (this != &other)
	{
		const DynamicBitset& a = dynamic_cast<const DynamicBitset&>(other);
		this->bitset_value = a.bitset_value;
	}
	return *this;
}

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- //
// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- //

#ifdef _WIN32
AnyBitset::AnyBitset()
{
	this->bitset_size = 0;
};

AnyBitset::AnyBitset(unsigned int bitsetSize)
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

AnyBitset::~AnyBitset()
{};

void AnyBitset::reset(bool b) 
{
	if (this->bitset_size < variant_size[0])			this->bitset_value = std::any_cast<std::bitset<SIZE_0>>(this->bitset_value).reset();
	else if (this->bitset_size < variant_size[1])		this->bitset_value = std::any_cast<std::bitset<SIZE_1>>(this->bitset_value).reset();
	else if (this->bitset_size < variant_size[2])		this->bitset_value = std::any_cast<std::bitset<SIZE_2>>(this->bitset_value).reset();
	else if (this->bitset_size < variant_size[3])		this->bitset_value = std::any_cast<std::bitset<SIZE_3>>(this->bitset_value).reset();
	else if (this->bitset_size < variant_size[4])		this->bitset_value = std::any_cast<std::bitset<SIZE_4>>(this->bitset_value).reset();
	else if (this->bitset_size < variant_size[5])		this->bitset_value = std::any_cast<std::bitset<SIZE_5>>(this->bitset_value).reset();
	else if (this->bitset_size < variant_size[6])		this->bitset_value = std::any_cast<std::bitset<SIZE_6>>(this->bitset_value).reset();
};

void AnyBitset::set(unsigned int iAttribute, bool b) 
{
	if (this->bitset_size < variant_size[0])			this->bitset_value = std::any_cast<std::bitset<SIZE_0>>(this->bitset_value).set(iAttribute, b);
	else if (this->bitset_size < variant_size[1])		this->bitset_value = std::any_cast<std::bitset<SIZE_1>>(this->bitset_value).set(iAttribute, b);
	else if (this->bitset_size < variant_size[2])		this->bitset_value = std::any_cast<std::bitset<SIZE_2>>(this->bitset_value).set(iAttribute, b);
	else if (this->bitset_size < variant_size[3])		this->bitset_value = std::any_cast<std::bitset<SIZE_3>>(this->bitset_value).set(iAttribute, b);
	else if (this->bitset_size < variant_size[4])		this->bitset_value = std::any_cast<std::bitset<SIZE_4>>(this->bitset_value).set(iAttribute, b);
	else if (this->bitset_size < variant_size[5])		this->bitset_value = std::any_cast<std::bitset<SIZE_5>>(this->bitset_value).set(iAttribute, b);
	else if (this->bitset_size < variant_size[6])		this->bitset_value = std::any_cast<std::bitset<SIZE_6>>(this->bitset_value).set(iAttribute, b);
}

bool AnyBitset::get(unsigned int iAttribute) const 
{
	if (this->bitset_size < variant_size[0])			return std::any_cast<std::bitset<SIZE_0>>(this->bitset_value)[iAttribute];
	else if (this->bitset_size < variant_size[1])		return std::any_cast<std::bitset<SIZE_1>>(this->bitset_value)[iAttribute];
	else if (this->bitset_size < variant_size[2])		return std::any_cast<std::bitset<SIZE_2>>(this->bitset_value)[iAttribute];
	else if (this->bitset_size < variant_size[3])		return std::any_cast<std::bitset<SIZE_3>>(this->bitset_value)[iAttribute];
	else if (this->bitset_size < variant_size[4])		return std::any_cast<std::bitset<SIZE_4>>(this->bitset_value)[iAttribute];
	else if (this->bitset_size < variant_size[5])		return std::any_cast<std::bitset<SIZE_5>>(this->bitset_value)[iAttribute];
	else if (this->bitset_size < variant_size[6])		return std::any_cast<std::bitset<SIZE_6>>(this->bitset_value)[iAttribute];
	return false;
}

unsigned int AnyBitset::count() const 
{
	if (this->bitset_size < variant_size[0]) { std::bitset<SIZE_0> x = std::any_cast<std::bitset<SIZE_0>>(this->bitset_value); return static_cast<unsigned int>(x.count()); }
	else if (this->bitset_size < variant_size[1]) { std::bitset<SIZE_1> x = std::any_cast<std::bitset<SIZE_1>>(this->bitset_value); return static_cast<unsigned int>(x.count()); }
	else if (this->bitset_size < variant_size[2]) { std::bitset<SIZE_2> x = std::any_cast<std::bitset<SIZE_2>>(this->bitset_value); return static_cast<unsigned int>(x.count()); }
	else if (this->bitset_size < variant_size[3]) { std::bitset<SIZE_3> x = std::any_cast<std::bitset<SIZE_3>>(this->bitset_value); return static_cast<unsigned int>(x.count()); }
	else if (this->bitset_size < variant_size[4]) { std::bitset<SIZE_4> x = std::any_cast<std::bitset<SIZE_4>>(this->bitset_value); return static_cast<unsigned int>(x.count()); }
	else if (this->bitset_size < variant_size[5]) { std::bitset<SIZE_5> x = std::any_cast<std::bitset<SIZE_5>>(this->bitset_value); return static_cast<unsigned int>(x.count()); }
	else if (this->bitset_size < variant_size[6]) { std::bitset<SIZE_6> x = std::any_cast<std::bitset<SIZE_6>>(this->bitset_value); return static_cast<unsigned int>(x.count()); }
	return 0;
}

Bitset& AnyBitset::bitset_or(const Bitset& b) 
{
	const AnyBitset& a = dynamic_cast<const AnyBitset&>(b);
	if (this->bitset_size < variant_size[0]) { this->bitset_value = std::any_cast<std::bitset<SIZE_0>>(a.bitset_value) | std::any_cast<std::bitset<SIZE_0>>(this->bitset_value); }
	else if (this->bitset_size < variant_size[1]) { this->bitset_value = std::any_cast<std::bitset<SIZE_1>>(a.bitset_value) | std::any_cast<std::bitset<SIZE_1>>(this->bitset_value); }
	else if (this->bitset_size < variant_size[2]) { this->bitset_value = std::any_cast<std::bitset<SIZE_2>>(a.bitset_value) | std::any_cast<std::bitset<SIZE_2>>(this->bitset_value); }
	else if (this->bitset_size < variant_size[3]) { this->bitset_value = std::any_cast<std::bitset<SIZE_3>>(a.bitset_value) | std::any_cast<std::bitset<SIZE_3>>(this->bitset_value); }
	else if (this->bitset_size < variant_size[4]) { this->bitset_value = std::any_cast<std::bitset<SIZE_4>>(a.bitset_value) | std::any_cast<std::bitset<SIZE_4>>(this->bitset_value); }
	else if (this->bitset_size < variant_size[5]) { this->bitset_value = std::any_cast<std::bitset<SIZE_5>>(a.bitset_value) | std::any_cast<std::bitset<SIZE_5>>(this->bitset_value); }
	else if (this->bitset_size < variant_size[6]) { this->bitset_value = std::any_cast<std::bitset<SIZE_6>>(a.bitset_value) | std::any_cast<std::bitset<SIZE_6>>(this->bitset_value); }
	return *this;
};

Bitset& AnyBitset::bitset_and(const Bitset& b) 
{
	const AnyBitset& a = dynamic_cast<const AnyBitset&>(b);
	if (this->bitset_size < variant_size[0]) { this->bitset_value = std::any_cast<std::bitset<SIZE_0>>(a.bitset_value) & std::any_cast<std::bitset<SIZE_0>>(this->bitset_value); }
	else if (this->bitset_size < variant_size[1]) { this->bitset_value = std::any_cast<std::bitset<SIZE_1>>(a.bitset_value) & std::any_cast<std::bitset<SIZE_1>>(this->bitset_value); }
	else if (this->bitset_size < variant_size[2]) { this->bitset_value = std::any_cast<std::bitset<SIZE_2>>(a.bitset_value) & std::any_cast<std::bitset<SIZE_2>>(this->bitset_value); }
	else if (this->bitset_size < variant_size[3]) { this->bitset_value = std::any_cast<std::bitset<SIZE_3>>(a.bitset_value) & std::any_cast<std::bitset<SIZE_3>>(this->bitset_value); }
	else if (this->bitset_size < variant_size[4]) { this->bitset_value = std::any_cast<std::bitset<SIZE_4>>(a.bitset_value) & std::any_cast<std::bitset<SIZE_4>>(this->bitset_value); }
	else if (this->bitset_size < variant_size[5]) { this->bitset_value = std::any_cast<std::bitset<SIZE_5>>(a.bitset_value) & std::any_cast<std::bitset<SIZE_5>>(this->bitset_value); }
	else if (this->bitset_size < variant_size[6]) { this->bitset_value = std::any_cast<std::bitset<SIZE_6>>(a.bitset_value) & std::any_cast<std::bitset<SIZE_6>>(this->bitset_value); }
	return *this;
};

bool AnyBitset::bitset_compare(const Bitset& b)  const 
{
	const AnyBitset& a = dynamic_cast<const AnyBitset&>(b);
	if (this->bitset_size < variant_size[0]) { return std::any_cast<std::bitset<SIZE_0>>(a.bitset_value) == std::any_cast<std::bitset<SIZE_0>>(this->bitset_value); }
	else if (this->bitset_size < variant_size[1]) { return std::any_cast<std::bitset<SIZE_1>>(a.bitset_value) == std::any_cast<std::bitset<SIZE_1>>(this->bitset_value); }
	else if (this->bitset_size < variant_size[2]) { return std::any_cast<std::bitset<SIZE_2>>(a.bitset_value) == std::any_cast<std::bitset<SIZE_2>>(this->bitset_value); }
	else if (this->bitset_size < variant_size[3]) { return std::any_cast<std::bitset<SIZE_3>>(a.bitset_value) == std::any_cast<std::bitset<SIZE_3>>(this->bitset_value); }
	else if (this->bitset_size < variant_size[4]) { return std::any_cast<std::bitset<SIZE_4>>(a.bitset_value) == std::any_cast<std::bitset<SIZE_4>>(this->bitset_value); }
	else if (this->bitset_size < variant_size[5]) { return std::any_cast<std::bitset<SIZE_5>>(a.bitset_value) == std::any_cast<std::bitset<SIZE_5>>(this->bitset_value); }
	else if (this->bitset_size < variant_size[6]) { return std::any_cast<std::bitset<SIZE_6>>(a.bitset_value) == std::any_cast<std::bitset<SIZE_6>>(this->bitset_value); }
	return false;
};

// copy assignment
AnyBitset& AnyBitset::operator=(const AnyBitset& other)
{
	if (this != &other)
	{
		const AnyBitset& a = dynamic_cast<const AnyBitset&>(other);
		this->bitset_size = a.bitset_size;
		this->bitset_value = a.bitset_value;
	}
	return *this;
}
#endif

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- //

#ifdef _WIN32
VariantBitset::VariantBitset()
{
	this->bitset_size = 0;
};

VariantBitset::VariantBitset(unsigned int bitsetSize)
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

VariantBitset::~VariantBitset()
{};

void VariantBitset::reset(bool b) 
{
	if (this->bitset_size < variant_size[0])			this->bitset_value = std::get<std::bitset<SIZE_0>>(this->bitset_value).reset();
	else if (this->bitset_size < variant_size[1])		this->bitset_value = std::get<std::bitset<SIZE_1>>(this->bitset_value).reset();
	else if (this->bitset_size < variant_size[2])		this->bitset_value = std::get<std::bitset<SIZE_2>>(this->bitset_value).reset();
	else if (this->bitset_size < variant_size[3])		this->bitset_value = std::get<std::bitset<SIZE_3>>(this->bitset_value).reset();
	else if (this->bitset_size < variant_size[4])		this->bitset_value = std::get<std::bitset<SIZE_4>>(this->bitset_value).reset();
	else if (this->bitset_size < variant_size[5])		this->bitset_value = std::get<std::bitset<SIZE_5>>(this->bitset_value).reset();
	else if (this->bitset_size < variant_size[6])		this->bitset_value = std::get<std::bitset<SIZE_6>>(this->bitset_value).reset();
};

void VariantBitset::set(unsigned int iAttribute, bool b) 
{
	if (this->bitset_size < variant_size[0])			this->bitset_value = std::get<std::bitset<SIZE_0>>(this->bitset_value).set(iAttribute, b);
	else if (this->bitset_size < variant_size[1])		this->bitset_value = std::get<std::bitset<SIZE_1>>(this->bitset_value).set(iAttribute, b);
	else if (this->bitset_size < variant_size[2])		this->bitset_value = std::get<std::bitset<SIZE_2>>(this->bitset_value).set(iAttribute, b);
	else if (this->bitset_size < variant_size[3])		this->bitset_value = std::get<std::bitset<SIZE_3>>(this->bitset_value).set(iAttribute, b);
	else if (this->bitset_size < variant_size[4])		this->bitset_value = std::get<std::bitset<SIZE_4>>(this->bitset_value).set(iAttribute, b);
	else if (this->bitset_size < variant_size[5])		this->bitset_value = std::get<std::bitset<SIZE_5>>(this->bitset_value).set(iAttribute, b);
	else if (this->bitset_size < variant_size[6])		this->bitset_value = std::get<std::bitset<SIZE_6>>(this->bitset_value).set(iAttribute, b);
}

bool VariantBitset::get(unsigned int iAttribute) const 
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

unsigned int VariantBitset::count() const 
{
	if (this->bitset_size < variant_size[0]) { std::bitset<SIZE_0> x = std::get<std::bitset<SIZE_0>>(this->bitset_value); return static_cast<unsigned int>(x.count()); }
	else if (this->bitset_size < variant_size[1]) { std::bitset<SIZE_1> x = std::get<std::bitset<SIZE_1>>(this->bitset_value); return static_cast<unsigned int>(x.count()); }
	else if (this->bitset_size < variant_size[2]) { std::bitset<SIZE_2> x = std::get<std::bitset<SIZE_2>>(this->bitset_value); return static_cast<unsigned int>(x.count()); }
	else if (this->bitset_size < variant_size[3]) { std::bitset<SIZE_3> x = std::get<std::bitset<SIZE_3>>(this->bitset_value); return static_cast<unsigned int>(x.count()); }
	else if (this->bitset_size < variant_size[4]) { std::bitset<SIZE_4> x = std::get<std::bitset<SIZE_4>>(this->bitset_value); return static_cast<unsigned int>(x.count()); }
	else if (this->bitset_size < variant_size[5]) { std::bitset<SIZE_5> x = std::get<std::bitset<SIZE_5>>(this->bitset_value); return static_cast<unsigned int>(x.count()); }
	else if (this->bitset_size < variant_size[6]) { std::bitset<SIZE_6> x = std::get<std::bitset<SIZE_6>>(this->bitset_value); return static_cast<unsigned int>(x.count()); }
	return 0;
}

Bitset& VariantBitset::bitset_or(const Bitset& b) 
{
	const VariantBitset& a = dynamic_cast<const VariantBitset&>(b);
	if (this->bitset_size < variant_size[0]) { this->bitset_value = std::get<std::bitset<SIZE_0>>(a.bitset_value) | std::get<std::bitset<SIZE_0>>(this->bitset_value); }
	else if (this->bitset_size < variant_size[1]) { this->bitset_value = std::get<std::bitset<SIZE_1>>(a.bitset_value) | std::get<std::bitset<SIZE_1>>(this->bitset_value); }
	else if (this->bitset_size < variant_size[2]) { this->bitset_value = std::get<std::bitset<SIZE_2>>(a.bitset_value) | std::get<std::bitset<SIZE_2>>(this->bitset_value); }
	else if (this->bitset_size < variant_size[3]) { this->bitset_value = std::get<std::bitset<SIZE_3>>(a.bitset_value) | std::get<std::bitset<SIZE_3>>(this->bitset_value); }
	else if (this->bitset_size < variant_size[4]) { this->bitset_value = std::get<std::bitset<SIZE_4>>(a.bitset_value) | std::get<std::bitset<SIZE_4>>(this->bitset_value); }
	else if (this->bitset_size < variant_size[5]) { this->bitset_value = std::get<std::bitset<SIZE_5>>(a.bitset_value) | std::get<std::bitset<SIZE_5>>(this->bitset_value); }
	else if (this->bitset_size < variant_size[6]) { this->bitset_value = std::get<std::bitset<SIZE_6>>(a.bitset_value) | std::get<std::bitset<SIZE_6>>(this->bitset_value); }
	return *this;
};

Bitset& VariantBitset::bitset_and(const Bitset& b) 
{
	const VariantBitset& a = dynamic_cast<const VariantBitset&>(b);
	if (this->bitset_size < variant_size[0]) { this->bitset_value = std::get<std::bitset<SIZE_0>>(a.bitset_value) & std::get<std::bitset<SIZE_0>>(this->bitset_value); }
	else if (this->bitset_size < variant_size[1]) { this->bitset_value = std::get<std::bitset<SIZE_1>>(a.bitset_value) & std::get<std::bitset<SIZE_1>>(this->bitset_value); }
	else if (this->bitset_size < variant_size[2]) { this->bitset_value = std::get<std::bitset<SIZE_2>>(a.bitset_value) & std::get<std::bitset<SIZE_2>>(this->bitset_value); }
	else if (this->bitset_size < variant_size[3]) { this->bitset_value = std::get<std::bitset<SIZE_3>>(a.bitset_value) & std::get<std::bitset<SIZE_3>>(this->bitset_value); }
	else if (this->bitset_size < variant_size[4]) { this->bitset_value = std::get<std::bitset<SIZE_4>>(a.bitset_value) & std::get<std::bitset<SIZE_4>>(this->bitset_value); }
	else if (this->bitset_size < variant_size[5]) { this->bitset_value = std::get<std::bitset<SIZE_5>>(a.bitset_value) & std::get<std::bitset<SIZE_5>>(this->bitset_value); }
	else if (this->bitset_size < variant_size[6]) { this->bitset_value = std::get<std::bitset<SIZE_6>>(a.bitset_value) & std::get<std::bitset<SIZE_6>>(this->bitset_value); }
	return *this;
};

// copy assignment
VariantBitset& VariantBitset::operator=(const VariantBitset& other)
{
	if (this != &other)
	{
		const VariantBitset& a = dynamic_cast<const VariantBitset&>(other);
		this->bitset_size = a.bitset_size;
		this->bitset_value = a.bitset_value;
	}
	return *this;
}
#endif // WIN32

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- //

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

unsigned int SparseIndexBitset::count() const
{
	return static_cast<unsigned int>(this->bitset_value.size());
}

Bitset& SparseIndexBitset::bitset_or(const Bitset& b)
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

Bitset& SparseIndexBitset::bitset_and(const Bitset& b)
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
}