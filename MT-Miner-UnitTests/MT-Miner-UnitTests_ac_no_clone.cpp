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
			HyperGraph hypergraph;
			bool parserResult = hypergraph.load("../../data/ac_130k.dat");
			Assert::AreEqual(parserResult, true);
			
			bool useCloneOptimization = false;
			MT_Miner miner(useCloneOptimization);
			miner.createBinaryRepresentation(hypergraph);
			
			// compute minimal transversals
			std::deque<std::shared_ptr<Itemset>> minimalTransversals;
			miner.computeMinimalTransversals(minimalTransversals);
			Assert::AreEqual(1916, static_cast<int>(minimalTransversals.size()));
		}

		TEST_METHOD(TestingMinimalTransversalsSize_ac_150k)
		{
			HyperGraph hypergraph;
			bool parserResult = hypergraph.load("../../data/ac_150k.dat");
			Assert::AreEqual(parserResult, true);
			
			bool useCloneOptimization = false;
			MT_Miner miner(useCloneOptimization);
			miner.createBinaryRepresentation(hypergraph);

			// compute minimal transversals
			std::deque< std::shared_ptr<Itemset>> minimalTransversals;
			miner.computeMinimalTransversals(minimalTransversals);
			Assert::AreEqual(1039, static_cast<int>(minimalTransversals.size()));
		}

		TEST_METHOD(TestingMinimalTransversalsSize_ac_200k)
		{
			HyperGraph hypergraph;
			bool parserResult = hypergraph.load("../../data/ac_200k.dat");
			Assert::AreEqual(parserResult, true);
			
			bool useCloneOptimization = false;
			MT_Miner miner(useCloneOptimization);
			miner.createBinaryRepresentation(hypergraph);

			// compute minimal transversals
			std::deque< std::shared_ptr<Itemset>> minimalTransversals;
			miner.computeMinimalTransversals(minimalTransversals);
			Assert::AreEqual(253, static_cast<int>(minimalTransversals.size()));
		}
	};
}
