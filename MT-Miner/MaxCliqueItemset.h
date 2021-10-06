#pragma once
#include "Itemset.h"

class BinaryRepresentation;

#define ISESSENTIAL_ON_TOEXPLORE

class MaxCliqueItemset : public Itemset
{
public:	
	MaxCliqueItemset();
	MaxCliqueItemset(unsigned int binaryRepIndex);
	MaxCliqueItemset(const std::shared_ptr<Itemset>& itemset);
	~MaxCliqueItemset();
	
	// pure virtual interfaces	
	std::shared_ptr<Itemset> createAndReplaceItem(unsigned int i, Item* item) override;
	void combineItemset(const std::shared_ptr<Itemset>& itemset_right) override;
};
