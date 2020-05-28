#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace MTMinerUnitTests_StaticBitset
{
	TEST_CLASS(MTMinerUnitTests_StaticBitset)
	{
	public:
		TEST_METHOD(TestingBitsets_equal)
		{
			StaticBitset b0(800);
			StaticBitset b1(800);

			srand(time(nullptr));
			for (int i = 0; i < 800; i++)
			{
				bool b = rand() % 2;
				b0.set(i, b);
				b1.set(i, b);
			}

			bool equal = b0.bitset_compare(b1);
			Assert::AreEqual(true, equal);
		}

		TEST_METHOD(TestingBitsets_or)
		{
			StaticBitset b0(800);
			StaticBitset b1(800);
			StaticBitset b2(800);

			srand(time(nullptr));
			for (int i = 0; i < 800; i++)
			{
				bool b = rand() % 2;
				b0.set(i, b);
			}

			b1 = b0;
			b1.bitset_or(b0);

			bool equal = b0.bitset_compare(b1);

			Assert::AreEqual(true, equal);
		}

		TEST_METHOD(TestingBitsets_and)
		{
			StaticBitset b0(800);
			StaticBitset b1(800);
			StaticBitset b2(800);

			srand(time(nullptr));
			for (int i = 0; i < 800; i++)
			{
				bool b = rand() % 2;
				b0.set(i, b);
			}

			b1 = b0;
			b1.bitset_and(b0);

			bool equal = b0.bitset_compare(b1);

			Assert::AreEqual(true, equal);
		}

		TEST_METHOD(TestingBitsets_count)
		{
			StaticBitset b(800);
			unsigned int sum_set = 0;
			srand(time(nullptr));
			for (int i = 0; i < 800; i++)
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
