#ifndef TEST_MOVABLE_H
#define TEST_MOVABLE_H

namespace {

class Movable
{
  public:
	const unsigned int Size = 10;
	enum class Construction	{ INITIALIZED };

	Movable() : size_(Size), array_(new int[Size]) { }

	Movable(Construction mode)
		: size_(Size), array_(new int[Size])
	{
		for (unsigned int i = 0; i < size_; i++)
			array_[i] = i;
	}

	~Movable() { delete[] array_; }

	Movable(const Movable &other)
		: size_(Size), array_(new int[Size])
	{
		for (unsigned int i = 0; i < Size; i++)
			array_[i] = other.array_[i];
	}

	Movable &operator=(Movable other)
	{
		nctl::swap(this->size_, other.size_);
		nctl::swap(this->array_, other.array_);
		return *this;
	}

	Movable(Movable &&other)
		: size_(other.size_), array_(other.array_)
	{
		other.size_ = 0;
		other.array_ = nullptr;
	}

	unsigned int size() const { return size_; }
	const int *data() const { return array_; }

	const int &operator[](unsigned int index) const
	{
		return array_[index];
	}

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
