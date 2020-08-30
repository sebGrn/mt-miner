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

			MT_Miner miner(true);
			miner.createBinaryRepresentation(hypergraph);

			// allocate miner
			std::shared_ptr<Itemset> item1 = std::make_shared<Itemset>();
			item1->addItem(BinaryRepresentation::getItemFromKey(1));
			int disjonctifSupport = BinaryRepresentation::computeDisjonctifSupport(item1);
			Assert::AreEqual(1, disjonctifSupport);

			std::shared_ptr<Itemset> item2 = std::make_shared<Itemset>();
			item2->addItem(BinaryRepresentation::getItemFromKey(1));
			item2->addItem(BinaryRepresentation::getItemFromKey(2));
			disjonctifSupport = BinaryRepresentation::computeDisjonctifSupport(item2);
			Assert::AreEqual(2, disjonctifSupport);

			std::shared_ptr<Itemset> item3 = std::make_shared<Itemset>();
			item3->addItem(BinaryRepresentation::getItemFromKey(1));
			item3->addItem(BinaryRepresentation::getItemFromKey(2));
			item3->addItem(BinaryRepresentation::getItemFromKey(3));
			disjonctifSupport = BinaryRepresentation::computeDisjonctifSupport(item3);
			Assert::AreEqual(3, disjonctifSupport);

			std::shared_ptr<Itemset> item4 = std::make_shared<Itemset>();
			item4->addItem(BinaryRepresentation::getItemFromKey(1));
			item4->addItem(BinaryRepresentation::getItemFromKey(2));
			item4->addItem(BinaryRepresentation::getItemFromKey(3));
			item4->addItem(BinaryRepresentation::getItemFromKey(4));
			disjonctifSupport = BinaryRepresentation::computeDisjonctifSupport(item4);
			Assert::AreEqual(4, disjonctifSupport);
		}

		TEST_METHOD(TestingMinimalTransversalsSize)
		{
			HypergraphParser parser;
			bool parserResult = parser.parse("../../data/test.txt");
			Assert::AreEqual(true, parserResult);

			MT_Miner miner(true);
			miner.createBinaryRepresentation(parser.getHypergraph());

			// compute minimal transversals
			std::vector<std::shared_ptr<Itemset>>&& minimalTransversals = miner.computeMinimalTransversals();
			//minimalTransversals = Utils::sortVectorOfItemset(minimalTransversals);
			Assert::AreEqual(6, static_cast<int>(minimalTransversals.size()));
		}

		TEST_METHOD(TestingCloneMinimalTransversalsSize_no_clone)
		{
			HypergraphParser parser;
			bool parserResult = parser.parse("../../data/test_clone.txt");
			Assert::AreEqual(true, parserResult);

			MT_Miner miner(false);
			miner.createBinaryRepresentation(parser.getHypergraph());

			// compute minimal transversals
			std::vector<std::shared_ptr<Itemset>>&& minimalTransversals = miner.computeMinimalTransversals();
			//minimalTransversals = Utils::sortVectorOfItemset(minimalTransversals);
			Assert::AreEqual(15, static_cast<int>(minimalTransversals.size()));
		}

		TEST_METHOD(TestingCloneMinimalTransversalsSize_clone)
		{
			HypergraphParser parser;
			bool parserResult = parser.parse("../../data/test_clone.txt");
			Assert::AreEqual(true, parserResult);

			MT_Miner miner(true);
			miner.createBinaryRepresentation(parser.getHypergraph());

			// compute minimal transversals
			std::vector<std::shared_ptr<Itemset>>&& minimalTransversals = miner.computeMinimalTransversals();
			//minimalTransversals = Utils::sortVectorOfItemset(minimalTransversals);
			Assert::AreEqual(15, static_cast<int>(minimalTransversals.size()));
		}
	};
}
