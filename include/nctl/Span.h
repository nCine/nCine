#ifndef CLASS_NCTL_SPAN
#define CLASS_NCTL_SPAN

#include <cstddef> // for `size_t`
#include <ncine/common_macros.h>
#include <nctl/ArrayIterator.h>

namespace nctl {

/// A class that describes an object that can refer to a contiguous sequence of objects
template <class T>
class Span
{
  public:
	/// The type of the referred elements
	using ElementType = T;
	/// The element type with `const` and `volatile` qualifiers removed
	using ValueType = removeCv<T>;
	/// The type used for sizes and indices
	using SizeType = size_t;
	/// The type used for the difference between two iterators
	using DifferenceType = ptrDiffType;
	/// Pointer to an element
	using Pointer = T *;
	/// Constant pointer to an element
	using ConstPointer = const T *;
	/// Reference to an element
	using Reference = T &;
	/// Constant reference to an element
	using ConstReference = const T &;

	/// Iterator type
	using Iterator = ArrayIterator<T, false>;
	/// Constant iterator type
	using ConstIterator = ArrayIterator<T, true>;
	/// Reverse iterator type
	using ReverseIterator = nctl::ReverseIterator<Iterator>;
	/// Reverse constant iterator type
	using ConstReverseIterator = nctl::ReverseIterator<ConstIterator>;

	/// Constructs an empty span
	constexpr Span() noexcept
	    : data_(nullptr), size_(0) {}

	/// Constructs a span from a pointer and a size
	constexpr Span(Pointer data, SizeType size) noexcept
	    : data_(data), size_(size) {}

	/// Constructs a span from a fixed-size array
	template <SizeType N>
	constexpr Span(T (&array)[N]) noexcept
	    : data_(array), size_(N) {}

	/// Constructs a span from another span of a convertible element type
	template <class U, nctl::enableIfT<nctl::isConvertible<U *, T *>::value, int> = 0>
	constexpr Span(const Span<U> &other) noexcept
	    : data_(other.data()), size_(other.size()) {}

	/// Constructs a span from a range delimited by two pointers
	constexpr Span(Pointer first, Pointer last) noexcept
	    : data_(first), size_(last - first) {}

	/// Constructs a span from a range delimited by two iterators
	constexpr Span(ArrayIterator<T, true> first, ArrayIterator<T, true> last) noexcept
	{
		data_ = &(*first);
		size_ = last - first;
	}

	/// Returns an iterator to the first element
	inline Iterator begin() { return Iterator(data_); }
	/// Returns a reverse iterator to the last element
	inline ReverseIterator rBegin() { return ReverseIterator(end()); }
	/// Returns an iterator to past the last element
	inline Iterator end() { return Iterator(data_ + size_); }
	/// Returns a reverse iterator to prior the first element
	inline ReverseIterator rEnd() { return ReverseIterator(begin()); }

	/// Returns a constant iterator to the first element
	inline ConstIterator begin() const { return ConstIterator(data_); }
	/// Returns a constant reverse iterator to the last element
	inline ConstReverseIterator rBegin() const { return ConstReverseIterator(cEnd()); }
	/// Returns a constant iterator to past the last lement
	inline ConstIterator end() const { return ConstIterator(data_ + size_); }
	/// Returns a constant reverse iterator to prior the first element
	inline ConstReverseIterator rEnd() const { return ConstReverseIterator(cBegin()); }

	/// Returns a constant iterator to the first element
	inline ConstIterator cBegin() const { return ConstIterator(data_); }
	/// Returns a constant reverse iterator to the last element
	inline ConstReverseIterator crBegin() const { return ConstReverseIterator(cEnd()); }
	/// Returns a constant iterator to past the last lement
	inline ConstIterator cEnd() const { return ConstIterator(data_ + size_); }
	/// Returns a constant reverse iterator to prior the first element
	inline ConstReverseIterator crEnd() const { return ConstReverseIterator(cBegin()); }

	/// Returns a reference to the first element
	constexpr Reference front() const
	{
		FATAL_ASSERT_MSG(size_ > 0, "Cannot retrieve an element from an empty span");
		return data_[0];
	}
	/// Returns a reference to the last element
	constexpr Reference back() const
	{
		FATAL_ASSERT_MSG(size_ > 0, "Cannot retrieve an element from an empty span");
		return data_[size_ - 1];
	}

	/// Returns a reference to the element at the specified index, asserting if out of bounds
	CONSTEXPR14 Reference at(SizeType index) const
	{
		FATAL_ASSERT_MSG_X(index < size_, "Index %u is out of bounds (size: %u)", index, size_);
		return data_[index];
	}

	/// Returns a reference to the element at the specified index
	constexpr Reference operator[](SizeType index) const noexcept
	{
		ASSERT_MSG_X(index < size_, "Index %u is out of bounds (size: %u)", index, size_);
		return data_[index];
	}

	/// Returns a pointer to the underlying data
	constexpr Pointer data() const noexcept { return data_; }
	/// Returns the number of elements
	constexpr SizeType size() const noexcept { return size_; }
	/// Returns the size in bytes of the underlying data
	constexpr SizeType sizeBytes() const noexcept { return size_ * sizeof(T); }
	/// Returns true if the span is empty
	constexpr bool isEmpty() const noexcept { return size_ == 0; }

	/// Returns a subspan consisting of the first `count` elements
	CONSTEXPR14 Span first(SizeType count) const noexcept
	{
		ASSERT_MSG(count <= size_, "Subspan out of bounds");
		return Span(data_, count);
	}

	/// Returns a subspan consisting of the last `count` elements
	CONSTEXPR14 Span last(SizeType count) const noexcept
	{
		ASSERT_MSG(count <= size_, "Subspan out of bounds");
		return Span(data_ + size_ - count, count);
	}

	/// Returns a subspan of `count` elements starting at the specified offset
	CONSTEXPR14 Span subSpan(SizeType offset, SizeType count) const noexcept
	{
		ASSERT_MSG(offset + count <= size_, "Subspan out of bounds");
		return Span(data_ + offset, count);
	}

  private:
	Pointer data_;
	SizeType size_;
};

}

#endif
