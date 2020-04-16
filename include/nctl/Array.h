#ifndef CLASS_NCTL_ARRAY
#define CLASS_NCTL_ARRAY

#include <new>
#include <ncine/common_macros.h>
#include "ArrayIterator.h"
#include "ReverseIterator.h"
#include "utility.h"

#include <ncine/config.h>
#if NCINE_WITH_ALLOCATORS
	#include "AllocManager.h"
	#include "IAllocator.h"
#endif

namespace nctl {

/// Construction modes for the `Array` class
/*! Declared outside the template class to use it without template parameters. */
enum class ArrayMode
{
	/// `Array` will have a growing capacity
	GROWING_CAPACITY,
	/// `Array` will have a fixed capacity
	FIXED_CAPACITY
};

/// A dynamic array based on templates that stores elements in the heap
template <class T>
class Array
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

	/// Constructs an array without allocating memory
	Array()
	    : Array(0, ArrayMode::GROWING_CAPACITY) {}
	/// Constructs an array with explicit capacity
	explicit Array(unsigned int capacity)
	    : Array(capacity, ArrayMode::GROWING_CAPACITY) {}
#if !NCINE_WITH_ALLOCATORS
	/// Constructs an array with explicit capacity and the option for it to be fixed
	Array(unsigned int capacity, ArrayMode mode);
#else
	/// Constructs an array with explicit capacity and the option for it to be fixed
	Array(unsigned int capacity, ArrayMode mode)
	    : Array(capacity, mode, theDefaultAllocator()) {}
	/// Constructs an array with explicit capacity and a custom allocator
	Array(unsigned int capacity, IAllocator &alloc)
	    : Array(capacity, ArrayMode::GROWING_CAPACITY, alloc) {}
	/// Constructs an array with explicit capacity, the option for it to be fixed, and a custom allocator
	Array(unsigned int capacity, ArrayMode mode, IAllocator &alloc);
#endif
	~Array();

	/// Copy constructor
	Array(const Array &other);
	/// Move constructor
	Array(Array &&other);
	/// Assignment operator
	Array &operator=(const Array &other);
	/// Move assignment operator
	Array &operator=(Array &&other);

	/// Swaps two arrays without copying their data
	inline void swap(Array &first, Array &second)
	{
#if NCINE_WITH_ALLOCATORS
		nctl::swap(first.alloc_, second.alloc_);
#endif
		nctl::swap(first.array_, second.array_);
		nctl::swap(first.size_, second.size_);
		nctl::swap(first.capacity_, second.capacity_);
		nctl::swap(first.fixedCapacity_, second.fixedCapacity_);
	}

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
	/*! The array has memory allocated to store until the `Capacity()`-1 element. */
	inline unsigned int capacity() const { return capacity_; }
	/// Sets a new capacity for the array (can be bigger or smaller than the current one)
	void setCapacity(unsigned int newCapacity);
	/// Sets a new size for the array (allowing for "holes")
	void setSize(unsigned int newSize);
	/// Decreases the capacity to match the current size of the array
	void shrinkToFit();

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
	/// Appends a new element in constant time, the element is copied into the array
	inline void pushBack(const T &element) { new (extendOne()) T(element); }
	/// Appends a new element in constant time, the element is moved into the array
	inline void pushBack(T &&element) { new (extendOne()) T(nctl::move(element)); }
	/// Constructs a new element at the end of the array
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
	/*! When adding new elements through a pointer the size field is not updated, like with `std::vector`. */
	inline T *data() { return array_; }

  private:
#if NCINE_WITH_ALLOCATORS
	/// The custom memory allocator for the array
	IAllocator &alloc_;
#endif
	T *array_;
	unsigned int size_;
	unsigned int capacity_;
	bool fixedCapacity_;

	/// Grows the array size by one and returns a pointer to the new element
	T *extendOne();
};

#if !NCINE_WITH_ALLOCATORS
template <class T>
Array<T>::Array(unsigned int capacity, ArrayMode mode)
    : array_(nullptr), size_(0), capacity_(0), fixedCapacity_(mode == ArrayMode::FIXED_CAPACITY)
{
	if (capacity > 0)
		setCapacity(capacity);
}
#else
template <class T>
Array<T>::Array(unsigned int capacity, ArrayMode mode, IAllocator &alloc)
    : alloc_(alloc), array_(nullptr), size_(0), capacity_(0),
      fixedCapacity_(mode == ArrayMode::FIXED_CAPACITY)
{
	if (capacity > 0)
		setCapacity(capacity);
}
#endif

template <class T>
Array<T>::~Array()
{
	destructArray(array_, size_);
#if !NCINE_WITH_ALLOCATORS
	::operator delete(array_);
#else
	alloc_.deallocate(array_);
#endif
}

template <class T>
Array<T>::Array(const Array<T> &other)
    :
#if NCINE_WITH_ALLOCATORS
      alloc_(other.alloc_),
#endif
      array_(nullptr), size_(other.size_), capacity_(other.capacity_), fixedCapacity_(other.fixedCapacity_)
{
#if !NCINE_WITH_ALLOCATORS
	array_ = static_cast<T *>(::operator new(capacity_ * sizeof(T)));
#else
	array_ = static_cast<T *>(alloc_.allocate(capacity_ * sizeof(T)));
#endif
	copyConstructArray(array_, other.array_, size_);
}

template <class T>
Array<T>::Array(Array<T> &&other)
    :
#if NCINE_WITH_ALLOCATORS
      alloc_(other.alloc_),
#endif
      array_(nullptr), size_(0), capacity_(0), fixedCapacity_(false)
{
	swap(*this, other);
}

template <class T>
Array<T> &Array<T>::operator=(const Array<T> &other)
{
	if (this == &other)
		return *this;

	if (other.size_ > capacity_)
		setCapacity(other.size_);

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

template <class T>
Array<T> &Array<T>::operator=(Array<T> &&other)
{
	if (this != &other)
	{
		swap(*this, other);
		other.clear();
	}
	return *this;
}

template <class T>
void Array<T>::setCapacity(unsigned int newCapacity)
{
	// If the call does not come from the constructor
	if (capacity_ != 0)
	{
		// Setting a new capacity is disabled if the array is fixed
		if (fixedCapacity_)
		{
			LOGW_X("Trying to change the capacity of a fixed array, from from %u to %u", capacity_, newCapacity);
			return;
		}

		if (newCapacity == capacity_)
		{
			LOGW_X("Array capacity already equal to %u", capacity_);
			return;
		}
		else if (newCapacity < capacity_)
			LOGI_X("Array capacity shrinking from %u to %u", capacity_, newCapacity);
		else if (newCapacity > capacity_)
			LOGD_X("Array capacity growing from %u to %u", capacity_, newCapacity);
	}

	T *newArray = nullptr;
	if (newCapacity > 0)
	{
#if !NCINE_WITH_ALLOCATORS
		newArray = static_cast<T *>(::operator new(newCapacity * sizeof(T)));
#else
		newArray = static_cast<T *>(alloc_.allocate(newCapacity * sizeof(T)));
#endif
	}

	if (size_ > 0)
	{
		const unsigned int oldSize = size_;
		if (newCapacity < size_) // shrinking
			size_ = newCapacity; // cropping last elements

		moveConstructArray(newArray, array_, size_);
		destructArray(array_, oldSize);
	}

#if !NCINE_WITH_ALLOCATORS
	::operator delete(array_);
#else
	alloc_.deallocate(array_);
#endif
	array_ = newArray;
	capacity_ = newCapacity;
}

template <class T>
void Array<T>::setSize(unsigned int newSize)
{
	const int newElements = newSize - size_;

	if (newElements < 0)
		destructArray(array_ + size_ + newElements, -newElements);

	if (newSize > capacity_)
	{
		setCapacity(newSize);
		// Extending size only if the capacity is not fixed
		if (capacity_ == newSize)
			size_ = newSize;
	}
	else
		size_ = newSize;
}

template <class T>
void Array<T>::shrinkToFit()
{
	if (size_ > 0)
		setCapacity(size_);
}

/*! Size will be set to zero but capacity remains unmodified. */
template <class T>
void Array<T>::clear()
{
	destructArray(array_, size_);
	size_ = 0;
}

template <class T>
template <typename... Args>
void Array<T>::emplaceBack(Args &&... args)
{
	new (extendOne()) T(nctl::forward<Args>(args)...);
}

template <class T>
void Array<T>::popBack()
{
	if (size_ > 0)
	{
		destructObject(array_ + size_ - 1);
		size_--;
	}
}

template <class T>
T *Array<T>::insertRange(unsigned int index, const T *firstPtr, const T *lastPtr)
{
	// Cannot insert at more than one position after the last element
	FATAL_ASSERT_MSG_X(index <= size_, "Index %u is out of bounds (size: %u)", index, size_);
	FATAL_ASSERT_MSG_X(firstPtr <= lastPtr, "First pointer %p should precede or be equal to the last one %p", firstPtr, lastPtr);

	const unsigned int numElements = static_cast<unsigned int>(lastPtr - firstPtr);

	if (size_ + numElements > capacity_)
		setCapacity((size_ + numElements) * 2);

	// Backwards loop to account for overlapping areas
	for (unsigned int i = size_ - index; i > 0; i--)
		array_[index + numElements + i - 1] = nctl::move(array_[index + i - 1]);
	copyConstructArray(array_ + index, firstPtr, numElements);
	size_ += numElements;

	return (array_ + index + numElements);
}

template <class T>
T *Array<T>::insertAt(unsigned int index, const T &element)
{
	// Cannot insert at more than one position after the last element
	FATAL_ASSERT_MSG_X(index <= size_, "Index %u is out of bounds (size: %u)", index, size_);

	if (size_ + 1 > capacity_)
	{
		const unsigned int newCapacity = (size_ == 0) ? 1 : size_ * 2;
		setCapacity(newCapacity);
	}

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

template <class T>
T *Array<T>::insertAt(unsigned int index, T &&element)
{
	// Cannot insert at more than one position after the last element
	FATAL_ASSERT_MSG_X(index <= size_, "Index %u is out of bounds (size: %u)", index, size_);

	if (size_ + 1 > capacity_)
	{
		const unsigned int newCapacity = (size_ == 0) ? 1 : size_ * 2;
		setCapacity(newCapacity);
	}

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

template <class T>
template <typename... Args>
T *Array<T>::emplaceAt(unsigned int index, Args &&... args)
{
	// Cannot emplace at more than one position after the last element
	FATAL_ASSERT_MSG_X(index <= size_, "Index %u is out of bounds (size: %u)", index, size_);

	if (size_ + 1 > capacity_)
	{
		const unsigned int newCapacity = (size_ == 0) ? 1 : size_ * 2;
		setCapacity(newCapacity);
	}

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

template <class T>
typename Array<T>::Iterator Array<T>::insert(Iterator position, const T &value)
{
	const unsigned int index = &(*position) - array_;
	T *nextElement = insertAt(index, value);

	return Iterator(nextElement);
}

template <class T>
typename Array<T>::Iterator Array<T>::insert(Iterator position, T &&value)
{
	const unsigned int index = &(*position) - array_;
	T *nextElement = insertAt(index, nctl::move(value));

	return Iterator(nextElement);
}

template <class T>
typename Array<T>::Iterator Array<T>::insert(Iterator position, Iterator first, Iterator last)
{
	const unsigned int index = static_cast<unsigned int>(&(*position) - array_);
	const T *firstPtr = &(*first);
	const T *lastPtr = &(*last);
	T *nextElement = insertRange(index, firstPtr, lastPtr);

	return Iterator(nextElement);
}

template <class T>
template <typename... Args>
typename Array<T>::Iterator Array<T>::emplace(Iterator position, Args &&... args)
{
	const unsigned int index = &(*position) - array_;
	T *nextElement = emplaceAt(index, nctl::forward<Args>(args)...);

	return Iterator(nextElement);
}

template <class T>
T *Array<T>::removeRange(unsigned int firstIndex, unsigned int lastIndex)
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

template <class T>
typename Array<T>::Iterator Array<T>::erase(Iterator position)
{
	const unsigned int index = static_cast<unsigned int>(&(*position) - array_);
	return removeAt(index);
}

template <class T>
typename Array<T>::Iterator Array<T>::erase(Iterator first, const Iterator last)
{
	const unsigned int firstIndex = static_cast<unsigned int>(&(*first) - array_);
	const unsigned int lastIndex = static_cast<unsigned int>(&(*last) - array_);
	T *nextElement = removeRange(firstIndex, lastIndex);

	return Iterator(nextElement);
}

/*! \note This method is faster than `removeRange()` but it will not preserve the array order */
template <class T>
T *Array<T>::unorderedRemoveRange(unsigned int firstIndex, unsigned int lastIndex)
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
template <class T>
typename Array<T>::Iterator Array<T>::unorderedErase(Iterator position)
{
	const unsigned int index = static_cast<unsigned int>(&(*position) - array_);
	return unorderedRemoveAt(index);
}

/*! \note This method is faster than `erase()` but it will not preserve the array order */
template <class T>
typename Array<T>::Iterator Array<T>::unorderedErase(Iterator first, const Iterator last)
{
	const unsigned int firstIndex = static_cast<unsigned int>(&(*first) - array_);
	const unsigned int lastIndex = static_cast<unsigned int>(&(*last) - array_);
	T *nextElement = unorderedRemoveRange(firstIndex, lastIndex);

	return Iterator(nextElement);
}

template <class T>
const T &Array<T>::at(unsigned int index) const
{
	FATAL_ASSERT_MSG_X(index < size_, "Index %u is out of bounds (size: %u)", index, size_);
	return operator[](index);
}

template <class T>
T &Array<T>::at(unsigned int index)
{
	FATAL_ASSERT_MSG_X(index < size_, "Index %u is out of bounds (size: %u)", index, size_);
	return operator[](index);
}

template <class T>
const T &Array<T>::operator[](unsigned int index) const
{
	ASSERT_MSG_X(index < size_, "Index %u is out of bounds (size: %u)", index, size_);
	return array_[index];
}

template <class T>
T &Array<T>::operator[](unsigned int index)
{
	ASSERT_MSG_X(index < size_, "Index %u is out of bounds (size: %u)", index, size_);
	return array_[index];
}

template <class T>
T *Array<T>::extendOne()
{
	// Need growing
	if (size_ == capacity_)
	{
		const unsigned int newCapacity = (capacity_ == 0) ? 1 : capacity_ * 2;
		setCapacity(newCapacity);
		// Extending size only if the capacity is not fixed
		if (capacity_ == newCapacity)
			size_++;
	}
	else
		size_++;

	return array_ + size_ - 1;
}

}

#endif
