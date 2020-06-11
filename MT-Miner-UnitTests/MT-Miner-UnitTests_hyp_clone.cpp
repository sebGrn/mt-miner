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
			miner.createBinaryRepresentation(parser.getHypergraph());

			// compute minimal transversals
			ItemsetList minimalTransversals = miner.computeMinimalTransversals();
			Assert::AreEqual(16, static_cast<int>(minimalTransversals.size()));
		}

		TEST_METHOD(TestingMinimalTransversalsSize_hyp2)
		{
			HypergraphParser parser;
			bool parserResult = parser.parse("../../data/Hyp2.txt");
			Assert::AreEqual(parserResult, true);
			Assert::IsNotNull(parser.getHypergraph().get());

			bool useCloneOptimization = true;
			MT_Miner miner(useCloneOptimization);
			miner.createBinaryRepresentation(parser.getHypergraph());

			// compute minimal transversals
			ItemsetList minimalTransversals = miner.computeMinimalTransversals();
			Assert::AreEqual(68, static_cast<int>(minimalTransversals.size()));
		}

		TEST_METHOD(TestingMinimalTransversalsSize_hyp3)
		{
			HypergraphParser parser;
			bool parserResult = parser.parse("../../data/Hyp3.txt");
			Assert::AreEqual(parserResult, true);
			Assert::IsNotNull(parser.getHypergraph().get());

			bool useCloneOptimization = true;
			MT_Miner miner(useCloneOptimization);
			miner.createBinaryRepresentation(parser.getHypergraph());

			// compute minimal transversals
			ItemsetList minimalTransversals = miner.computeMinimalTransversals();
			Assert::AreEqual(57, static_cast<int>(minimalTransversals.size()));
		}
	};
}
