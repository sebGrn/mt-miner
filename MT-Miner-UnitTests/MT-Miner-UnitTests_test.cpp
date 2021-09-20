#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace MTMinerUnitTests_test
{
	TEST_CLASS(MTMinerUnitTests_test)
	{
	public:
		
		TEST_METHOD(TestingMinimalTransversalsSize_accident)
		{
			HyperGraph hypergraph;
			bool parserResult = hypergraph.load("../../data/ac_130k.dat");
			Assert::AreEqual(true, parserResult);

			MT_Miner miner(true);
			miner.createBinaryRepresentation(hypergraph);

			// compute minimal transversals
			std::deque< std::shared_ptr<Itemset>> minimalTransversals;
			miner.computeMinimalTransversals(minimalTransversals);
			Assert::AreEqual(1916, static_cast<int>(minimalTransversals.size()));
		}
		TEST_METHOD(TestingMinimalTransversalsSize)
		{
			HyperGraph hypergraph;
			bool parserResult = hypergraph.load("../../data/simple.txt");
			Assert::AreEqual(true, parserResult);

			MT_Miner miner(true);
			miner.createBinaryRepresentation(hypergraph);

			// compute minimal transversals
			std::deque< std::shared_ptr<Itemset>> minimalTransversals;
			miner.computeMinimalTransversals(minimalTransversals);
			//minimalTransversals = Utils::sortVectorOfItemset(minimalTransversals);
			Assert::AreEqual(6, static_cast<int>(minimalTransversals.size()));
		}

		TEST_METHOD(TestingCloneMinimalTransversalsSize_no_clone)
		{
			HyperGraph hypergraph;
			bool parserResult = hypergraph.load("../../data/test_clone.txt");
			Assert::AreEqual(true, parserResult);

			MT_Miner miner(false);
			miner.createBinaryRepresentation(hypergraph);

			// compute minimal transversals
			std::deque<std::shared_ptr<Itemset>> minimalTransversals;
			miner.computeMinimalTransversals(minimalTransversals);
			//minimalTransversals = Utils::sortVectorOfItemset(minimalTransversals);
			Assert::AreEqual(15, static_cast<int>(minimalTransversals.size()));
		}

		TEST_METHOD(TestingCloneMinimalTransversalsSize_clone)
		{
			HyperGraph hypergraph;
			bool parserResult = hypergraph.load("../../data/test_clone.txt");
			Assert::AreEqual(true, parserResult);

			MT_Miner miner(true);
			miner.createBinaryRepresentation(hypergraph);

			// compute minimal transversals
			std::deque< std::shared_ptr<Itemset>> minimalTransversals;
			miner.computeMinimalTransversals(minimalTransversals);
			//minimalTransversals = Utils::sortVectorOfItemset(minimalTransversals);
			Assert::AreEqual(15, static_cast<int>(minimalTransversals.size()));
		}

		//TEST_METHOD(TestingIsEssential)
		//{
		//	std::shared_ptr<Item> item1 = std::make_shared<Item>(1, 5);
		//	item1->staticBitset.set(0);	item1->sparseBitset.set(0, true);
		//	item1->staticBitset.set(3); item1->sparseBitset.set(3, true);

		//	std::shared_ptr<Item> item2 = std::make_shared<Item>(2, 5);
		//	item2->staticBitset.set(0); item2->sparseBitset.set(0, true);
		//	item2->staticBitset.set(1); item2->sparseBitset.set(1, true);

		//	std::shared_ptr<Item> item3 = std::make_shared<Item>(3, 5);
		//	item3->staticBitset.set(2); item3->sparseBitset.set(2, true);

		//	std::shared_ptr<Item> item4 = std::make_shared<Item>(4, 5);
		//	item4->staticBitset.set(4); item4->sparseBitset.set(1, true);	 // this one is false
		//	item4->staticBitset.set(4); item4->sparseBitset.set(4, true);

		//	std::shared_ptr<Itemset> itemset = std::make_shared<Itemset>();
		//	itemset->addItem(item1);
		//	itemset->addItem(item2);
		//	itemset->addItem(item3);
		//	itemset->addItem(item4);

		//	Assert::AreEqual(false, itemset->isEssential);
		//}
	};
}
