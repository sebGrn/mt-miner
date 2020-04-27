#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace MTMinerUnitTests_test
{
	TEST_CLASS(MTMinerUnitTests_test)
	{
	public:
		
		TEST_METHOD(TestingDisjonctifSupport)
		{
			HypergraphParser parser;
			bool parserResult = parser.parse("../../data/test.txt");
			Assert::AreEqual(parserResult, true);

			std::shared_ptr<HyperGraph> hypergraph = parser.getHypergraph();
			int objectCount = parser.getObjectCount();
			int itemCount = parser.getItemCount();
			Assert::AreEqual(objectCount, 6);
			Assert::AreEqual(itemCount, 8);

			// allocate miner
			MT_Miner miner(true);
			std::vector<Utils::Itemset> toTraverse;
			miner.init(hypergraph, toTraverse);

			Utils::Itemset item1{ 1 };
			int disjonctifSupport = miner.computeDisjonctifSupport(item1);
			Assert::AreEqual(disjonctifSupport, 1);

			Utils::Itemset item2{ 1, 2 };
			disjonctifSupport = miner.computeDisjonctifSupport(item2);
			Assert::AreEqual(disjonctifSupport, 2);

			Utils::Itemset item3{ 1, 2, 3 };
			disjonctifSupport = miner.computeDisjonctifSupport(item3);
			Assert::AreEqual(disjonctifSupport, 3);

			Utils::Itemset item4{ 1, 2, 3, 4 };
			disjonctifSupport = miner.computeDisjonctifSupport(item4);
			Assert::AreEqual(disjonctifSupport, 4);
		}

		TEST_METHOD(TestingMinimalTransversalsSize)
		{
			HypergraphParser parser;
			bool parserResult = parser.parse("../../data/test.txt");
			Assert::AreEqual(parserResult, true);

			MT_Miner miner(true);
			std::vector<Utils::Itemset> toTraverse;
			miner.init(parser.getHypergraph(), toTraverse);

			// compute minimal transversals
			std::vector<Utils::Itemset> minimalTransversals = miner.computeMinimalTransversals(toTraverse);
			//minimalTransversals = Utils::sortVectorOfItemset(minimalTransversals);
			Assert::AreEqual(static_cast<int>(minimalTransversals.size()), 6);
		}

	};
}
