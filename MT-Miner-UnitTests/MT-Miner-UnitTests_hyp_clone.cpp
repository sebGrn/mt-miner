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
			HyperGraph hypergraph;
			bool parserResult = hypergraph.load("../../data/Hyp1.txt");
			Assert::AreEqual(parserResult, true);

			bool useCloneOptimization = true;
			MT_Miner miner(useCloneOptimization);
			miner.createBinaryRepresentation(hypergraph);

			// compute minimal transversals
			std::deque< std::shared_ptr<Itemset>> minimalTransversals;
			miner.computeMinimalTransversals(minimalTransversals);
			Assert::AreEqual(16, static_cast<int>(minimalTransversals.size()));
		}

		TEST_METHOD(TestingMinimalTransversalsSize_hyp2)
		{
			HyperGraph hypergraph;
			bool parserResult = hypergraph.load("../../data/Hyp2.txt");
			Assert::AreEqual(parserResult, true);

			bool useCloneOptimization = true;
			MT_Miner miner(useCloneOptimization);
			miner.createBinaryRepresentation(hypergraph);

			// compute minimal transversals
			std::deque< std::shared_ptr<Itemset>> minimalTransversals;
			miner.computeMinimalTransversals(minimalTransversals);
			Assert::AreEqual(68, static_cast<int>(minimalTransversals.size()));
		}

		TEST_METHOD(TestingMinimalTransversalsSize_hyp3)
		{
			HyperGraph hypergraph;
			bool parserResult = hypergraph.load("../../data/Hyp3.txt");
			Assert::AreEqual(parserResult, true);

			bool useCloneOptimization = true;
			MT_Miner miner(useCloneOptimization);
			miner.createBinaryRepresentation(hypergraph);

			// compute minimal transversals
			std::deque< std::shared_ptr<Itemset>> minimalTransversals;
			miner.computeMinimalTransversals(minimalTransversals);
			Assert::AreEqual(57, static_cast<int>(minimalTransversals.size()));
		}
	};
}
