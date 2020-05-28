#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace MTMinerUnitTests_CustomBitset
{
	TEST_CLASS(MTMinerUnitTests_CustomBitset)
	{
	public:
		TEST_METHOD(TestingBitsets_equal)
		{
			CustomBitset b0(1000);
			CustomBitset b1(1000);

			srand(time(nullptr));
			for (int i = 0; i < 1000; i++)
			{
				bool b = rand() % 2;
				b0.set(i, b);
				b1.set(i, b);
			}

			bool equal = (b0.bitset_compare(b1));
			Assert::AreEqual(true, equal);
		}

		TEST_METHOD(TestingBitsets_or)
		{
			CustomBitset b0(4);
			CustomBitset b1(4);
			CustomBitset b2(4);

			b0.set(0, true);  
			b0.set(1, false);
			b0.set(2, false);
			b0.set(3, true);

			b1.set(0, false);
			b1.set(1, false);
			b1.set(2, true);
			b1.set(3, true);

			b2 = b0;
			b2.bitset_or(b1);

			Assert::AreEqual(true, b2.get(0));
			Assert::AreEqual(false, b2.get(1));
			Assert::AreEqual(true, b2.get(2));
			Assert::AreEqual(true, b2.get(3));
		}

		TEST_METHOD(TestingBitsets_and)
		{
			CustomBitset b0(4);
			CustomBitset b1(4);
			CustomBitset b2(4);

			b0.set(0, true);
			b0.set(1, false);
			b0.set(2, false);
			b0.set(3, true);

			b1.set(0, false);
			b1.set(1, false);
			b1.set(2, true);
			b1.set(3, true);

			b2 = b0;
			b2.bitset_and(b1);

			Assert::AreEqual(false, b2.get(0));
			Assert::AreEqual(false, b2.get(1));
			Assert::AreEqual(false, b2.get(2));
			Assert::AreEqual(true, b2.get(3));
		}

		TEST_METHOD(TestingBitsets_count)
		{
			CustomBitset b(1000);
			unsigned int sum_set = 0;
			srand(time(nullptr));
			for (int i = 0; i < 1000; i++)
			{
				bool v = rand() % 2;
				b.set(i, v);
				if (v)
					sum_set++;
			}

			Assert::AreEqual(sum_set, b.count());
		}
	};
}
