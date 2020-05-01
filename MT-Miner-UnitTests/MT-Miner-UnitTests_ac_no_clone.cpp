#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace MTMinerUnitTests_ac_no_clone
{
	TEST_CLASS(MTMinerUnitTests_ac_no_clone)
	{
	public:
		TEST_METHOD(TestingMinimalTransversalsSize_ac_130k)
		{
			HypergraphParser parser;
			bool parserResult = parser.parse("../../data/ac_130k.dat");
			Assert::AreEqual(parserResult, true);
			Assert::IsNotNull(parser.getHypergraph().get());

			bool useCloneOptimization = false;
			MT_Miner miner(useCloneOptimization);
			std::vector<Utils::Itemset> toTraverse;
			miner.init(parser.getHypergraph(), toTraverse);

			// compute minimal transversals
			std::vector<Utils::Itemset>&& minimalTransversals = miner.computeMinimalTransversals(std::move(toTraverse));
			Assert::AreEqual(1916, static_cast<int>(minimalTransversals.size()));
		}

		TEST_METHOD(TestingMinimalTransversalsSize_ac_150k)
		{
			HypergraphParser parser;
			bool parserResult = parser.parse("../../data/ac_150k.dat");
			Assert::AreEqual(parserResult, true);
			Assert::IsNotNull(parser.getHypergraph().get());

			bool useCloneOptimization = false;
			MT_Miner miner(useCloneOptimization);
			std::vector<Utils::Itemset> toTraverse;
			miner.init(parser.getHypergraph(), toTraverse);

			// compute minimal transversals
			std::vector<Utils::Itemset>&& minimalTransversals = miner.computeMinimalTransversals(std::move(toTraverse));
			Assert::AreEqual(1039, static_cast<int>(minimalTransversals.size()));
		}

		TEST_METHOD(TestingMinimalTransversalsSize_ac_200k)
		{
			HypergraphParser parser;
			bool parserResult = parser.parse("../../data/ac_200k.dat");
			Assert::AreEqual(parserResult, true);
			Assert::IsNotNull(parser.getHypergraph().get());

			bool useCloneOptimization = false;
			MT_Miner miner(useCloneOptimization);
			std::vector<Utils::Itemset> toTraverse;
			miner.init(parser.getHypergraph(), toTraverse);

			// compute minimal transversals
			std::vector<Utils::Itemset>&& minimalTransversals = miner.computeMinimalTransversals(std::move(toTraverse));
			Assert::AreEqual(253, static_cast<int>(minimalTransversals.size()));
		}
	};
}
