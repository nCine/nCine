#ifndef CLASS_NCINE_ARRAYITERATOR
#define CLASS_NCINE_ARRAYITERATOR

namespace ncine {

/// An Array iterator
template <class T>
class ArrayIterator
{
  public:	
	explicit ArrayIterator(T *element)
		: element_(element) { }
	ArrayIterator(const ArrayIterator& iterator)
		: element_(iterator.element_) { }

	// Read-only deferencing operator
	inline const T& operator*() const;
	// Deferencing operator
	inline T& operator*();

	// Iterates to the next element (prefix)
	ArrayIterator<T> operator++() const;
	// Iterates to the next element (postfix)
	ArrayIterator<T> operator++(int) const;

	// Iterates to the previous element (prefix)
	ArrayIterator<T> operator--() const;
	// Iterates to the previous element (postfix)
	ArrayIterator<T> operator--(int) const;

	/// Equality operator
	inline bool operator==(const ArrayIterator<T>& iterator) const { return element_ == iterator.element_; }
	/// Inequality operator
	inline bool operator!=(const ArrayIterator<T>& iterator) const { return element_ != iterator.element_; }

  private:
	mutable T* element_;
};

/// Read-only deferencing operator
template <class T>
const T& ArrayIterator<T>::operator*() const
{
	return *element_;
}

/// Deferencing operator
template <class T>
T& ArrayIterator<T>::operator*()
{
	return *element_;
}

/// Iterates to the next element (prefix)
template <class T>
ArrayIterator<T> ArrayIterator<T>::operator++() const
{
	element_++;

	return *this;
}

/// Iterates to the next element (postfix)
template <class T>
ArrayIterator<T> ArrayIterator<T>::operator++(int) const
{
	// Create an unmodified copy to return
	ArrayIterator<T> iterator = *this;

	element_++;

	return iterator;
}

/// Iterates to the previous element (prefix)
template <class T>
ArrayIterator<T> ArrayIterator<T>::operator--() const
{
	element_--;

	return *this;
}

/// Iterates to the previous element (postfix)
template <class T>
ArrayIterator<T> ArrayIterator<T>::operator--(int) const
{
	// Create an unmodified copy to return
	ArrayIterator<T> iterator = *this;

	element_--;

	return iterator;
}

}

#endif
