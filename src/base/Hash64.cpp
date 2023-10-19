#include "Hash64.h"
#include <nctl/HashFunctions.h>
#include <nctl/CString.h>
#include "FileSystem.h"

namespace ncine {

namespace {
	const uint64_t HashSeed = 0x01000193811C9DC5;
	unsigned int bufferSize = 0;
	nctl::UniquePtr<uint8_t[]> bufferPtr;
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
	uint64_t hash = 0;
	for (unsigned int i = 0; i < count; i++)
	{
		ASSERT(strings[i] != nullptr);
		ASSERT(lengths[i] > 0);
		if (strings[i] != nullptr && lengths[i] > 0)
		{
			const unsigned int length = static_cast<unsigned int>(lengths[i]);
			// Align the length to 64 bits, plus 7 bytes read after that
			const unsigned int paddedLength = length + 8 - (length % 8) + 7;
			// Recycling the loading binary buffer for zero-padded shader sources
			if (bufferSize < paddedLength)
			{
				bufferSize = paddedLength;
				bufferPtr = nctl::makeUnique<uint8_t[]>(bufferSize);
			}

			for (unsigned int j = 0; j < length; j++)
				bufferPtr[j] = strings[i][j];
			// Set padding bytes to zero for a deterministic hash
			for (unsigned int j = length; j < paddedLength; j++)
				bufferPtr[j] = '\0';

			hash += nctl::fasthash64(bufferPtr.get(), length, HashSeed);

			if (statisticsEnabled_)
			{
				statistics_.HashedStrings++;
				statistics_.HashedCharacters += lengths[i];
			}
		}
	}
	if (hash != 0 && statisticsEnabled_)
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
		const unsigned long int length = nctl::strnlen(filename, fs::MaxPathLength);

		const fs::FileDate d = fs::lastModificationTime(filename);
		const long int s = fs::fileSize(filename);
		unsigned long int n = 0;
		for (unsigned int i = 0; i < length; i++)
			n += static_cast<unsigned int>(filename[i]);

		// Combining file name, size, and date in a unique id
		hash = (n + s) * 100000000000000 + d.year * 10000000000 + d.month * 100000000 + d.day * 1000000 + d.hour * 10000 + d.minute * 100 + d.second;

		if (statisticsEnabled_)
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
		char stringToParse[17] = "\0";
		for (unsigned int i = 0; i < length / 16; i++)
		{
			for (unsigned j = 0; j < 16; j++)
				stringToParse[j] = string[i * 16 + j];
			stringToParse[16] = '\0';
			hash += strtoull(stringToParse, nullptr, 16);
		}

		if (statisticsEnabled_)
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
