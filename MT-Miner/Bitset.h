#pragma once

#include <boost/dynamic_bitset.hpp>
#include <bitset>
#include <cassert>
#include <algorithm>
//#include <execution>
#ifdef _WIN32
#include <any>
#include <variant>
#endif

#define SIZE_0	item_count<10>	// 1024
#define SIZE_1	item_count<11>	// 2048
#define SIZE_2	item_count<12>	// 4096
#define SIZE_3	item_count<13>	// 8192
#define SIZE_4	item_count<14>	// 16348
#define SIZE_5	item_count<15>	// 32768
#define SIZE_6	item_count<16>	// 65535
//#define MAP_BITSET(i)  std::bitset<SIZE_#i#>

template <unsigned N>
constexpr std::uint32_t item_count = 1<<N;

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- //

/**
 * Bitset parent class
 * Describe minimum virtual interface for bitsets
 */
class Bitset
{
protected:
	unsigned int bitset_size;

public:
	virtual void reset(bool b = false) = 0;
	virtual unsigned int count() const = 0;
	virtual unsigned int size() const = 0;
	virtual bool get(unsigned int iAttribute) const = 0;
	virtual void set(unsigned int iAttribute, bool b = true) = 0;
	virtual Bitset& bitset_or(const Bitset & b) = 0;
	virtual Bitset& bitset_and(const Bitset & b) = 0;
	virtual bool bitset_compare(const Bitset& a) const = 0;
};

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- //

/**
 * Static Bitset implementation
 * Wrapper of std::bitset<n> with hardcoded n value
 * Template value can be std::bitset<SIZE_0>, std::bitset<SIZE_1>, ... std::bitset<SIZE_6>
 */
template <class T>
class StaticBitset : public Bitset
{
private:
	T bitset_value;

public:
	StaticBitset();
	StaticBitset(unsigned int bitsetSize);
	~StaticBitset();

	void reset(bool b = false) override;
	unsigned int count() const override;
	unsigned int size() const override;
	bool get(unsigned int iAttribute) const override;
	void set(unsigned int iAttribute, bool b = true) override;
	Bitset& bitset_or(const Bitset& b) override;
	Bitset& bitset_and(const Bitset& b) override;
	bool bitset_compare(const Bitset& a) const override;
	StaticBitset<T>& operator=(const StaticBitset<T>& other);
};

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- //

/**
 * Custom Bitset implementation
 * Use vector of unsigned long long value (64 bits) as a bitset
  */
class CustomBitset : public Bitset
{
private:	
	// use block of 32 bits (unsigned long int) to represent a bitset
	// https://fr.cppreference.com/w/cpp/language/types
	std::vector<unsigned long int> bitset_value;

	const unsigned int memory_size;

public:
	CustomBitset();		
	CustomBitset(unsigned int bitsetSize);
	CustomBitset(const CustomBitset& bitset);
	~CustomBitset();

	void reset(bool b = false) override;
	void set(unsigned int iAttribute, bool b = true) override;
	bool get(unsigned int iAttribute) const override;
	unsigned int size() const override;
	unsigned int count() const override;
	Bitset& bitset_or(const Bitset& b) override;
	Bitset& bitset_and(const Bitset& b) override;
	bool bitset_compare(const Bitset& b) const override;
	CustomBitset& operator=(const CustomBitset& other);
};

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- //

/**
* Dynamic Bitset implementation
* Wrapper of boost dynamic bitset 
*/ 
class DynamicBitset : public Bitset
{
private:
	boost::dynamic_bitset<> bitset_value;

public:
	DynamicBitset();
	DynamicBitset(unsigned int bitsetSize);
	DynamicBitset(const DynamicBitset& bitset);
	~DynamicBitset();

	void reset(bool b = false) override;
	void set(unsigned int iAttribute, bool b = true) override;
	bool get(unsigned int iAttribute) const override;
	unsigned int size() const override;
	unsigned int count() const override;
	Bitset& bitset_or(const Bitset& b) override;
	Bitset& bitset_and(const Bitset& b) override;
	bool bitset_compare(const Bitset& b) const override;
	DynamicBitset& operator=(const DynamicBitset& other);
};

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- //

/**
* Sparse Bitset implementation
* Use sparsity property of bitsets
* Stores a vector of indexes
* an index is a reference to the ith element corresponding to set bit from the original bitset
*/ 
class SparseIndexBitset : public Bitset
{
private:
	std::list<unsigned int> bitset_value;

public:
	SparseIndexBitset();
	SparseIndexBitset(unsigned int bitsetSize);
	SparseIndexBitset(const SparseIndexBitset& bitset);
	~SparseIndexBitset();

	void reset(bool b = false) override;
	void set(unsigned int iAttribute, bool b = true) override;
	bool get(unsigned int iAttribute) const override;
	unsigned int size() const override;
	unsigned int count() const override;
	Bitset& bitset_or(const Bitset& b) override;
	Bitset& bitset_and(const Bitset& b) override;
	bool bitset_compare(const Bitset& b) const override;
	SparseIndexBitset& operator=(const SparseIndexBitset& other);
};

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- //

#ifdef _WIN32
/**
* Any Bitset implementation
* Wrapper to a bitset of type any
* can be can be std::bitset<SIZE_0>, std::bitset<SIZE_1>, ... std::bitset<SIZE_6>
*/
class AnyBitset : public Bitset
{
private:
	const std::vector<unsigned int> variant_size = { SIZE_0, SIZE_1, SIZE_2, SIZE_3, SIZE_4, SIZE_5, SIZE_6 };

	std::any bitset_value;

public:
	AnyBitset();
	AnyBitset(unsigned int bitsetSize);
	~AnyBitset();

	void reset(bool b = false) override;
	void set(unsigned int iAttribute, bool b = true) override;
	bool get(unsigned int iAttribute) const override;
	unsigned int size() const override;
	unsigned int count() const override;
	Bitset& bitset_or(const Bitset& b) override;
	Bitset& bitset_and(const Bitset& b) override;
	bool bitset_compare(const Bitset& b)  const override;
	AnyBitset& operator=(const AnyBitset& other);
};
#endif

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- //

#ifdef _WIN32
/**
* Variant Bitset implementation
* Wrapper to a bitset of type variant
* can be can be std::bitset<SIZE_0>, std::bitset<SIZE_1>, ... std::bitset<SIZE_6>
*/
class VariantBitset : public Bitset
{
private:
	const std::vector<unsigned int> variant_size = { SIZE_0, SIZE_1, SIZE_2, SIZE_3, SIZE_4, SIZE_5, SIZE_6 };
	std::variant<std::bitset<SIZE_0>, std::bitset<SIZE_1>, std::bitset<SIZE_2>, std::bitset<SIZE_3>, std::bitset<SIZE_4>, std::bitset<SIZE_5>, std::bitset<SIZE_6>> bitset_value;

public:
	VariantBitset();
	VariantBitset(unsigned int bitsetSize);
	~VariantBitset();

	void reset(bool b = false) override;
	void set(unsigned int iAttribute, bool b = true) override;
	bool get(unsigned int iAttribute) const override;
	unsigned int size() const override;
	unsigned int count() const override;
	Bitset& bitset_or(const Bitset& b) override;
	Bitset& bitset_and(const Bitset& b) override;
	bool bitset_compare(const Bitset& b) const override;
	VariantBitset& operator=(const VariantBitset& other);
};
#endif // WIN32

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- //
// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- //

template <class T>
inline void StaticBitset<T>::reset(bool b)
{
	this->bitset_value.reset(b);
}

template <class T>
inline unsigned int StaticBitset<T>::count() const
{
	return static_cast<unsigned int>(this->bitset_value.count());
}

template <class T>
inline unsigned int StaticBitset<T>::size() const
{
	return this->bitset_size;
}

template <class T>
inline bool StaticBitset<T>::get(unsigned int iAttribute) const
{
	return this->bitset_value[iAttribute];
}

template <class T>
inline void StaticBitset<T>::set(unsigned int iAttribute, bool b)
{
	this->bitset_value.set(iAttribute, b);
}

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- //

inline unsigned int CustomBitset::size() const
{
	return this->bitset_size;
};

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- //

inline void DynamicBitset::reset(bool b)
{
	bitset_value.reset();
};

inline void DynamicBitset::set(unsigned int iAttribute, bool b)
{
	this->bitset_value[iAttribute] = b;
};

inline bool DynamicBitset::get(unsigned int iAttribute) const
{
	assert(iAttribute < size());
	return this->bitset_value[iAttribute];
};

inline unsigned int DynamicBitset::size() const
{
	return this->bitset_size;
};

inline unsigned int DynamicBitset::count() const
{
	return static_cast<unsigned int>(this->bitset_value.count());
};

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- //

#ifdef _WIN32
inline unsigned int AnyBitset::size() const
{
	return this->bitset_size;
};
#endif

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- //

#ifdef _WIN32
inline unsigned int VariantBitset::size() const
{
	return this->bitset_size;
};

inline bool VariantBitset::bitset_compare(const Bitset& b) const
{
	return dynamic_cast<const VariantBitset&>(b).bitset_value == this->bitset_value;
};
#endif
