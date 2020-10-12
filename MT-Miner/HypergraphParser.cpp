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

			hypergraph->addLine(data);

			// check if file is zero indexed
			if (oneIndexedBase && (std::find(data.begin(), data.end(), 0) != data.end()))
				oneIndexedBase = false;

			// as many items as lines in the file
			objectCount++;
		}
	}
	hypergraph->setItemCount(maxItemCount);
	hypergraph->setObjectCount(objectCount);
	hypergraph->setOneBasedIndex(oneIndexedBase);

	if(!oneIndexedBase)
		Logger::log("hypergraph has a zero based index mode\n");

	inputStream.close();
	
	int64_t duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - beginTime).count();
	Logger::log("parsing hypergraph done in ", duration, " ms, found ", maxItemCount, " items (lines) and ", objectCount, " objects (columns)\n");
	
	return true;
}
