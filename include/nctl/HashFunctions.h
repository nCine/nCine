#ifndef CLASS_NCTL_HASHFUNCTIONS
#define CLASS_NCTL_HASHFUNCTIONS

#include <cstdint>
#include <cstring>
#include "String.h"

namespace nctl {

namespace detail {

template <class K>
struct KeyBytes
{
	static const unsigned char *data(const K &key)
	{
		return reinterpret_cast<const unsigned char *>(&key);
	}

	static size_t size(const K &)
	{
		return sizeof(K);
	}
};

template <>
struct KeyBytes<const char *>
{
	static const unsigned char *data(const char *key)
	{
		return reinterpret_cast<const unsigned char *>(key);
	}

	static size_t size(const char *key)
	{
		return strlen(key);
	}
};

template <>
struct KeyBytes<String>
{
	static const unsigned char *data(const String &s)
	{
		return reinterpret_cast<const unsigned char *>(s.data());
	}

	static size_t size(const String &s)
	{
		return s.length();
	}
};

}

using hash_t = uint32_t;
const hash_t NullHash = static_cast<hash_t>(~0);

/// Hash function returning always the first hashmap bucket, for debug purposes
template <class K>
class FixedHashFunc
{
  public:
	hash_t operator()(const K &key) const { return static_cast<hash_t>(0); }
};

/// Hash function returning the modulo of the key, for debug purposes
template <class K, unsigned int Value>
class ModuloHashFunc
{
  public:
	hash_t operator()(const K &key) const { return static_cast<hash_t>(key % Value); }
};

/// Hash function returning the key unchanged
/*! The key type should be convertible to `hash_t.` */
template <class K>
class IdentityHashFunc
{
  public:
	hash_t operator()(const K &key) const { return static_cast<hash_t>(key); }
};

/// Hash function performing a XOR-fold of a 64bit key down to a 32bit hash
/*! The key type should be convertible to `uint64_t.` */
template <class K>
class Fold64To32Hash
{
  public:
	hash_t operator()(const K &key) const
	{
		const uint64_t h = static_cast<uint64_t>(key);
		return static_cast<uint32_t>(h ^ (h >> 32));
	}
};

/// Hash function performing a multiply mix of a 64bit key down to a 32bit hash
/*! The key type should be convertible to `uint64_t.` */
template <class K>
class Mul64To32Hash
{
  public:
	hash_t operator()(const K &key) const
	{
		const uint64_t h = static_cast<uint64_t>(key);
		return static_cast<uint32_t>((h * 0x9E3779B97F4A7C15ULL) >> 32);
	}
};

namespace deprecated {

DLL_PUBLIC uint32_t saxHashBytes(const unsigned char *data, size_t len, uint32_t seed);
DLL_PUBLIC uint32_t saxHashBytes(const unsigned char *data, size_t len);
DLL_PUBLIC uint32_t jenkinsHashBytes(const unsigned char *data, size_t len, uint32_t seed);
DLL_PUBLIC uint32_t jenkinsHashBytes(const unsigned char *data, size_t len);
DLL_PUBLIC uint32_t fnv1aHashBytes(const unsigned char *data, size_t len);

/// Shift-Add-XOR hash function
template <class K>
class SaxHashFunc
{
  public:
	hash_t operator()(const K &key) const
	{
		const auto *data = detail::KeyBytes<K>::data(key);
		const size_t len = detail::KeyBytes<K>::size(key);
		return saxHashBytes(data, len);
	}
};

/// Jenkins hash function
/*!
 * For more information: http://en.wikipedia.org/wiki/Jenkins_hash_function
 */
template <class K>
class JenkinsHashFunc
{
  public:
	hash_t operator()(const K &key) const
	{
		const auto *data = detail::KeyBytes<K>::data(key);
		const size_t len = detail::KeyBytes<K>::size(key);
		return jenkinsHashBytes(data, len);
	}
};

/// Fowler-Noll-Vo Hash (FNV-1a)
/*!
 * For more information: http://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function
 */
template <class K>
class FNV1aHashFunc
{
  public:
	hash_t operator()(const K &key) const
	{
		const auto *data = detail::KeyBytes<K>::data(key);
		const size_t len = detail::KeyBytes<K>::size(key);
		return fnv1aHashBytes(data, len);
	}
};

}

DLL_PUBLIC uint64_t fasthash64(const void *buf, size_t len, uint64_t seed);
DLL_PUBLIC uint32_t fasthash32(const void *buf, size_t len, uint32_t seed);

/// fast-hash
/*!
 * For more information: https://github.com/ztanml/fast-hash
 */
template <class K>
class FastHashFunc
{
  public:
	hash_t operator()(const K &key) const
	{
		const auto *data = detail::KeyBytes<K>::data(key);
		const size_t len = detail::KeyBytes<K>::size(key);
		return fasthash32(data, len, Seed);
	}

  private:
	static const uint32_t Seed = 0x811C9DC5;
};

}

#endif
