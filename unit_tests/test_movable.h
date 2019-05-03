#ifndef TEST_MOVABLE_H
#define TEST_MOVABLE_H

#define TEST_MOVABLE_ONLY (0)

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

  private:
	unsigned int size_;
	int *array_;
};

}

#endif
