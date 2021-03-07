#ifndef TEST_MOVABLE_H
#define TEST_MOVABLE_H

#define TEST_MOVABLE_ONLY (0)

#include <nctl/HashFunctions.h>

namespace {

class Movable
{
  public:
	static const unsigned int Size = 10;
	enum class Construction
	{
		INITIALIZED
	};

	Movable()
	    : size_(0), array_(nullptr) {}

	Movable(Construction mode)
	    : size_(Size), array_(new int[Size])
	{
		for (unsigned int i = 0; i < size_; i++)
			array_[i] = i;
	}

	Movable(Movable &&other)
	    : size_(other.size_), array_(other.array_)
	{
		other.size_ = 0;
		other.array_ = nullptr;
	}

	~Movable() { delete[] array_; }

	Movable &operator=(Movable &&other)
	{
		nctl::swap(this->size_, other.size_);
		nctl::swap(this->array_, other.array_);
		return *this;
	}

#if TEST_MOVABLE_ONLY
	Movable(const Movable &other) = delete;
	Movable &operator=(const Movable &other) = delete;
#else
	Movable(const Movable &other)
	    : size_(other.size_), array_(new int[Size])
	{
		for (unsigned int i = 0; i < size_; i++)
			array_[i] = other.array_[i];
	}

	Movable &operator=(const Movable &other)
	{
		if (this != &other)
		{
			delete[] array_;
			size_ = other.size_;
			array_ = new int[size_];

			for (unsigned int i = 0; i < size_; i++)
				array_[i] = other.array_[i];
		}

		return *this;
	}
#endif

	unsigned int size() const
	{
		return size_;
	}

	// Needed by container hash functions
	unsigned int length() const
	{
		return size_;
	}

	const int *data() const { return array_; }
	const int &operator[](unsigned int index) const { return array_[index]; }

	void printAndAssert() const
	{
		for (unsigned int i = 0; i < size_; i++)
		{
			printf(" %d", array_[i]);
			ASSERT_EQ(array_[i], i);
		}
		printf("\n");
	}

	friend inline bool operator==(const Movable &lhs, const Movable &rhs)
	{
		bool result = (lhs.size_ == rhs.size_);

		if (result)
		{
			for (unsigned int i = 0; i < lhs.size_; i++)
			{
				if (lhs.array_[i] != rhs.array_[i])
				{
					result = false;
					break;
				}
			}
		}

		return result;
	}

  private:
	unsigned int size_;
	int *array_;
};

}

namespace nctl {

/// Fowler-Noll-Vo Hash (FNV-1a)
/*!
 * \note Specialized version of the function for Movable objects
 *
 * For more information: http://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function
 */
template <>
class FNV1aHashFunc<Movable>
{
  public:
	hash_t operator()(const Movable &key) const
	{
		const unsigned int length = key.length();
		hash_t hash = static_cast<hash_t>(Seed);
		for (unsigned int i = 0; i < length; i++)
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
