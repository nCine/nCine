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
	using ElementType = T;
	using ValueType = removeCv<T>;
	using SizeType = size_t;
	using DifferenceType = ptrDiffType;
	using Pointer = T *;
	using ConstPointer = const T *;
	using Reference = T &;
	using ConstReference = const T &;

	/// Iterator type
	using Iterator = ArrayIterator<T, false>;
	/// Constant iterator type
	using ConstIterator = ArrayIterator<T, true>;
	/// Reverse iterator type
	using ReverseIterator = nctl::ReverseIterator<Iterator>;
	/// Reverse constant iterator type
	using ConstReverseIterator = nctl::ReverseIterator<ConstIterator>;

	constexpr Span() noexcept
	    : data_(nullptr), size_(0) {}

	constexpr Span(Pointer data, SizeType size) noexcept
	    : data_(data), size_(size) {}

	template <SizeType N>
	constexpr Span(T (&array)[N]) noexcept
	    : data_(array), size_(N) {}

	template <class U, nctl::enableIfT<nctl::isConvertible<U *, T *>::value, int> = 0>
	constexpr Span(const Span<U> &other) noexcept
	    : data_(other.data()), size_(other.size()) {}

	constexpr Span(Pointer first, Pointer last) noexcept
	    : data_(first), size_(last - first) {}

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

	constexpr Reference front() const
	{
		FATAL_ASSERT_MSG(size_ > 0, "Cannot retrieve an element from an empty span");
		return data_[0];
	}
	constexpr Reference back() const
	{
		FATAL_ASSERT_MSG(size_ > 0, "Cannot retrieve an element from an empty span");
		return data_[size_ - 1];
	}

	CONSTEXPR14 Reference at(SizeType index) const
	{
		FATAL_ASSERT_MSG_X(index < size_, "Index %u is out of bounds (size: %u)", index, size_);
		return data_[index];
	}

	constexpr Reference operator[](SizeType index) const noexcept
	{
		ASSERT_MSG_X(index < size_, "Index %u is out of bounds (size: %u)", index, size_);
		return data_[index];
	}

	constexpr Pointer data() const noexcept { return data_; }
	constexpr SizeType size() const noexcept { return size_; }
	constexpr SizeType sizeBytes() const noexcept { return size_ * sizeof(T); }
	constexpr bool isEmpty() const noexcept { return size_ == 0; }

	CONSTEXPR14 Span first(SizeType count) const noexcept
	{
		ASSERT_MSG(count <= size_, "Subspan out of bounds");
		return Span(data_, count);
	}

	CONSTEXPR14 Span last(SizeType count) const noexcept
	{
		ASSERT_MSG(count <= size_, "Subspan out of bounds");
		return Span(data_ + size_ - count, count);
	}

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
