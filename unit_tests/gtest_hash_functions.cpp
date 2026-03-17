#include "gtest_hash_functions.h"

namespace {

// --- FastHash64Test ---

TEST(FastHash64Test, Deterministic)
{
	const uint64_t ha = nctl::fasthash64(Text, TextLength + 1, 123);
	const uint64_t hb = nctl::fasthash64(Text, TextLength + 1, 123);

	ASSERT_EQ(ha, hb);
}

TEST(FastHash64Test, DifferentInputs)
{
	const uint64_t ha = nctl::fasthash64(SmallTextA, SmallTextALength + 1, 0);
	const uint64_t hb = nctl::fasthash64(SmallTextB, SmallTextBLength + 1, 0);

	ASSERT_NE(ha, hb);
}

TEST(FastHash64Test, DifferentSeeds)
{
	const uint64_t ha = nctl::fasthash64(Text, TextLength + 1, 1);
	const uint64_t hb = nctl::fasthash64(Text, TextLength + 1, 2);

	ASSERT_NE(ha, hb);
}

TEST(FastHash64Test, SmallSizes)
{
	for (unsigned int len = 1; len < 8; len++)
	{
		const uint64_t h = nctl::fasthash64(ShortBuffer, len, 0);
		ASSERT_NE(h, 0ULL);
	}
}

TEST(FastHash64Test, Prefixes)
{
	uint64_t prev = nctl::fasthash64(ShortBuffer, 1, 0);

	for (int len = 2; len < 8; len++)
	{
		uint64_t h = nctl::fasthash64(ShortBuffer, len, 0);
		ASSERT_NE(h, prev);
		prev = h;
	}
}

TEST(FastHash64Test, CollisionStress)
{
	nctl::HashSet<uint64_t, nctl::Mul64To32Hash<uint64_t>> hashes(CollisionCount * 3);

	char buffer[32];
	for (unsigned int i = 0; i < CollisionCount; i++)
	{
		sprintf(buffer, "value_%d", i);
		uint64_t h = nctl::fasthash64(buffer, strlen(buffer), 0);

		hashes.insert(h);
	}

	ASSERT_GT(hashes.size(), CollisionCount * 0.99);
}

// --- FastHash32Test ---

TEST(FastHash32Test, Deterministic)
{
	const uint32_t ha = nctl::fasthash32(Text, TextLength + 1, 123);
	const uint32_t hb = nctl::fasthash32(Text, TextLength + 1, 123);

	ASSERT_EQ(ha, hb);
}

TEST(FastHash32Test, DifferentInputs)
{
	const uint32_t ha = nctl::fasthash32(SmallTextA, SmallTextALength + 1, 0);
	const uint32_t hb = nctl::fasthash32(SmallTextB, SmallTextBLength + 1, 0);

	ASSERT_NE(ha, hb);
}

TEST(FastHash32Test, DifferentSeeds)
{
	const uint32_t ha = nctl::fasthash32(Text, TextLength + 1, 1);
	const uint32_t hb = nctl::fasthash32(Text, TextLength + 1, 2);

	ASSERT_NE(ha, hb);
}

TEST(FastHash32Test, SmallSizes)
{
	for (unsigned int len = 1; len < 8; len++)
	{
		const uint32_t h = nctl::fasthash32(ShortBuffer, len, 0);
		ASSERT_NE(h, 0);
	}
}

TEST(FastHash32Test, Prefixes)
{
	uint32_t prev = nctl::fasthash32(ShortBuffer, 1, 0);

	for (int len = 2; len < 8; len++)
	{
		const uint32_t h = nctl::fasthash32(ShortBuffer, len, 0);
		ASSERT_NE(h, prev);
		prev = h;
	}
}

TEST(FastHash32Test, CollisionStress)
{
	nctl::HashSet<uint32_t, nctl::IdentityHashFunc<uint32_t>> hashes(CollisionCount * 3);

	char buffer[32];
	for (unsigned int i = 0; i < CollisionCount; i++)
	{
		sprintf(buffer, "value_%d", i);
		const uint32_t h = nctl::fasthash32(buffer, strlen(buffer), 0);

		hashes.insert(h);
	}

	ASSERT_GT(hashes.size(), CollisionCount * 0.99);
}

// --- HashFunctorTest ---

using HashFunctorTypes = ::testing::Types<
	nctl::FastHashFunc<nctl::String>,
	nctl::deprecated::SaxHashFunc<nctl::String>,
	nctl::deprecated::JenkinsHashFunc<nctl::String>,
	nctl::deprecated::FNV1aHashFunc<nctl::String>
>;

template <typename T>
class HashFunctorTest : public ::testing::Test
{
  public:
	using HashFunc = T;
	HashFunc hashFunc;
};

TYPED_TEST_SUITE(HashFunctorTest, HashFunctorTypes);

TYPED_TEST(HashFunctorTest, Deterministic)
{
	nctl::String str(Text);

	const nctl::hash_t ha = this->hashFunc(str);
	const nctl::hash_t hb = this->hashFunc(str);

	ASSERT_EQ(ha, hb);
}

TYPED_TEST(HashFunctorTest, DifferentInputs)
{
	nctl::String a(SmallTextA);
	nctl::String b(SmallTextB);

	const nctl::hash_t ha = this->hashFunc(a);
	const nctl::hash_t hb = this->hashFunc(b);

	ASSERT_NE(ha, hb);
}

TYPED_TEST(HashFunctorTest, SmallSizes)
{
	for (unsigned int len = 1; len < 8; len++)
	{
		nctl::String str(len);
		str.setLength(len);
		for (unsigned int i = 0; i < len; i++)
			str[i] = ShortBuffer[i];

		const nctl::hash_t h = this->hashFunc(str);

		ASSERT_NE(h, 0);
	}
}

TYPED_TEST(HashFunctorTest, Prefixes)
{
	nctl::String str(ShortBuffer);

	nctl::hash_t prev = this->hashFunc(str);

	for (unsigned int i = 1; i < str.length(); i++)
	{
		nctl::String prefix(i);
		str.setLength(i);
		for (unsigned int j = 0; j < i; j++)
			str[j] = ShortBuffer[j];

		const nctl::hash_t h = this->hashFunc(prefix);

		ASSERT_NE(h, prev);
		prev = h;
	}
}

TYPED_TEST(HashFunctorTest, CollisionStress)
{
	nctl::HashSet<uint32_t, nctl::IdentityHashFunc<uint32_t>> hashes(CollisionCount * 3);

	char buffer[32];
	for (unsigned int i = 0; i < CollisionCount; i++)
	{
		sprintf(buffer, "value_%u", i);

		nctl::String str(buffer);
		const nctl::hash_t h = this->hashFunc(str);

		hashes.insert(h);
	}

	ASSERT_GT(hashes.size(), CollisionCount * 0.99);
}

}
