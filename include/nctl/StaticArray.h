#ifndef CLASS_NCTL_STATICARRAY
#define CLASS_NCTL_STATICARRAY

#include <new>
#include <ncine/common_macros.h>
#include "ArrayIterator.h"
#include "ReverseIterator.h"
#include "utility.h"

namespace nctl {

/// Construction modes for the `StaticArray` class
/*! Declared outside the template class to use it without template parameters. */
enum class StaticArrayMode
{
	/// `StaticArray` will have a zero size
	ZERO_SIZE,
	/// `StaticArray` will extend the size to match its capacity
	EXTEND_SIZE
};

/// A static array based on templates that stores elements in the stack
template <class T, unsigned int C>
class StaticArray
{
  public:
	/// Iterator type
	using Iterator = ArrayIterator<T, false>;
	/// Constant iterator type
	using ConstIterator = ArrayIterator<T, true>;
	/// Reverse iterator type
	using ReverseIterator = nctl::ReverseIterator<Iterator>;
	/// Reverse constant iterator type
	using ConstReverseIterator = nctl::ReverseIterator<ConstIterator>;

	/// Constructs an empty array with fixed capacity
	StaticArray()
	    : StaticArray(StaticArrayMode::ZERO_SIZE) {}
	/// Constructs an array with the option for it to have the size match its capacity
	explicit StaticArray(StaticArrayMode mode)
	    : size_(0), capacity_(C)
	{
		if (mode == StaticArrayMode::EXTEND_SIZE)
			setSize(capacity_);
	}
	inline ~StaticArray() { destructArray(array_, size_); }

	/// Copy constructor
	StaticArray(const StaticArray &other);
	/// Move constructor
	StaticArray(StaticArray &&other);
	/// Assignment operator
	StaticArray &operator=(const StaticArray &other);
	/// Move assignment operator
	StaticArray &operator=(StaticArray &&other);

	/// Returns an iterator to the first element
	inline Iterator begin() { return Iterator(array_); }
	/// Returns a reverse iterator to the last element
	inline ReverseIterator rBegin() { return ReverseIterator(Iterator(array_ + size_ - 1)); }
	/// Returns an iterator to past the last element
	inline Iterator end() { return Iterator(array_ + size_); }
	/// Returns a reverse iterator to prior the first element
	inline ReverseIterator rEnd() { return ReverseIterator(Iterator(array_ - 1)); }

	/// Returns a constant iterator to the first element
	inline ConstIterator begin() const { return ConstIterator(array_); }
	/// Returns a constant reverse iterator to the last element
	inline ConstReverseIterator rBegin() const { return ConstReverseIterator(ConstIterator(array_ + size_ - 1)); }
	/// Returns a constant iterator to past the last lement
	inline ConstIterator end() const { return ConstIterator(array_ + size_); }
	/// Returns a constant reverse iterator to prior the first element
	inline ConstReverseIterator rEnd() const { return ConstReverseIterator(ConstIterator(array_ - 1)); }

	/// Returns a constant iterator to the first element
	inline ConstIterator cBegin() const { return ConstIterator(array_); }
	/// Returns a constant reverse iterator to the last element
	inline ConstReverseIterator crBegin() const { return ConstReverseIterator(ConstIterator(array_ + size_ - 1)); }
	/// Returns a constant iterator to past the last lement
	inline ConstIterator cEnd() const { return ConstIterator(array_ + size_); }
	/// Returns a constant reverse iterator to prior the first element
	inline ConstReverseIterator crEnd() const { return ConstReverseIterator(ConstIterator(array_ - 1)); }

	/// Returns true if the array is empty
	inline bool isEmpty() const { return size_ == 0; }
	/// Returns the array size
	/*! The array is filled without gaps until the `Size()`-1 element. */
	inline unsigned int size() const { return size_; }
	/// Returns the array capacity
	inline unsigned int capacity() const { return capacity_; }
	/// Sets a new size for the array (allowing for "holes")
	void setSize(unsigned int newSize);

	/// Clears the array
	void clear();
	/// Returns a constant reference to the first element in constant time
	inline const T &front() const { return array_[0]; }
	/// Returns a reference to the first element in constant time
	inline T &front() { return array_[0]; }
	/// Returns a constant reference to the last element in constant time
	inline const T &back() const { return array_[size_ - 1]; }
	/// Returns a reference to the last element in constant time
	inline T &back() { return array_[size_ - 1]; }
	/// Inserts a new element as the last one in constant time
	void pushBack(const T &element);
	/// Move inserts a new element as the last one in constant time
	void pushBack(T &&element);
	/// Constructs a new element as the last one in constant time
	template <typename... Args> void emplaceBack(Args &&... args);
	/// Removes the last element in constant time
	void popBack();
	/// Inserts new elements at the specified position from a source range, last not included (shifting elements around)
	T *insertRange(unsigned int index, const T *firstPtr, const T *lastPtr);
	/// Inserts a new element at a specified position (shifting elements around)
	T *insertAt(unsigned int index, const T &element);
	/// Move inserts a new element at a specified position (shifting elements around)
	T *insertAt(unsigned int index, T &&element);
	/// Constructs a new element at the position specified by the index
	template <typename... Args> T *emplaceAt(unsigned int index, Args &&... args);
	/// Inserts a new element at the position specified by the iterator (shifting elements around)
	Iterator insert(Iterator position, const T &value);
	/// Move inserts a new element at the position specified by the iterator (shifting elements around)
	Iterator insert(Iterator position, T &&value);
	/// Inserts new elements from a source at the position specified by the iterator (shifting elements around)
	Iterator insert(Iterator position, Iterator first, Iterator last);
	/// Constructs a new element at the position specified by the iterator
	template <typename... Args> Iterator emplace(Iterator position, Args &&... args);

	/// Removes the specified range of elements, last not included (shifting elements around)
	T *removeRange(unsigned int firstIndex, unsigned int lastIndex);
	/// Removes an element at a specified position (shifting elements around)
	inline Iterator removeAt(unsigned int index) { return Iterator(removeRange(index, index + 1)); }
	/// Removes the element pointed by the iterator (shifting elements around)
	Iterator erase(Iterator position);
	/// Removes the elements in the range, last not included (shifting elements around)
	Iterator erase(Iterator first, const Iterator last);

	/// Removes the specified range of elements, last not included (moving tail elements in place)
	T *unorderedRemoveRange(unsigned int firstIndex, unsigned int lastIndex);
	/// Removes an element at a specified position (moving the last element in place)
	inline Iterator unorderedRemoveAt(unsigned int index) { return Iterator(unorderedRemoveRange(index, index + 1)); }
	/// Removes the element pointed by the iterator (moving the last element in place)
	Iterator unorderedErase(Iterator position);
	/// Removes the elements in the range, last not included (moving tail elements in place)
	Iterator unorderedErase(Iterator first, const Iterator last);

	/// Read-only access to the specified element (with bounds checking)
	const T &at(unsigned int index) const;
	/// Access to the specified element (with bounds checking)
	T &at(unsigned int index);
	/// Read-only subscript operator
	const T &operator[](unsigned int index) const;
	/// Subscript operator
	T &operator[](unsigned int index);

	/// Returns a constant pointer to the allocated memory
	inline const T *data() const { return array_; }
	/// Returns a pointer to the allocated memory
	inline T *data() { return array_; }

  private:
	unsigned char arrayBuffer_[C * sizeof(T)];
	T *array_ = reinterpret_cast<T *>(arrayBuffer_);
	unsigned int size_;
	unsigned int capacity_;

	/// Grows the array size by one and returns a pointer to the new element
	T *extendOne();
};

template <class T, unsigned int C>
StaticArray<T, C>::StaticArray(const StaticArray<T, C> &other)
    : size_(other.size_), capacity_(other.capacity_)
{
	copyConstructArray(array_, other.array_, size_);
}

template <class T, unsigned int C>
StaticArray<T, C>::StaticArray(StaticArray<T, C> &&other)
    : size_(other.size_), capacity_(other.capacity_)
{
	moveConstructArray(array_, other.array_, size_);
	other.clear();
}

template <class T, unsigned int C>
StaticArray<T, C> &StaticArray<T, C>::operator=(const StaticArray<T, C> &other)
{
	if (this == &other)
		return *this;

	if (other.size_ > 0 && other.size_ >= size_)
	{
		copyAssignArray(array_, other.array_, size_);
		copyConstructArray(array_ + size_, other.array_ + size_, other.size_ - size_);
	}
	else if (size_ > 0 && size_ >= other.size_)
	{
		copyAssignArray(array_, other.array_, other.size_);
		destructArray(array_ + other.size_, size_ - other.size_);
	}

	size_ = other.size_;
	return *this;
}

template <class T, unsigned int C>
StaticArray<T, C> &StaticArray<T, C>::operator=(StaticArray<T, C> &&other)
{
	if (this == &other)
		return *this;

	if (other.size_ > 0 && other.size_ >= size_)
	{
		moveAssignArray(array_, other.array_, size_);
		moveConstructArray(array_ + size_, other.array_ + size_, other.size_ - size_);
	}
	else if (size_ > 0 && size_ >= other.size_)
	{
		moveAssignArray(array_, other.array_, other.size_);
		destructArray(array_ + other.size_, size_ - other.size_);
	}

	size_ = other.size_;
	other.clear();
	return *this;
}

template <class T, unsigned int C>
void StaticArray<T, C>::setSize(unsigned int newSize)
{
	const int newElements = newSize - size_;

	if (newSize > capacity_)
	{
		LOGW_X("Trying to extend the size of a static array beyond its cpacity, from from %u to %u", capacity_, newSize);
		return;
	}

	if (newElements > 0)
		constructArray(array_ + size_, newElements);
	else if (newElements < 0)
		destructArray(array_ + size_ + newElements, -newElements);
	size_ += newElements;
}

/*! Size will be set to zero but capacity remains unmodified. */
template <class T, unsigned int C>
void StaticArray<T, C>::clear()
{
	destructArray(array_, size_);
	size_ = 0;
}

template <class T, unsigned int C>
void StaticArray<T, C>::pushBack(const T &element)
{
	T *ptr = extendOne();
	if (ptr)
		new (ptr) T(element);
}

template <class T, unsigned int C>
void StaticArray<T, C>::pushBack(T &&element)
{
	T *ptr = extendOne();
	if (ptr)
		new (ptr) T(nctl::move(element));
}

template <class T, unsigned int C>
template <typename... Args>
void StaticArray<T, C>::emplaceBack(Args &&... args)
{
	T *ptr = extendOne();
	if (ptr)
		new (ptr) T(nctl::forward<Args>(args)...);
}

template <class T, unsigned int C>
void StaticArray<T, C>::popBack()
{
	if (size_ > 0)
	{
		destructObject(array_ + size_ - 1);
		size_--;
	}
}

template <class T, unsigned int C>
T *StaticArray<T, C>::insertRange(unsigned int index, const T *firstPtr, const T *lastPtr)
{
	// Cannot insert at more than one position after the last element
	FATAL_ASSERT_MSG_X(index <= size_, "Index %u is out of bounds (size: %u)", index, size_);
	FATAL_ASSERT_MSG_X(firstPtr <= lastPtr, "First pointer %p should precede or be equal to the last one %p", firstPtr, lastPtr);

	const unsigned int numElements = static_cast<unsigned int>(lastPtr - firstPtr);
	FATAL_ASSERT_MSG_X(size_ + numElements <= capacity_, "Can't add element beyond capacity (%d)", capacity_);

	// Backwards loop to account for overlapping areas
	for (unsigned int i = size_ - index; i > 0; i--)
		array_[index + numElements + i - 1] = nctl::move(array_[index + i - 1]);
	copyConstructArray(array_ + index, firstPtr, numElements);
	size_ += numElements;

	return (array_ + index + numElements);
}

template <class T, unsigned int C>
T *StaticArray<T, C>::insertAt(unsigned int index, const T &element)
{
	// Cannot insert at more than one position after the last element
	FATAL_ASSERT_MSG_X(index <= size_, "Index %u is out of bounds (size: %u)", index, size_);
	FATAL_ASSERT_MSG_X(size_ + 1 <= capacity_, "Can't add element beyond capacity (%d)", capacity_);

	if (index < size_)
	{
		// Constructing a new element by moving the last one
		new (array_ + size_) T(nctl::move(array_[size_ - 1]));
		// Backwards loop to account for overlapping areas
		for (unsigned int i = size_ - index - 1; i > 0; i--)
			array_[index + i] = nctl::move(array_[index + i - 1]);
		array_[index] = element;
	}
	else
		new (array_ + size_) T(element);
	size_++;

	return (array_ + index + 1);
}

template <class T, unsigned int C>
T *StaticArray<T, C>::insertAt(unsigned int index, T &&element)
{
	// Cannot insert at more than one position after the last element
	FATAL_ASSERT_MSG_X(index <= size_, "Index %u is out of bounds (size: %u)", index, size_);
	FATAL_ASSERT_MSG_X(size_ + 1 <= capacity_, "Can't add element beyond capacity (%d)", capacity_);

	if (index < size_)
	{
		// Constructing a new element by moving the last one
		new (array_ + size_) T(nctl::move(array_[size_ - 1]));
		// Backwards loop to account for overlapping areas
		for (unsigned int i = size_ - index - 1; i > 0; i--)
			array_[index + i] = nctl::move(array_[index + i - 1]);
		array_[index] = nctl::move(element);
	}
	else
		new (array_ + size_) T(nctl::move(element));
	size_++;

	return (array_ + index + 1);
}

template <class T, unsigned int C>
template <typename... Args>
T *StaticArray<T, C>::emplaceAt(unsigned int index, Args &&... args)
{
	// Cannot emplace at more than one position after the last element
	FATAL_ASSERT_MSG_X(index <= size_, "Index %u is out of bounds (size: %u)", index, size_);
	FATAL_ASSERT_MSG_X(size_ + 1 <= capacity_, "Can't add element beyond capacity (%d)", capacity_);

	if (index < size_)
	{
		// Constructing a new element by moving the last one
		new (array_ + size_) T(nctl::move(array_[size_ - 1]));
		// Backwards loop to account for overlapping areas
		for (unsigned int i = size_ - index - 1; i > 0; i--)
			array_[index + i] = nctl::move(array_[index + i - 1]);
		destructObject(array_ + index);
	}
	new (array_ + index) T(nctl::forward<Args>(args)...);
	size_++;

	return (array_ + index + 1);
}

template <class T, unsigned int C>
typename StaticArray<T, C>::Iterator StaticArray<T, C>::insert(Iterator position, const T &value)
{
	const unsigned int index = &(*position) - array_;
	T *nextElement = insertAt(index, value);

	return Iterator(nextElement);
}

template <class T, unsigned int C>
typename StaticArray<T, C>::Iterator StaticArray<T, C>::insert(Iterator position, T &&value)
{
	const unsigned int index = &(*position) - array_;
	T *nextElement = insertAt(index, nctl::move(value));

	return Iterator(nextElement);
}

template <class T, unsigned int C>
typename StaticArray<T, C>::Iterator StaticArray<T, C>::insert(Iterator position, Iterator first, Iterator last)
{
	const unsigned int index = static_cast<unsigned int>(&(*position) - array_);
	const T *firstPtr = &(*first);
	const T *lastPtr = &(*last);
	T *nextElement = insertRange(index, firstPtr, lastPtr);

	return Iterator(nextElement);
}

template <class T, unsigned int C>
template <typename... Args>
typename StaticArray<T, C>::Iterator StaticArray<T, C>::emplace(Iterator position, Args &&... args)
{
	const unsigned int index = &(*position) - array_;
	T *nextElement = emplaceAt(index, nctl::forward<Args>(args)...);

	return Iterator(nextElement);
}

template <class T, unsigned int C>
T *StaticArray<T, C>::removeRange(unsigned int firstIndex, unsigned int lastIndex)
{
	// Cannot remove past the last element
	FATAL_ASSERT_MSG_X(firstIndex < size_, "First index %u out of size range", firstIndex);
	FATAL_ASSERT_MSG_X(lastIndex <= size_, "Last index %u out of size range", lastIndex);
	FATAL_ASSERT_MSG_X(firstIndex <= lastIndex, "First index %u should precede or be equal to the last one %u", firstIndex, lastIndex);

	const unsigned int numElements = lastIndex - firstIndex;
	moveAssignArray(array_ + firstIndex, array_ + lastIndex, size_ - lastIndex);
	destructArray(array_ + size_ - numElements, numElements);
	size_ -= numElements;

	return (array_ + firstIndex);
}

template <class T, unsigned int C>
typename StaticArray<T, C>::Iterator StaticArray<T, C>::erase(Iterator position)
{
	const unsigned int index = static_cast<unsigned int>(&(*position) - array_);
	return removeAt(index);
}

template <class T, unsigned int C>
typename StaticArray<T, C>::Iterator StaticArray<T, C>::erase(Iterator first, const Iterator last)
{
	const unsigned int firstIndex = static_cast<unsigned int>(&(*first) - array_);
	const unsigned int lastIndex = static_cast<unsigned int>(&(*last) - array_);
	T *nextElement = removeRange(firstIndex, lastIndex);

	return Iterator(nextElement);
}

/*! \note This method is faster than `removeRange()` but it will not preserve the array order */
template <class T, unsigned int C>
T *StaticArray<T, C>::unorderedRemoveRange(unsigned int firstIndex, unsigned int lastIndex)
{
	// Cannot remove past the last element
	FATAL_ASSERT_MSG_X(firstIndex < size_, "First index %u out of size range", firstIndex);
	FATAL_ASSERT_MSG_X(lastIndex <= size_, "Last index %u out of size range", lastIndex);
	FATAL_ASSERT_MSG_X(firstIndex <= lastIndex, "First index %u should precede or be equal to the last one %u", firstIndex, lastIndex);

	const unsigned int numElements = lastIndex - firstIndex;
	for (unsigned int i = 0; i < numElements; i++)
		array_[firstIndex + i] = nctl::move(array_[size_ - i - 1]);
	destructArray(array_ + size_ - numElements, numElements);
	size_ -= numElements;

	return (array_ + firstIndex + 1);
}

/*! \note This method is faster than `erase()` but it will not preserve the array order */
template <class T, unsigned int C>
typename StaticArray<T, C>::Iterator StaticArray<T, C>::unorderedErase(Iterator position)
{
	const unsigned int index = static_cast<unsigned int>(&(*position) - array_);
	return unorderedRemoveAt(index);
}

/*! \note This method is faster than `erase()` but it will not preserve the array order */
template <class T, unsigned int C>
typename StaticArray<T, C>::Iterator StaticArray<T, C>::unorderedErase(Iterator first, const Iterator last)
{
	const unsigned int firstIndex = static_cast<unsigned int>(&(*first) - array_);
	const unsigned int lastIndex = static_cast<unsigned int>(&(*last) - array_);
	T *nextElement = unorderedRemoveRange(firstIndex, lastIndex);

	return Iterator(nextElement);
}

template <class T, unsigned int C>
const T &StaticArray<T, C>::at(unsigned int index) const
{
	FATAL_ASSERT_MSG_X(index < size_, "Index %u is out of bounds (size: %u)", index, size_);
	return operator[](index);
}

template <class T, unsigned int C>
T &StaticArray<T, C>::at(unsigned int index)
{
	// Avoid creating "holes" into the array
	FATAL_ASSERT_MSG_X(index <= size_, "Index %u is out of bounds (size: %u)", index, size_);
	return operator[](index);
}

template <class T, unsigned int C>
const T &StaticArray<T, C>::operator[](unsigned int index) const
{
	// TEMP
	FATAL_ASSERT_MSG_X(index < size_, "Index %u is out of bounds (size: %u)", index, size_);
	return array_[index];
}

template <class T, unsigned int C>
T &StaticArray<T, C>::operator[](unsigned int index)
{
	FATAL_ASSERT_MSG_X(index < size_, "Index %u is out of bounds (size: %u)", index, size_);
	return array_[index];
}

template <class T, unsigned int C>
T *StaticArray<T, C>::extendOne()
{
	if (size_ == capacity_)
		return nullptr;
	size_++;
	return array_ + size_ - 1;
}

}

#endif
