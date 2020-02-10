#ifndef CLASS_NCTL_HASHFUNCTIONS
#define CLASS_NCTL_HASHFUNCTIONS

#include <cstdint>
#include <cstring>

namespace nctl {

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

/// Shift-Add-XOR hash function
template <class K>
class SaxHashFunc
{
  public:
	hash_t operator()(const K &key) const
	{
		const unsigned char *bytes = reinterpret_cast<const unsigned char *>(&key);
		hash_t hash = static_cast<hash_t>(0);
		for (unsigned int i = 0; i < sizeof(K); i++)
			hash ^= (hash << 5) + (hash >> 2) + static_cast<hash_t>(bytes[i]);

		return hash;
	}
};

/// Shift-Add-XOR hash function
/*!
 * \note Specialized version of the function for C-style strings
 */
template <>
class SaxHashFunc<const char *>
{
  public:
	hash_t operator()(const char *key) const
	{
		const unsigned int length = strlen(key);
		hash_t hash = static_cast<hash_t>(0);
		for (unsigned int i = 0; i < length; i++)
			hash ^= (hash << 5) + (hash >> 2) + static_cast<hash_t>(key[i]);

		return hash;
	}
};

/// Shift-Add-XOR hash function
/*!
 * \note The key type should be a container exposing `length()` and `operator[]()` methods.
 * Contained elements should be convertible to `hash_t`.
 */
template <class K>
class SaxHashFuncContainer
{
  public:
	hash_t operator()(const K &key) const
	{
		hash_t hash = static_cast<hash_t>(0);
		for (unsigned int i = 0; i < key.length(); i++)
			hash ^= (hash << 5) + (hash >> 2) + static_cast<hash_t>(key[i]);

		return hash;
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
		const unsigned char *bytes = reinterpret_cast<const unsigned char *>(&key);
		hash_t hash = static_cast<hash_t>(0);
		for (unsigned int i = 0; i < sizeof(K); i++)
		{
			hash += static_cast<hash_t>(bytes[i]);
			hash += (hash << 10);
			hash ^= (hash >> 6);
		}
		hash += (hash << 3);
		hash ^= (hash >> 11);
		hash += (hash << 15);

		return hash;
	}
};

/// Jenkins hash function
/*!
 * \note Specialized version of the function for C-style strings
 *
 * For more information: http://en.wikipedia.org/wiki/Jenkins_hash_function
 */
template <>
class JenkinsHashFunc<const char *>
{
  public:
	hash_t operator()(const char *key) const
	{
		const unsigned int length = strlen(key);
		hash_t hash = static_cast<hash_t>(0);
		for (unsigned int i = 0; i < length; i++)
		{
			hash += static_cast<hash_t>(key[i]);
			hash += (hash << 10);
			hash ^= (hash >> 6);
		}
		hash += (hash << 3);
		hash ^= (hash >> 11);
		hash += (hash << 15);

		return hash;
	}
};

/// Jenkins hash function
/*!
 * \note The key type should be a container exposing `length()` and `operator[]()` methods.
 * Contained elements should be convertible to `hash_t`.
 *
 * For more information: http://en.wikipedia.org/wiki/Jenkins_hash_function
 */
template <class K>
class JenkinsHashFuncContainer
{
  public:
	hash_t operator()(const K &key) const
	{
		hash_t hash = static_cast<hash_t>(0);
		for (unsigned int i = 0; i < key.length(); i++)
		{
			hash += static_cast<hash_t>(key[i]);
			hash += (hash << 10);
			hash ^= (hash >> 6);
		}
		hash += (hash << 3);
		hash ^= (hash >> 11);
		hash += (hash << 15);

		return hash;
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
		const unsigned char *bytes = reinterpret_cast<const unsigned char *>(&key);
		hash_t hash = static_cast<hash_t>(Seed);
		for (unsigned int i = 0; i < sizeof(K); i++)
			hash = fnv1a(bytes[i], hash);

		return hash;
	}

  private:
	static const hash_t Prime = 0x01000193; //  16777619
	static const hash_t Seed = 0x811C9DC5; // 2166136261

	inline hash_t fnv1a(const unsigned char oneByte, hash_t hash = Seed) const
	{
		return (oneByte ^ hash) * Prime;
	}
};

/*!
 * \note Specialized version of the function for C-style strings
 *
 * For more information: http://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function
 */
template <>
class FNV1aHashFunc<const char *>
{
  public:
	hash_t operator()(const char *key) const
	{
		const unsigned int length = strlen(key);
		hash_t hash = static_cast<hash_t>(Seed);
		for (unsigned int i = 0; i < length; i++)
			hash = fnv1a(key[i], hash);

		return hash;
	}

  private:
	static const hash_t Prime = 0x01000193; //  16777619
	static const hash_t Seed = 0x811C9DC5; // 2166136261

	inline hash_t fnv1a(const char oneByte, hash_t hash = Seed) const
	{
		return (oneByte ^ hash) * Prime;
	}
};

/// Fowler-Noll-Vo Hash (FNV-1a)
/*!
 * \note The key type should be a container exposing `length()` and `operator[]()` methods.
 * Contained elements should be convertible to `hash_t`.
 *
 * For more information: http://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function
 */
template <class K>
class FNV1aHashFuncContainer
{
  public:
	hash_t operator()(const K &key) const
	{
		hash_t hash = static_cast<hash_t>(Seed);
		for (unsigned int i = 0; i < key.length(); i++)
			hash = fnv1a(static_cast<hash_t>(key[i]), hash);

		return hash;
	}

  private:
	static const hash_t Prime = 0x01000193; //  16777619
	static const hash_t Seed = 0x811C9DC5; // 2166136261

	inline hash_t fnv1a(unsigned char oneByte, hash_t hash = Seed) const
	{
		return (oneByte ^ hash) * Prime;
	}
};

}

#endif
