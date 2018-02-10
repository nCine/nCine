#ifndef CLASS_NCTL_HASHFUNCTIONS
#define CLASS_NCTL_HASHFUNCTIONS

namespace nctl {

using hash_t = unsigned long int;

/// Hash function returning always the first hashmap bucket, for debug purposes
template <class K>
class FixedHashFunc
{
  public:
	hash_t operator()(const K &key) const { return static_cast<hash_t>(0); }
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

}

#endif
