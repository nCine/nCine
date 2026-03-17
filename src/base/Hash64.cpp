#include "Hash64.h"
#include <nctl/HashFunctions.h>
#include <nctl/CString.h>
#include "FileSystem.h"

namespace ncine {

namespace {
	constexpr uint64_t HashSeed = 0x01000193811C9DC5ULL;
}

Hash64 &hash64()
{
	static Hash64 instance;
	return instance;
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

uint64_t Hash64::hashStrings(unsigned int count, const char **strings, const int *lengths) const
{
	uint64_t hash = HashSeed;

	for (unsigned int i = 0; i < count; i++)
	{
		ASSERT(strings[i] != nullptr);
		ASSERT(lengths[i] > 0);
		if (strings[i] != nullptr && lengths[i] > 0)
		{
			const size_t length = static_cast<size_t>(lengths[i]);

			hash = nctl::fasthash64(strings[i], length, hash);

			statistics_.HashedStrings++;
			statistics_.HashedCharacters += lengths[i];
		}
	}

	if (count > 0)
		statistics_.HashStringCalls++;

	return hash;
}

uint64_t Hash64::hashString(const char *string, int length) const
{
	const char *strings[1] = { string };
	const int lengths[1] = { length };
	return hashStrings(1, strings, lengths);
}

uint64_t Hash64::hashFileStat(const char *filename) const
{
	uint64_t hash = 0;
	const bool isReadable = fs::isReadableFile(filename);

	ASSERT(isReadable);
	if (isReadable)
	{
		const size_t length = nctl::strnlen(filename, fs::MaxPathLength);
		const long int s = fs::fileSize(filename);
		const fs::FileDate d = fs::lastModificationTime(filename);

		hash = nctl::fasthash64(filename, length, HashSeed);
		hash = nctl::fasthash64(&s, sizeof(s), hash);
		hash = nctl::fasthash64(&d, sizeof(d), hash);

		statistics_.HashedFiles++;
	}

	return hash;
}

/*! \note Can be used to scan MD5 sums from CMake `file(MD5)` command (128 bits, 32 chars) or BinaryShaderCache fasthash64() hashes (64 bits, 16 chars) */
uint64_t Hash64::scanHashString(const char *string, unsigned int length) const
{
	uint64_t hash = 0;
	ASSERT(string != nullptr && length % 16 == 0);

	if (string != nullptr && length % 16 == 0)
	{
		char stringToParse[17];
		stringToParse[16] = '\0';

		for (unsigned int i = 0; i < length / 16; i++)
		{
			memcpy(stringToParse, &string[i * 16], 16);
			const uint64_t part = strtoull(stringToParse, nullptr, 16);

			hash ^= part;
		}

		statistics_.ScannedHashStrings++;
	}

	return hash;
}

/*! \note Can be used to scan MD5 sums from CMake `file(MD5)` command (128 bits, 32 chars) or BinaryShaderCache fasthash64() hashes (64 bits, 16 chars) */
uint64_t Hash64::scanHashString(const char *string) const
{
	const unsigned int length = nctl::strnlen(string, 256);
	return scanHashString(string, length);
}

void Hash64::clearStatistics()
{
	statistics_.HashStringCalls = 0;
	statistics_.HashedStrings = 0;
	statistics_.HashedCharacters = 0;
	statistics_.HashedFiles = 0;
	statistics_.ScannedHashStrings = 0;
}

}
