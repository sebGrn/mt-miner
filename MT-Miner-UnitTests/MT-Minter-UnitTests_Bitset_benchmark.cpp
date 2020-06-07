#include "pch.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#define SIZE_TEST SIZE_6

namespace MTMinerUnitTests_Bitset_benchmark
{
	TEST_CLASS(MTMinerUnitTests_Bitset_benchmark)
	{
	public:
		void testing(Bitset& b)
		{
			const int n = 1000;
			for (int j = 0; j < n; j++)
			{
				for (int i = 0; i < SIZE_TEST; i++)
					b.set(i, rand() % 2);
				b.bitset_or(b);
				b.bitset_and(b);
				b.bitset_compare(b);
				b = b;
			}
		}

		TEST_METHOD(TestingBitsets_static)
		{
			StaticBitset<std::bitset<SIZE_TEST>> b(SIZE_TEST);
			srand(time(nullptr));
			testing(b);
			Assert::AreEqual(true, true);
		}

		TEST_METHOD(TestingBitsets_custom)
		{
			CustomBitset b(SIZE_TEST);
			srand(time(nullptr));
			testing(b);
			Assert::AreEqual(true, true);
		}

		TEST_METHOD(TestingBitsets_any)
		{
			AnyBitset b(SIZE_TEST);
			srand(time(nullptr));
			testing(b);
			Assert::AreEqual(true, true);
		}

		TEST_METHOD(TestingBitsets_variant)
		{
			VariantBitset b(SIZE_TEST);
			srand(time(nullptr));
			testing(b);
			Assert::AreEqual(true, true);
		}

		TEST_METHOD(SparseBitsets_sparse)
		{
			SparseIndexBitset b(SIZE_TEST);
			srand(time(nullptr));
			testing(b);
			Assert::AreEqual(true, true);
		}
	};
}
