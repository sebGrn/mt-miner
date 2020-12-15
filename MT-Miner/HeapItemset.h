#pragma once

#include "Item.h"

class HeapItemset
{
	StaticBitset* itemset;

public:
	bool dirty;
	StaticBitset orValue;
	unsigned int orSupport;
	bool hasClone;

	bool isEssential;
	StaticBitset isEssentialADNBitset;
	StaticBitset markedNonEssentialBisetIndex;
	StaticBitset temporaryBitset;

public:
	HeapItemset(unsigned int itemset_size);
	

	~HeapItemset();


};

