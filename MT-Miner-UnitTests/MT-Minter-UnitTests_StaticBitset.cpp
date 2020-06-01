#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace MTMinerUnitTests_Bitset
{
	TEST_CLASS(MTMinerUnitTests_StaticBitset)
	{
	public:
		TEST_METHOD(TestingBitsets_equal)
		{
			StaticBitset<std::bitset<SIZE_0>> b0(SIZE_0);
			StaticBitset<std::bitset<SIZE_0>> b1(SIZE_0);

			srand(time(nullptr));
			for (int i = 0; i < SIZE_0; i++)
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
			StaticBitset<std::bitset<SIZE_0>> b0(SIZE_0);
			StaticBitset<std::bitset<SIZE_0>> b1(SIZE_0);
			StaticBitset<std::bitset<SIZE_0>> b2(SIZE_0);

			srand(time(nullptr));
			for (int i = 0; i < SIZE_0; i++)
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
			StaticBitset<std::bitset<SIZE_0>> b0(SIZE_0);
			StaticBitset<std::bitset<SIZE_0>> b1(SIZE_0);
			StaticBitset<std::bitset<SIZE_0>> b2(SIZE_0);

			srand(time(nullptr));
			for (int i = 0; i < SIZE_0; i++)
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
			StaticBitset<std::bitset<SIZE_0>> b(800);
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
