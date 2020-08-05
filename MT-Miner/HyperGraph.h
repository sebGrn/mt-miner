#pragma once
#include <vector>
#include <cassert>
#include <algorithm>
#include "utils.h"
/**
 *
 *
 */
class HyperGraph
{
private:
	/// hypergraph ie matrix of int
	std::vector<std::vector<unsigned int>> hypergraph;

	/// number of objects/lines
	unsigned int objectCount;

	/// number of items/attributes/columns
	unsigned int itemCount;

	bool oneBasedIndex;

public:
	HyperGraph()
	{
		this->objectCount = 0;
		this->itemCount = 0;
		this->oneBasedIndex = true;
	};

	~HyperGraph()
	{};

	void addLine(const std::vector<unsigned int>& data)
	{
		this->hypergraph.push_back(data);
	};

	void setObjectCount(unsigned int nb)
	{
		this->objectCount = nb;
	};

	void setItemCount(unsigned int nb)
	{
		this->itemCount = nb;
	};

	unsigned int getObjectCount() const
	{
		return this->objectCount;
	};

	unsigned int getItemCount() const
	{
		return this->itemCount;
	};

	std::vector<unsigned int> getLine(unsigned int i) const
	{
		assert(i < getObjectCount());
		return this->hypergraph[i];
	};	

	void setOneBasedIndex(bool value)
	{
		this->oneBasedIndex = value;
	};

	bool getOneBasedIndex() const
	{
		return this->oneBasedIndex;
	};
};