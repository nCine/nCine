#ifndef TEST_MOVABLE_H
#define TEST_MOVABLE_H

#define TEST_AUTOMATIC_STORAGE (0)

namespace {

#if !TEST_AUTOMATIC_STORAGE
class Movable
{
  public:
	static const unsigned int Size = 128;
	enum class Construction
	{
		ALLOCATED,
		INITIALIZED
	};

	Movable()
	    : size_(0), array_(nullptr) {}

	Movable(Construction mode)
	    : size_(Size), array_(new int[Size])
	{
		if (mode == Construction::INITIALIZED)
		{
			for (unsigned int i = 0; i < size_; i++)
				array_[i] = i;
		}
	}

	Movable(const Movable &other)
	    : size_(other.size_), array_(new int[Size])
	{
		for (unsigned int i = 0; i < size_; i++)
			array_[i] = other.array_[i];
	}

	Movable(Movable &&other)
	    : size_(other.size_), array_(other.array_)
	{
		other.size_ = 0;
		other.array_ = nullptr;
	}

	~Movable() { delete[] array_; }

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

	Movable &operator=(Movable &&other)
	{
	#ifdef TEST_WITH_NCTL
		nctl::swap(this->size_, other.size_);
		nctl::swap(this->array_, other.array_);
	#else
		std::swap(this->size_, other.size_);
		std::swap(this->array_, other.array_);
	#endif
		return *this;
	}

	inline unsigned int size() const { return size_; }
	inline const int *data() const { return array_; }
	inline int *data() { return array_; }

  private:
	unsigned int size_;
	int *array_;
};
#else
class Movable
{
  public:
	static const unsigned int Size = 128;
	enum class Construction
	{
		ALLOCATED, // Added for code compatibility
		INITIALIZED
	};

	Movable() {}

	Movable(Construction mode)
	{
		for (unsigned int i = 0; i < Size; i++)
			array_[i] = i;
	}

	Movable(const Movable &other)
	{
		for (unsigned int i = 0; i < Size; i++)
			array_[i] = other.array_[i];
	}

	~Movable() {}

	Movable &operator=(const Movable &other)
	{
		if (this != &other)
		{
			for (unsigned int i = 0; i < Size; i++)
				array_[i] = other.array_[i];
		}

		return *this;
	}

	inline unsigned int size() const { return Size; }
	inline const int *data() const { return array_; }
	inline int *data() { return array_; }

  private:
	int array_[Size];
};
#endif

}

#endif
