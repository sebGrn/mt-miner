#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace MTMinerUnitTests_hyp_clone
{
	TEST_CLASS(MTMinerUnitTests_hyp_clone)
	{
	public:
		TEST_METHOD(TestingMinimalTransversalsSize_hyp1)
		{
			HypergraphParser parser;
			bool parserResult = parser.parse("../../data/Hyp1.txt");
			Assert::AreEqual(parserResult, true);
			Assert::IsNotNull(parser.getHypergraph().get());

			bool useCloneOptimization = true;
			MT_Miner miner(useCloneOptimization);
			std::vector<Utils::Itemset> toTraverse;
			miner.init(parser.getHypergraph(), toTraverse);

			// compute minimal transversals
			std::vector<Utils::Itemset> minimalTransversals = miner.computeMinimalTransversals(toTraverse);
			Assert::AreEqual(static_cast<int>(minimalTransversals.size()), 16);
		}

		TEST_METHOD(TestingMinimalTransversalsSize_hyp2)
		{
			HypergraphParser parser;
			bool parserResult = parser.parse("../../data/Hyp2.txt");
			Assert::AreEqual(parserResult, true);
			Assert::IsNotNull(parser.getHypergraph().get());

			bool useCloneOptimization = true;
			MT_Miner miner(useCloneOptimization);
			std::vector<Utils::Itemset> toTraverse;
			miner.init(parser.getHypergraph(), toTraverse);

			// compute minimal transversals
			std::vector<Utils::Itemset> minimalTransversals = miner.computeMinimalTransversals(toTraverse);
			Assert::AreEqual(static_cast<int>(minimalTransversals.size()), 68);
		}

		TEST_METHOD(TestingMinimalTransversalsSize_hyp3)
		{
			HypergraphParser parser;
			bool parserResult = parser.parse("../../data/Hyp3.txt");
			Assert::AreEqual(parserResult, true);
			Assert::IsNotNull(parser.getHypergraph().get());

			bool useCloneOptimization = true;
			MT_Miner miner(useCloneOptimization);
			std::vector<Utils::Itemset> toTraverse;
			miner.init(parser.getHypergraph(), toTraverse);

			// compute minimal transversals
			std::vector<Utils::Itemset> minimalTransversals = miner.computeMinimalTransversals(toTraverse);
			Assert::AreEqual(static_cast<int>(minimalTransversals.size()), 57);
		}
	};
}
