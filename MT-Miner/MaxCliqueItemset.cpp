#include "MaxCliqueItemset.h"
#include "BinaryRepresentation.h"

MaxCliqueItemset::MaxCliqueItemset()
{
	this->supportValue = 0;
	this->hasClone = false;
}

MaxCliqueItemset::MaxCliqueItemset(unsigned int binaryRepIndex)
{
	assert(this->itemsetIndexVector.empty());
	std::shared_ptr<Item> item = BinaryRepresentation::getItemFromKey(binaryRepIndex);
	this->supportBitset = (*item->staticBitset);
	this->supportValue = item->count();
	this->hasClone = false;
	if (item->isAClone())
		this->hasClone = true;
	// copy list of itemset indexes from itemset
	this->itemsetIndexVector.push_back(binaryRepIndex);
}

MaxCliqueItemset::MaxCliqueItemset(const std::shared_ptr<Itemset>& itemset)
{
	assert(this->itemsetIndexVector.empty());
	this->supportBitset = itemset->getSupportBitset();
	this->supportValue = itemset->getSupport();
	this->hasClone = itemset->containsAClone();
	// copy list of itemset indexes from itemset
	itemset->copyItemset(this->itemsetIndexVector);
}

// make a copy of currentItemset and replace ith item by clone item
std::shared_ptr<Itemset> MaxCliqueItemset::createAndReplaceItem(unsigned int iToReplace, Item* itemToReplace)
{
	try
	{
		std::shared_ptr<MaxCliqueItemset> clonedItemset(new MaxCliqueItemset());
		if (clonedItemset)
		{
			clonedItemset->setSupportBitset(this->supportBitset, this->supportValue);
			clonedItemset->setCloneValue(this->hasClone);

			for (unsigned int i = 0; i < this->getItemsetSize(); i++)
			{
				if (iToReplace == i)
				{
					clonedItemset->itemsetIndexVector.push_back(itemToReplace->attributeIndex);
					if (itemToReplace->isAClone())
						clonedItemset->hasClone = true;
				}
				else
				{
					clonedItemset->itemsetIndexVector.push_back(this->itemsetIndexVector[i]);
				}
			}
		}
		return clonedItemset;
	}
	catch (std::exception& e)
	{
		std::cout << "during createAndReplaceItem " << e.what() << std::endl;
	}
	return nullptr;
}

void MaxCliqueItemset::combineItemset(const std::shared_ptr<Itemset>& itemset_right)
{
	// "1" + "2" => "12"
	// "71" + "72" => "712"
	// we can always add the last one
	unsigned int lastIndex = itemset_right->getItemsetSize() - 1;
	unsigned int indexItemToAdd = itemset_right->getItemsetFromIndex(lastIndex);
	Item* itItemToAdd = Itemset::getItem(itemset_right, indexItemToAdd);

	// update support
	if (itemsetType == CONSJONCTIVE)
		this->supportBitset = (*itItemToAdd->staticBitset) & this->getSupportBitset();
	else
		this->supportBitset = (*itItemToAdd->staticBitset) | this->getSupportBitset();

	// update clone status
	if (itItemToAdd->isAClone())
		this->hasClone = true;

	// update support
	this->supportValue = this->supportBitset.count();

	// finally add the last item
	this->itemsetIndexVector.push_back(indexItemToAdd);
}