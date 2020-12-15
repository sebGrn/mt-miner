#include "HeapItemset.h"

HeapItemset::HeapItemset(unsigned int itemset_size)
{
	this->itemset = new StaticBitset[itemset_size];
	this->dirty = true;
	this->orSupport = 0;
	hasClone = false;
	isEssential = true;
};

HeapItemset::~HeapItemset()
{
	if (this->itemset)
		delete this->itemset;
}

//void updateIsEssential(const std::shared_ptr<Item>& item)
//{
//
//}