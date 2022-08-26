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
	const uint64_t *pos = reinterpret_cast<const uint64_t *>(buf);
	const uint64_t *end = pos + (len / 8);
	uint64_t h = seed ^ (len * m);
	uint64_t v = 0;

	while (pos <= end)
	{
		v = *pos++;
		h ^= fasthash_mix(v);
		h *= m;
	}

	const unsigned char *pos2 = reinterpret_cast<const unsigned char *>(pos);
	v = 0;

	switch (len & 7)
	{
		case 7: v ^= static_cast<uint64_t>(pos2[6]) << 48;
		case 6: v ^= static_cast<uint64_t>(pos2[5]) << 40;
		case 5: v ^= static_cast<uint64_t>(pos2[4]) << 32;
		case 4: v ^= static_cast<uint64_t>(pos2[3]) << 24;
		case 3: v ^= static_cast<uint64_t>(pos2[2]) << 16;
		case 2: v ^= static_cast<uint64_t>(pos2[1]) << 8;
		case 1: v ^= static_cast<uint64_t>(pos2[0]);
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

}
