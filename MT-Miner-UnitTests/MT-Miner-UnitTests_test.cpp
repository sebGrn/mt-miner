#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace MTMinerUnitTests_test
{
	TEST_CLASS(MTMinerUnitTests_test)
	{
	public:
		
		/*TEST_METHOD(TestingDisjonctifSupport)
		{
			HyperGraph hypergraph;
			bool parserResult = hypergraph.load("../../data/test.txt");
			Assert::AreEqual(parserResult, true);
			
			int objectCount = hypergraph.getObjectCount();
			int itemCount = hypergraph.getItemCount();
			Assert::AreEqual(6, objectCount);
			Assert::AreEqual(8, itemCount);

			MT_Miner miner(true);
			miner.createBinaryRepresentation(hypergraph);

			// allocate miner
			std::shared_ptr<Itemset> item1 = std::make_shared<Itemset>();
			item1->addItem(BinaryRepresentation::getItemFromKey(1));
			int disjonctifSupport = item1->getDisjunctifSupport();
			Assert::AreEqual(1, disjonctifSupport);

			std::shared_ptr<Itemset> item2 = std::make_shared<Itemset>();
			item2->addItem(BinaryRepresentation::getItemFromKey(1));
			item2->addItem(BinaryRepresentation::getItemFromKey(2));
			disjonctifSupport = item2->getDisjunctifSupport();
			Assert::AreEqual(2, disjonctifSupport);

			std::shared_ptr<Itemset> item3 = std::make_shared<Itemset>();
			item3->addItem(BinaryRepresentation::getItemFromKey(1));
			item3->addItem(BinaryRepresentation::getItemFromKey(2));
			item3->addItem(BinaryRepresentation::getItemFromKey(3));
			disjonctifSupport = item3->getDisjunctifSupport();
			Assert::AreEqual(3, disjonctifSupport);

			std::shared_ptr<Itemset> item4 = std::make_shared<Itemset>();
			item4->addItem(BinaryRepresentation::getItemFromKey(1));
			item4->addItem(BinaryRepresentation::getItemFromKey(2));
			item4->addItem(BinaryRepresentation::getItemFromKey(3));
			item4->addItem(BinaryRepresentation::getItemFromKey(4));
			disjonctifSupport = item4->getDisjunctifSupport();
			Assert::AreEqual(4, disjonctifSupport);
		}*/

		TEST_METHOD(TestingMinimalTransversalsSize)
		{
			HyperGraph hypergraph;
			bool parserResult = hypergraph.load("../../data/test.txt");
			Assert::AreEqual(true, parserResult);

			MT_Miner miner(true);
			miner.createBinaryRepresentation(hypergraph);

			// compute minimal transversals
			std::vector< std::shared_ptr<Itemset>> minimalTransversals;
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
			std::vector< std::shared_ptr<Itemset>> minimalTransversals;
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
			std::vector< std::shared_ptr<Itemset>> minimalTransversals;
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
