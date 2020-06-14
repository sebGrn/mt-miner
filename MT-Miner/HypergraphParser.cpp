#include "HypergraphParser.h"
#include "utils.h"

// -------------------------------------------------------------------------------------------------------------------------------------- //

HypergraphParser::HypergraphParser()
{
}

HypergraphParser::~HypergraphParser()
{
}

bool HypergraphParser::parse(const std::string& file)
{
	hypergraph.reset();

	std::ifstream inputStream = std::ifstream();
	inputStream.open(file);
	if (inputStream.fail())
	{ 
		std::cout << RED << "couldn't load file " << file << RESET << std::endl;
		return false;
	}

	unsigned int maxItemCount = 0;
	unsigned int objectCount = 0;
	hypergraph = std::make_shared<HyperGraph>();
		
	Logger::log(GREEN, "parsing ", file, "\n", RESET);
	auto beginTime = std::chrono::system_clock::now();

	bool oneIndexedBase = true;

	std::string line;
	while (std::getline(inputStream, line))
	{
		if (!line.empty())
		{
			line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
			line = Utils::trim(line);
			Itemset data;
			data.itemset_list = Utils::splitToVectorOfInt(line, ' ');
			maxItemCount = std::max(*std::max_element(data.itemset_list.begin(), data.itemset_list.end()), maxItemCount);

			hypergraph->addLine(data.itemset_list);

			if (oneIndexedBase && Itemset::containsZero(data))
				oneIndexedBase = false;

			// as many items as lines in the file
			objectCount++;
		}
	}
	hypergraph->setItemCount(maxItemCount);
	hypergraph->setObjectCount(objectCount);
	hypergraph->setOneBasedIndex(oneIndexedBase);

	if(!oneIndexedBase)
		Logger::log(RED, "hypergraph has a zero based index mode\n", RESET);

	inputStream.close();
	
	int64_t duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - beginTime).count();
	Logger::log(GREEN, "parsing hypergraph done in ", duration, " ms, found ", maxItemCount, " items (columns) and ", objectCount, " objects (lines)\n\n", RESET);
	
	return true;
}
