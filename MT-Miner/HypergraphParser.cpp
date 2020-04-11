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
		std::cout << "couldn't load file " << file << std::endl;
		return false;
	}

	unsigned int maxItemCount = 0;
	unsigned int objectCount = 0;
	hypergraph = std::make_shared<HyperGraph>();

	std::string line;
	while (std::getline(inputStream, line))
	{
		if (!line.empty())
		{
			line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
			line = trim(line);
			Itemset data = splitToVectorOfInt(line, ' ');
			maxItemCount = std::max(*std::max_element(data.begin(), data.end()), maxItemCount);

			hypergraph->addLine(data);

			// as many items as lines in the file
			objectCount++;
		}
	}
	hypergraph->setItemCount(maxItemCount);
	hypergraph->setObjectCount(objectCount);
		
	inputStream.close();
	return true;
}
