#pragma once

#include "Itemset.h"

class BinaryRepresentation;

class ToExploreItemset : public Itemset
{
public:
#ifdef ISESSENTIAL_ON_TOEXPLORE
	bool isEssential;
#endif
	StaticBitset cumulatedXorbitset;
	StaticBitset noiseBitset;

private:
	static bool computeIsEssentialParameters(const std::shared_ptr<Itemset>& itemset, StaticBitset& cumulatedXorbitset, StaticBitset& noiseBitset);

public:	
	ToExploreItemset();
	ToExploreItemset(unsigned int binaryRepIndex);
	ToExploreItemset(const std::shared_ptr<Itemset>& itemset);

	void setCumulatedXorBitset(const StaticBitset& bitset);
	void setNoiseBitset(const StaticBitset& bitset);
	StaticBitset getCumulatedXorBitset() const;
	StaticBitset getNoiseBitset() const;

	std::shared_ptr<Itemset> createAndReplaceItem(unsigned int i, Item* item) override;	
	void combineItemset(const std::shared_ptr<Itemset>& itemset_right) override;

#ifdef ISESSENTIAL_ON_TOEXPLORE
	bool getIsEssential() const;
	bool setEssentiality(bool value);
#endif
	
#ifndef ISESSENTIAL_ON_TOEXPLORE
	static bool computeIsEssential(const std::shared_ptr<Itemset>& left, const std::shared_ptr<Itemset>& right);
#else
	static bool computeIsEssential(const std::shared_ptr<Itemset>& itemset, bool mtComputation = false);
#endif
	static unsigned int computeSupport(const Itemset& left, const std::shared_ptr<Itemset>& right);	
	static bool isEssentialRapid(std::shared_ptr<Itemset>& left, std::shared_ptr<Itemset>& right);
};

inline void ToExploreItemset::setCumulatedXorBitset(const StaticBitset& bitset)
{
	this->cumulatedXorbitset = bitset;
}

inline void ToExploreItemset::setNoiseBitset(const StaticBitset& bitset)
{
	this->noiseBitset = bitset;
}

inline StaticBitset ToExploreItemset::getCumulatedXorBitset() const
{
	return this->cumulatedXorbitset;
}

inline StaticBitset ToExploreItemset::getNoiseBitset() const
{
	return this->noiseBitset;
}

#ifdef ISESSENTIAL_ON_TOEXPLORE
inline bool ToExploreItemset::getIsEssential() const
{
	return this->isEssential;
}

inline bool ToExploreItemset::setEssentiality(bool value)
{
	this->isEssential = value;
}
#endif
