#include "HypergraphParser.h"
#include "utils.h"

// -------------------------------------------------------------------------------------------------------------------------------------- //

HypergraphParser::HypergraphParser()
{
	itemCount = 0;
	objectCount = 0;
}

HypergraphParser::~HypergraphParser()
{

}

bool HypergraphParser::parse(const std::string& file)
{
	assert(hypergraph.empty());

	std::ifstream inputStream = std::ifstream();
	inputStream.open(file);
	if (inputStream.fail())
	{ 
		std::cout << "couldn't load file " << file << std::endl;
		return false;
	}

	std::string line;
	while (std::getline(inputStream, line))
	{
		if (!line.empty())
		{
			line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
			line = trim(line);
			std::vector<unsigned int> data = splitToVectorOfInt(line, ' ');
			this->itemCount = std::max(*std::max_element(data.begin(), data.end()), this->itemCount);

			hypergraph.push_back(data);

			// as many items as lines in the file
			this->objectCount++;
		}
	}
		
	inputStream.close();
	return true;
}
