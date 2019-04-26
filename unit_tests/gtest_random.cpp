#include "gtest_random.h"

namespace {

class RandomTest : public ::testing::Test
{
  public:
	nc::Random rnd_;
};

TEST_F(RandomTest, GenerateBoundedInteger)
{
	const uint32_t min = 0;
	const uint32_t max = 6;
	printf("Generate a random integer between %u and %u, %u not included\n", min, max, max);

	bool minGenerated = false;
	bool maxGenerated = false;
	for (unsigned int i = 0; i < Repetitions; i++)
	{
		const uint32_t num = rnd_.integer(min, max);
		printf("Number %u is: %u\n", i, num);
		ASSERT_TRUE(num >= min);
		ASSERT_TRUE(num < max);

		if (num == min && minGenerated == false)
		{
			minGenerated = true;
			printf("Minimum number %u generated at repetition %u\n", min, i);
		}
		else if (num == max - 1 && maxGenerated == false)
		{
			maxGenerated = true;
			printf("Maximum number %u generated at repetition %u\n", max - 1, i);
		}

		if (minGenerated == true && maxGenerated == true)
			break;
	}

	ASSERT_TRUE(minGenerated);
	ASSERT_TRUE(maxGenerated);
}

TEST_F(RandomTest, FastGenerateBoundedInteger)
{
	const uint32_t min = 0;
	const uint32_t max = 6;
	printf("Generate a random integer between %u and %u, %u not included\n", min, max, max);

	bool minGenerated = false;
	bool maxGenerated = false;
	for (unsigned int i = 0; i < Repetitions; i++)
	{
		const uint32_t num = rnd_.fastInteger(min, max);
		printf("Number %u is: %u\n", i, num);
		ASSERT_TRUE(num >= min);
		ASSERT_TRUE(num < max);

		if (num == min && minGenerated == false)
		{
			minGenerated = true;
			printf("Minimum number %u generated at repetition %u\n", min, i);
		}
		else if (num == max - 1 && maxGenerated == false)
		{
			maxGenerated = true;
			printf("Maximum number %u generated at repetition %u\n", max - 1, i);
		}

		if (minGenerated == true && maxGenerated == true)
			break;
	}

	ASSERT_TRUE(minGenerated);
	ASSERT_TRUE(maxGenerated);
}

TEST_F(RandomTest, GenerateReal)
{
	const float min = 0.0f;
	const float max = 1.0f;
	printf("Generate a random float between %f and %f\n", min, max);

	for (unsigned int i = 0; i < Repetitions; i++)
	{
		const float num = rnd_.real();
		printf("Number %u is: %f\n", i, num);
		ASSERT_TRUE(num > min);
		ASSERT_TRUE(num < max);
	}
}

TEST_F(RandomTest, FastGenerateReal)
{
	const float min = 0.0f;
	const float max = 1.0f;
	printf("Generate a random float between %f and %f\n", min, max);

	for (unsigned int i = 0; i < Repetitions; i++)
	{
		const float num = rnd_.fastReal();
		printf("Number %u is: %f\n", i, num);
		ASSERT_TRUE(num > min);
		ASSERT_TRUE(num < max);
	}
}

TEST_F(RandomTest, GenerateBoundedReal)
{
	const float min = 0.25f;
	const float max = 0.75f;
	printf("Generate a random float between %f and %f\n", min, max);

	for (unsigned int i = 0; i < Repetitions; i++)
	{
		const float num = rnd_.real(min, max);
		printf("Number %u is: %f\n", i, num);
		ASSERT_TRUE(num > min);
		ASSERT_TRUE(num < max);
	}
}

TEST_F(RandomTest, FastGenerateBoundedReal)
{
	const float min = 0.25f;
	const float max = 0.75f;
	printf("Generate a random float between %f and %f\n", min, max);

	for (unsigned int i = 0; i < Repetitions; i++)
	{
		const float num = rnd_.fastReal(min, max);
		printf("Number %u is: %f\n", i, num);
		ASSERT_TRUE(num > min);
		ASSERT_TRUE(num < max);
	}
}

}
