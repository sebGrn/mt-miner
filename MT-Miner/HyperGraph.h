#pragma once
#include <vector>
#include <cassert>
#include <algorithm>
#include <chrono>
#include <iostream>
#include <fstream>

#include "Logger.h"
#include "utils.h"

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
	{
		reset();
	};

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

	void reset()
	{
		this->objectCount = 0;
		this->itemCount = 0;
		this->oneBasedIndex = true;
		for (unsigned int i = 0; i < hypergraph.size(); i++)
			hypergraph[i].clear();
		hypergraph.clear();
	};

	bool load(const std::string& file)
	{
		this->reset();

		std::ifstream inputStream;
		inputStream.open(file);
		if (inputStream.fail())
		{
			std::cout << RED << "couldn't load file " << file << RESET << std::endl;
			return false;
		}

		unsigned int maxItemCount = 0;
		unsigned int objectCount = 0;

		std::cout << GREEN << "parsing " << file << "\n" << RESET;
		auto beginTime = std::chrono::system_clock::now();

		bool oneIndexedBase = true;

		std::string line;
		while (std::getline(inputStream, line))
		{
			if (!line.empty())
			{
				line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
				line = Utils::trim(line);
				std::vector<unsigned int> data = Utils::splitToVectorOfInt(line, ' ');
				maxItemCount = std::max(*std::max_element(data.begin(), data.end()), maxItemCount);

				this->addLine(data);

				// check if file is zero indexed
				if (oneIndexedBase && (std::find(data.begin(), data.end(), 0) != data.end()))
					oneIndexedBase = false;

				// as many items as lines in the file
				objectCount++;
			}
		}
		this->setItemCount(maxItemCount);
		this->setObjectCount(objectCount);
		this->setOneBasedIndex(oneIndexedBase);

		if (!oneIndexedBase)
			Logger::log("hypergraph has a zero based index mode\n");

		inputStream.close();

		int64_t duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - beginTime).count();
		Logger::log("parsing hypergraph done in ", duration, " ms, found ", maxItemCount, " items (lines) and ", objectCount, " objects (columns)\n");

		return true;
	};

};