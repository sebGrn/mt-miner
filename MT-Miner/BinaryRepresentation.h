#pragma once

#include <unordered_map>

#include "Itemset.h"
#include "utils.h"
#include "Item.h"
#include "FormalContext.h"

/** 
 * a binary representation is a formal context representation in columns
 * each vector contains the attributes/items boolean values
 * the index of the map is the item number
 * this representation is usefull to compute disjonctif support easyly (with a OR operator)
 */
class BinaryRepresentation
{
private:
	///  key/value definition of a binary represention (key as the attribute id, value as the bitset)
	static std::unordered_map<unsigned int, std::shared_ptr<Item>> binaryRepresentationMap;

	/// number of items/attributes/columns
	static unsigned int itemCount;

	/// number of objects/lines
	static unsigned int objectCount;

	/// count number of cloned itemsets has been removed from mt computation
	static unsigned int nbItemsetNotAddedFromClone;

public:
	/// build binary representation from formal context
	static void buildFromFormalContext(const FormalContext& context);
	///
	static unsigned int buildCloneList();
	//
	static void serialize(const std::string& outputile);

	static unsigned int getItemCount();
	static unsigned int getObjectCount();

	static std::shared_ptr<Item> getItemFromKey(unsigned int key);

	static void clear();
};

inline void BinaryRepresentation::clear()
{
	binaryRepresentationMap.clear();
}

inline unsigned int BinaryRepresentation::getItemCount()
{
	return itemCount;
};

inline unsigned int BinaryRepresentation::getObjectCount()
{
	return objectCount;
};

inline std::shared_ptr<Item> BinaryRepresentation::getItemFromKey(unsigned int key)
{
	assert(binaryRepresentationMap.find(key) != binaryRepresentationMap.end());
	return binaryRepresentationMap.at(key);
}
