#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace MTMinerUnitTests_hyp_no_clone
{
	TEST_CLASS(MTMinerUnitTests_hyp_no_clone)
	{
	public:
		TEST_METHOD(TestingMinimalTransversalsSize_hyp1)
		{
			HypergraphParser parser;
			bool parserResult = parser.parse("../../data/Hyp1.txt");
			Assert::AreEqual(parserResult, true);
			Assert::IsNotNull(parser.getHypergraph().get());

			bool useCloneOptimization = false;
			MT_Miner miner(parser.getHypergraph(), useCloneOptimization);

			// compute minimal transversals
			std::vector<Itemset> minimalTransversals = miner.computeMinimalTransversals();
			Assert::AreEqual(16, static_cast<int>(minimalTransversals.size()));
		}

		TEST_METHOD(TestingMinimalTransversalsSize_hyp2)
		{
			HypergraphParser parser;
			bool parserResult = parser.parse("../../data/Hyp2.txt");
			Assert::AreEqual(parserResult, true);
			Assert::IsNotNull(parser.getHypergraph().get());

			bool useCloneOptimization = false;
			MT_Miner miner(parser.getHypergraph(), useCloneOptimization);

			// compute minimal transversals
			std::vector<Itemset> minimalTransversals = miner.computeMinimalTransversals();
			Assert::AreEqual(68, static_cast<int>(minimalTransversals.size()));
		}

		TEST_METHOD(TestingMinimalTransversalsSize_hyp3)
		{
			HypergraphParser parser;
			bool parserResult = parser.parse("../../data/Hyp3.txt");
			Assert::AreEqual(parserResult, true);
			Assert::IsNotNull(parser.getHypergraph().get());

			bool useCloneOptimization = false;
			MT_Miner miner(parser.getHypergraph(), useCloneOptimization);

			// compute minimal transversals
			std::vector<Itemset> minimalTransversals = miner.computeMinimalTransversals();
			Assert::AreEqual(57, static_cast<int>(minimalTransversals.size()));
		}
	};
}
