#include <nctl/HashFunctions.h>

namespace nctl {

// Compression function for Merkle-Damgard construction.
uint64_t fasthash_mix(uint64_t h)
{
	h ^= h >> 23;
	h *= 0x2127599bf4325c37ULL;
	h ^= h >> 47;
	return h;
}

uint64_t fasthash64(const void *buf, size_t len, uint64_t seed)
{
	const uint64_t m = 0x880355f21e6d1965ULL;
	const unsigned char *pos = static_cast<const unsigned char *>(buf);
	const unsigned char *end = pos + (len & ~size_t(7));
	uint64_t h = seed ^ (len * m);

	while (pos < end)
	{
		uint64_t v;
		memcpy(&v, pos, sizeof(uint64_t));
		pos += 8;

		h ^= fasthash_mix(v);
		h *= m;
	}

	uint64_t v = 0;
	switch (len & 7)
	{
		case 7: v ^= static_cast<uint64_t>(pos[6]) << 48;
		case 6: v ^= static_cast<uint64_t>(pos[5]) << 40;
		case 5: v ^= static_cast<uint64_t>(pos[4]) << 32;
		case 4: v ^= static_cast<uint64_t>(pos[3]) << 24;
		case 3: v ^= static_cast<uint64_t>(pos[2]) << 16;
		case 2: v ^= static_cast<uint64_t>(pos[1]) << 8;
		case 1: v ^= static_cast<uint64_t>(pos[0]);
			h ^= fasthash_mix(v);
			h *= m;
	}

	return fasthash_mix(h);
}

uint32_t fasthash32(const void *buf, size_t len, uint32_t seed)
{
	// The following trick converts the 64-bit hashcode to Fermat residue, which
	// shall retain information from both the higher and lower parts of hashcode.
	uint64_t h = fasthash64(buf, len, seed);
	return static_cast<uint32_t>(h - (h >> 32));
}

// ---------------------------------------------------------

namespace deprecated {

uint32_t saxHashBytes(const unsigned char *data, size_t len, uint32_t seed)
{
	uint32_t hash = seed;
	for (size_t i = 0; i < len; i++)
		hash ^= (hash << 5) + (hash >> 2) + data[i];
	return hash;
}

uint32_t saxHashBytes(const unsigned char *data, size_t len)
{
	return saxHashBytes(data, len, 0x9E3779B9);
}

uint32_t jenkinsHashBytes(const unsigned char *data, size_t len, uint32_t seed)
{
	uint32_t hash = seed;
	for (size_t i = 0; i < len; i++)
	{
		hash += data[i];
		hash += (hash << 10);
		hash ^= (hash >> 6);
	}

	hash += (hash << 3);
	hash ^= (hash >> 11);
	hash += (hash << 15);
	return hash;
}

uint32_t jenkinsHashBytes(const unsigned char *data, size_t len)
{
	return jenkinsHashBytes(data, len, 0x9E3779B9);
}

uint32_t fnv1aHashBytes(const unsigned char *data, size_t len)
{
	const uint32_t Prime = 0x01000193;
	uint32_t hash = 0x811C9DC5;

	for (size_t i = 0; i < len; i++)
		hash = (data[i] ^ hash) * Prime;

	return hash;
}

}

}
