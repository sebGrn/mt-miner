#pragma once
#include <boost/dynamic_bitset.hpp>
#include <bitset>
#include <cassert>
#include <any>
#include <variant>
#include <algorithm>
#include <execution>

#define SIZE_0	item_count<10>	// 1024
#define SIZE_1	item_count<11>	// 2048
#define SIZE_2	item_count<12>	// 4096
#define SIZE_3	item_count<13>	// 8192
#define SIZE_4	item_count<14>	// 16348
#define SIZE_5	item_count<15>	// 32768
#define SIZE_6	item_count<16>	// 65535
//#define MAP_BITSET(i)  std::bitset<SIZE_#i#>

template <unsigned N>
inline constexpr std::uint32_t item_count = 1<<N;

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- //
// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- //

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
// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- //

template <class T>
class StaticBitset : public Bitset
{
private:
	//std::bitset<SIZE_0> bitset_value;
	T bitset_value;

public:
	StaticBitset() : bitset_value()
	{
		this->bitset_size = 0;
	};

	StaticBitset(unsigned int bitsetSize)
	{
		this->bitset_size = bitsetSize;
	};

	~StaticBitset()
	{};

	void reset(bool b = false) override
	{
		this->bitset_value.reset(b);
	}

	unsigned int count() const override
	{
		return static_cast<unsigned int>(this->bitset_value.count());
	}

	unsigned int size() const override
	{
		return this->bitset_size;
	}

	bool get(unsigned int iAttribute) const override
	{
		return this->bitset_value[iAttribute];
	}

	void set(unsigned int iAttribute, bool b = true) override
	{
		this->bitset_value.set(iAttribute, b);
	}

	Bitset& bitset_or(const Bitset& b) override
	{
		const StaticBitset& staticBitset = dynamic_cast<const StaticBitset&>(b);
		this->bitset_value = staticBitset.bitset_value | this->bitset_value;
		return *this;
	}

	Bitset& bitset_and(const Bitset& b) override
	{
		const StaticBitset& staticBitset = dynamic_cast<const StaticBitset&>(b);
		this->bitset_value = staticBitset.bitset_value & this->bitset_value;
		return *this;
	}

	bool bitset_compare(const Bitset& a) const override
	{
		const StaticBitset& b = dynamic_cast<const StaticBitset&>(a);
		return b.bitset_value == this->bitset_value;
	};

	StaticBitset& operator=(const StaticBitset& other)
	{
		if (this != &other)
		{
			this->bitset_size = dynamic_cast<const StaticBitset&>(other).bitset_size;
			this->bitset_value = dynamic_cast<const StaticBitset&>(other).bitset_value;
		}
		return *this;
	}
};



// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- //
// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- //

class AnyBitset : public Bitset
{
private:
	const std::vector<unsigned int> variant_size = { SIZE_0, SIZE_1, SIZE_2, SIZE_3, SIZE_4, SIZE_5, SIZE_6 };

	std::any bitset_value;

public:
	AnyBitset()
	{
		this->bitset_size = 0;
	};

	AnyBitset(unsigned int bitsetSize)
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

	~AnyBitset()
	{};

	void reset(bool b = false)
	{
		if (this->bitset_size < variant_size[0])			this->bitset_value = std::any_cast<std::bitset<SIZE_0>>(this->bitset_value).reset();
		else if (this->bitset_size < variant_size[1])		this->bitset_value = std::any_cast<std::bitset<SIZE_1>>(this->bitset_value).reset();
		else if (this->bitset_size < variant_size[2])		this->bitset_value = std::any_cast<std::bitset<SIZE_2>>(this->bitset_value).reset();
		else if (this->bitset_size < variant_size[3])		this->bitset_value = std::any_cast<std::bitset<SIZE_3>>(this->bitset_value).reset();
		else if (this->bitset_size < variant_size[4])		this->bitset_value = std::any_cast<std::bitset<SIZE_4>>(this->bitset_value).reset();
		else if (this->bitset_size < variant_size[5])		this->bitset_value = std::any_cast<std::bitset<SIZE_5>>(this->bitset_value).reset();
		else if (this->bitset_size < variant_size[6])		this->bitset_value = std::any_cast<std::bitset<SIZE_6>>(this->bitset_value).reset();
	};

	void set(unsigned int iAttribute, bool b = true)
	{
		if (this->bitset_size < variant_size[0])			this->bitset_value = std::any_cast<std::bitset<SIZE_0>>(this->bitset_value).set(iAttribute, b);
		else if (this->bitset_size < variant_size[1])		this->bitset_value = std::any_cast<std::bitset<SIZE_1>>(this->bitset_value).set(iAttribute, b);
		else if (this->bitset_size < variant_size[2])		this->bitset_value = std::any_cast<std::bitset<SIZE_2>>(this->bitset_value).set(iAttribute, b);
		else if (this->bitset_size < variant_size[3])		this->bitset_value = std::any_cast<std::bitset<SIZE_3>>(this->bitset_value).set(iAttribute, b);
		else if (this->bitset_size < variant_size[4])		this->bitset_value = std::any_cast<std::bitset<SIZE_4>>(this->bitset_value).set(iAttribute, b);
		else if (this->bitset_size < variant_size[5])		this->bitset_value = std::any_cast<std::bitset<SIZE_5>>(this->bitset_value).set(iAttribute, b);
		else if (this->bitset_size < variant_size[6])		this->bitset_value = std::any_cast<std::bitset<SIZE_6>>(this->bitset_value).set(iAttribute, b);
	}

	bool get(unsigned int iAttribute) const
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

	unsigned int size() const
	{
		return this->bitset_size;
	};

	unsigned int count() const
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

	Bitset& bitset_or(const Bitset& b) override
	{
		const AnyBitset& a = dynamic_cast<const AnyBitset&>(b);
		if (this->bitset_size < variant_size[0])	  { this->bitset_value = std::any_cast<std::bitset<SIZE_0>>(a.bitset_value) | std::any_cast<std::bitset<SIZE_0>>(this->bitset_value); }
		else if (this->bitset_size < variant_size[1]) { this->bitset_value = std::any_cast<std::bitset<SIZE_1>>(a.bitset_value) | std::any_cast<std::bitset<SIZE_1>>(this->bitset_value); }
		else if (this->bitset_size < variant_size[2]) { this->bitset_value = std::any_cast<std::bitset<SIZE_2>>(a.bitset_value) | std::any_cast<std::bitset<SIZE_2>>(this->bitset_value); }
		else if (this->bitset_size < variant_size[3]) { this->bitset_value = std::any_cast<std::bitset<SIZE_3>>(a.bitset_value) | std::any_cast<std::bitset<SIZE_3>>(this->bitset_value); }
		else if (this->bitset_size < variant_size[4]) { this->bitset_value = std::any_cast<std::bitset<SIZE_4>>(a.bitset_value) | std::any_cast<std::bitset<SIZE_4>>(this->bitset_value); }
		else if (this->bitset_size < variant_size[5]) { this->bitset_value = std::any_cast<std::bitset<SIZE_5>>(a.bitset_value) | std::any_cast<std::bitset<SIZE_5>>(this->bitset_value); }
		else if (this->bitset_size < variant_size[6]) { this->bitset_value = std::any_cast<std::bitset<SIZE_6>>(a.bitset_value) | std::any_cast<std::bitset<SIZE_6>>(this->bitset_value); }
		return *this;
	};

	Bitset& bitset_and(const Bitset& b) override
	{
		const AnyBitset& a = dynamic_cast<const AnyBitset&>(b);
		if (this->bitset_size < variant_size[0])      { this->bitset_value = std::any_cast<std::bitset<SIZE_0>>(a.bitset_value) & std::any_cast<std::bitset<SIZE_0>>(this->bitset_value); }
		else if (this->bitset_size < variant_size[1]) { this->bitset_value = std::any_cast<std::bitset<SIZE_1>>(a.bitset_value) & std::any_cast<std::bitset<SIZE_1>>(this->bitset_value); }
		else if (this->bitset_size < variant_size[2]) { this->bitset_value = std::any_cast<std::bitset<SIZE_2>>(a.bitset_value) & std::any_cast<std::bitset<SIZE_2>>(this->bitset_value); }
		else if (this->bitset_size < variant_size[3]) { this->bitset_value = std::any_cast<std::bitset<SIZE_3>>(a.bitset_value) & std::any_cast<std::bitset<SIZE_3>>(this->bitset_value); }
		else if (this->bitset_size < variant_size[4]) { this->bitset_value = std::any_cast<std::bitset<SIZE_4>>(a.bitset_value) & std::any_cast<std::bitset<SIZE_4>>(this->bitset_value); }
		else if (this->bitset_size < variant_size[5]) { this->bitset_value = std::any_cast<std::bitset<SIZE_5>>(a.bitset_value) & std::any_cast<std::bitset<SIZE_5>>(this->bitset_value); }
		else if (this->bitset_size < variant_size[6]) { this->bitset_value = std::any_cast<std::bitset<SIZE_6>>(a.bitset_value) & std::any_cast<std::bitset<SIZE_6>>(this->bitset_value); }
		return *this;
	};

	bool bitset_compare(const Bitset& b)  const override
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
	AnyBitset& operator=(const AnyBitset& other)
	{
		if (this != &other)
		{
			const AnyBitset& a = dynamic_cast<const AnyBitset&>(other);
			this->bitset_size = a.bitset_size;
			this->bitset_value = a.bitset_value;
		}
		return *this;
	}
};

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- //
// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- //

class VariantBitset : public Bitset
{

private:
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

	Bitset& bitset_or(const Bitset& b) override
	{
		const VariantBitset& a = dynamic_cast<const VariantBitset&>(b);
		if (this->bitset_size < variant_size[0])	  { this->bitset_value = std::get<std::bitset<SIZE_0>>(a.bitset_value) | std::get<std::bitset<SIZE_0>>(this->bitset_value); }
		else if (this->bitset_size < variant_size[1]) { this->bitset_value = std::get<std::bitset<SIZE_1>>(a.bitset_value) | std::get<std::bitset<SIZE_1>>(this->bitset_value); }
		else if (this->bitset_size < variant_size[2]) { this->bitset_value = std::get<std::bitset<SIZE_2>>(a.bitset_value) | std::get<std::bitset<SIZE_2>>(this->bitset_value); }
		else if (this->bitset_size < variant_size[3]) { this->bitset_value = std::get<std::bitset<SIZE_3>>(a.bitset_value) | std::get<std::bitset<SIZE_3>>(this->bitset_value); }
		else if (this->bitset_size < variant_size[4]) { this->bitset_value = std::get<std::bitset<SIZE_4>>(a.bitset_value) | std::get<std::bitset<SIZE_4>>(this->bitset_value); }
		else if (this->bitset_size < variant_size[5]) { this->bitset_value = std::get<std::bitset<SIZE_5>>(a.bitset_value) | std::get<std::bitset<SIZE_5>>(this->bitset_value); }
		else if (this->bitset_size < variant_size[6]) { this->bitset_value = std::get<std::bitset<SIZE_6>>(a.bitset_value) | std::get<std::bitset<SIZE_6>>(this->bitset_value); }
		return *this;
	};

	Bitset& bitset_and(const Bitset& b) override
	{
		const VariantBitset& a = dynamic_cast<const VariantBitset&>(b);
		if (this->bitset_size < variant_size[0])	  { this->bitset_value = std::get<std::bitset<SIZE_0>>(a.bitset_value) & std::get<std::bitset<SIZE_0>>(this->bitset_value); }
		else if (this->bitset_size < variant_size[1]) { this->bitset_value = std::get<std::bitset<SIZE_1>>(a.bitset_value) & std::get<std::bitset<SIZE_1>>(this->bitset_value); }
		else if (this->bitset_size < variant_size[2]) { this->bitset_value = std::get<std::bitset<SIZE_2>>(a.bitset_value) & std::get<std::bitset<SIZE_2>>(this->bitset_value); }
		else if (this->bitset_size < variant_size[3]) { this->bitset_value = std::get<std::bitset<SIZE_3>>(a.bitset_value) & std::get<std::bitset<SIZE_3>>(this->bitset_value); }
		else if (this->bitset_size < variant_size[4]) { this->bitset_value = std::get<std::bitset<SIZE_4>>(a.bitset_value) & std::get<std::bitset<SIZE_4>>(this->bitset_value); }
		else if (this->bitset_size < variant_size[5]) { this->bitset_value = std::get<std::bitset<SIZE_5>>(a.bitset_value) & std::get<std::bitset<SIZE_5>>(this->bitset_value); }
		else if (this->bitset_size < variant_size[6]) { this->bitset_value = std::get<std::bitset<SIZE_6>>(a.bitset_value) & std::get<std::bitset<SIZE_6>>(this->bitset_value); }
		return *this;
	};

	bool bitset_compare(const Bitset& b) const override
	{
		return dynamic_cast<const VariantBitset&>(b).bitset_value == this->bitset_value;
	};

	// copy assignment
	VariantBitset& operator=(const VariantBitset& other)
	{
		if (this != &other)
		{
			const VariantBitset& a = dynamic_cast<const VariantBitset&>(other);
			this->bitset_size = a.bitset_size;
			this->bitset_value = a.bitset_value;
		}
		return *this;
	}
};

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- //
// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- //

class CustomBitset : public Bitset
{
private:	
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
		std::for_each(std::execution::seq, this->bitset_value.begin(), this->bitset_value.end(), [&count, this](unsigned long long v) {
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

	Bitset& bitset_or(const Bitset& b) override
	{
		const CustomBitset& a = dynamic_cast<const CustomBitset&>(b);
		std::transform(std::execution::seq, a.bitset_value.begin(), a.bitset_value.end(), this->bitset_value.begin(), this->bitset_value.begin(), [](unsigned long long v0, unsigned long long v1) {
			unsigned long long v(v0);
			v |= v1;
			return v;
		});
		return *this;
	};

	Bitset& bitset_and(const Bitset& b) override
	{
		const CustomBitset& a = dynamic_cast<const CustomBitset&>(b);
		CustomBitset bitset(b.size());
		std::transform(std::execution::seq, a.bitset_value.begin(), a.bitset_value.end(), this->bitset_value.begin(), this->bitset_value.begin(), [](unsigned long long v0, unsigned long long v1) {
			unsigned long long v(v0);
			v &= v1;
			return v;
		});
		return *this;
	};

	bool bitset_compare(const Bitset& b) const override
	{
		if (b.size() != this->bitset_size)
			return false;

		const CustomBitset& a = dynamic_cast<const CustomBitset&>(b);
		unsigned int n = (this->bitset_size / memory_size) + 1;
		bool equal = true;
		return std::equal(a.bitset_value.begin(), a.bitset_value.end(), this->bitset_value.begin());
	};

	// copy assignment
	CustomBitset& operator=(const CustomBitset& other)
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
};

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- //
// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- //

class DynamicBitset : public Bitset
{
private:
	boost::dynamic_bitset<> bitset_value;

public:
	DynamicBitset()
	{
		this->bitset_size = 0;
	};

	DynamicBitset(unsigned int bitsetSize) : bitset_value(bitset_size)
	{
		this->bitset_size = bitsetSize;		
	};

	DynamicBitset(const DynamicBitset& bitset) 
	{
		this->bitset_size = bitset.size();
		this->bitset_value = bitset.bitset_value;
	};

	void reset(bool b = false)
	{
		bitset_value.reset();
	};

	void set(unsigned int iAttribute, bool b = true)
	{
		this->bitset_value[iAttribute] = b;
	};

	bool get(unsigned int iAttribute) const
	{
		assert(iAttribute < size());
		return this->bitset_value[iAttribute];
	};

	unsigned int size() const
	{
		return this->bitset_size;
	};

	unsigned int count() const
	{
		return static_cast<unsigned int>(this->bitset_value.count());
	};

	Bitset& bitset_or(const Bitset& b) override
	{
		const DynamicBitset& a = dynamic_cast<const DynamicBitset&>(b);
		this->bitset_value = this->bitset_value | a.bitset_value;
		return *this;
	};

	Bitset& bitset_and(const Bitset& b) override
	{
		const DynamicBitset& a = dynamic_cast<const DynamicBitset&>(b);
		this->bitset_value = this->bitset_value & a.bitset_value;
		return *this;
	};

	bool bitset_compare(const Bitset& b) const override
	{
		if (b.size() != this->bitset_size)
			return false;

		const DynamicBitset& a = dynamic_cast<const DynamicBitset&>(b);
		return a.bitset_value == this->bitset_value;
	};

	// copy assignment
	DynamicBitset& operator=(const DynamicBitset& other)
	{
		if (this != &other)
		{
			const DynamicBitset& a = dynamic_cast<const DynamicBitset&>(other);
			this->bitset_value = a.bitset_value;
		}
		return *this;
	}
};
