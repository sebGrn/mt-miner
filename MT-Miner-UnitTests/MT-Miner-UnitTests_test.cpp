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
			Assert::AreEqual(6, objectCount);
			Assert::AreEqual(8, itemCount);

			// allocate miner
			MT_Miner<StaticBitset<std::bitset<SIZE_0>>> miner(hypergraph, true);

			Itemset item1{ 1 };
			int disjonctifSupport = miner.computeDisjonctifSupport(item1);
			Assert::AreEqual(1, disjonctifSupport);

			Itemset item2{ 1, 2 };
			disjonctifSupport = miner.computeDisjonctifSupport(item2);
			Assert::AreEqual(2, disjonctifSupport);

			Itemset item3{ 1, 2, 3 };
			disjonctifSupport = miner.computeDisjonctifSupport(item3);
			Assert::AreEqual(3, disjonctifSupport);

			Itemset item4{ 1, 2, 3, 4 };
			disjonctifSupport = miner.computeDisjonctifSupport(item4);
			Assert::AreEqual(4, disjonctifSupport);
		}

		TEST_METHOD(TestingMinimalTransversalsSize)
		{
			HypergraphParser parser;
			bool parserResult = parser.parse("../../data/test.txt");
			Assert::AreEqual(true, parserResult);

			MT_Miner<StaticBitset<std::bitset<SIZE_0>>> miner(parser.getHypergraph(), true);

			// compute minimal transversals
			std::vector<Itemset>&& minimalTransversals = miner.computeMinimalTransversals();
			//minimalTransversals = Utils::sortVectorOfItemset(minimalTransversals);
			Assert::AreEqual(6, static_cast<int>(minimalTransversals.size()));
		}

		TEST_METHOD(TestingCloneMinimalTransversalsSize_no_clone)
		{
			HypergraphParser parser;
			bool parserResult = parser.parse("../../data/test_clone.txt");
			Assert::AreEqual(true, parserResult);

			MT_Miner<StaticBitset<std::bitset<SIZE_0>>> miner(parser.getHypergraph(), false);

			// compute minimal transversals
			std::vector<Itemset>&& minimalTransversals = miner.computeMinimalTransversals();
			//minimalTransversals = Utils::sortVectorOfItemset(minimalTransversals);
			Assert::AreEqual(15, static_cast<int>(minimalTransversals.size()));
		}

		TEST_METHOD(TestingCloneMinimalTransversalsSize_clone)
		{
			HypergraphParser parser;
			bool parserResult = parser.parse("../../data/test_clone.txt");
			Assert::AreEqual(true, parserResult);

			MT_Miner<StaticBitset<std::bitset<SIZE_0>>> miner(parser.getHypergraph(), true);

			// compute minimal transversals
			std::vector<Itemset>&& minimalTransversals = miner.computeMinimalTransversals();
			//minimalTransversals = Utils::sortVectorOfItemset(minimalTransversals);
			Assert::AreEqual(15, static_cast<int>(minimalTransversals.size()));
		}
	};
}
