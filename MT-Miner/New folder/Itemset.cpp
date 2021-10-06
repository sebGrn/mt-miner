#include "Itemset.h"
#include <unordered_map>
#include <bit>
#include "BinaryRepresentation.h"

Itemset::ItemsetType Itemset::itemsetType = Itemset::ItemsetType::DISJUNCTIVE;

Itemset::~Itemset()
{
	this->itemsetIndexVector.clear();
}

Item* Itemset::getItem(const std::shared_ptr<Itemset>& itemset, unsigned int i)
{
	assert(i < itemset->itemsetIndexVector.size());
	return BinaryRepresentation::getItemFromKey(itemset->itemsetIndexVector[i]).get();
}

unsigned int Itemset::computeSupport(const Itemset& left, const std::shared_ptr<Itemset>& right)
{
	StaticBitset res;
	if (itemsetType == CONSJONCTIVE)
		res = left.supportBitset & right->supportBitset;
	else
		res = left.supportBitset | right->supportBitset;
	unsigned int r = res.count();
	return r;
}

bool Itemset::operator==(const Itemset& other)
{
	// first test if support is different, itemsets are differents

	if (this->supportValue != other.supportValue)
		return false;

	// test each attributeIndex of itemset
	unsigned int i = other.itemsetIndexVector[0];
	unsigned int otherAttributeIndex = BinaryRepresentation::getItemFromKey(i).get()->getAttributeIndex();

	for(auto it2 = this->itemsetIndexVector.begin(); it2 != this->itemsetIndexVector.end(); it2++)
	{
		unsigned int attributeIndex = BinaryRepresentation::getItemFromKey(*it2).get()->getAttributeIndex();
		if (otherAttributeIndex != attributeIndex)
			return false;
	}
	return true;
}

std::string Itemset::toString() const
{
	std::string res = "{";
	for_each(this->itemsetIndexVector.begin(), this->itemsetIndexVector.end(), [&](unsigned int i) {
		Item* item = BinaryRepresentation::getItemFromKey(i).get();
		res += std::to_string(item->getAttributeIndex());
		res += ",";
		});
	res.pop_back();
	res += "}";
	return res;
}

void Itemset::flip()
{
	if (itemsetType == CONSJONCTIVE)
	{
		for (auto& elt : itemsetIndexVector)
		{
			Item* item = BinaryRepresentation::getItemFromKey(elt).get();
			item->staticBitset->flip();
		}
		this->supportBitset.flip();
		this->supportValue = this->supportBitset.count();
	}
}

