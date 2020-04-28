#include "HypergraphParser.h"
#include "utils.h"

// -------------------------------------------------------------------------------------------------------------------------------------- //

HypergraphParser::HypergraphParser()
{
	this->parsingDone = false;
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
		
	// lambda function called during parsing every minutes
	auto callback = [](bool& done, unsigned int& objectCount) {
		int n = 0;
		while (!done)
		{
			if (n)
				std::cout << "parsing in progress : " << objectCount << " objects found after " << 10*n << " seconds" << std::endl;
			std::this_thread::sleep_for(std::chrono::seconds(10));
			n++;
		}
	};

	Logger::log("parsing ", file, "\n");
	auto beginTime = std::chrono::system_clock::now();

	this->parsingDone = false;
	// instanciate new thead for regular log
	std::thread thread(callback, std::ref(this->parsingDone), std::ref(objectCount));

	std::string line;
	while (std::getline(inputStream, line))
	{
		if (!line.empty())
		{
			line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());
			line = Utils::trim(line);
			Itemset data = Utils::splitToVectorOfInt(line, ' ');
			maxItemCount = std::max(*std::max_element(data.begin(), data.end()), maxItemCount);

			hypergraph->addLine(data);

			// as many items as lines in the file
			objectCount++;
		}
	}
	hypergraph->setItemCount(maxItemCount);
	hypergraph->setObjectCount(objectCount);
	this->parsingDone = true;
	inputStream.close();
	
	thread.detach();

	int64_t duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - beginTime).count();
	Logger::log("parsing hypergraph done in ", duration, " ms, found ", maxItemCount, " items (columns) and ", objectCount, " objects (lines)\n\n");
	
	return true;
}
