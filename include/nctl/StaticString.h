#ifndef CLASS_NCTL_STATICSTRING
#define CLASS_NCTL_STATICSTRING

#include <ncine/common_macros.h>
#include "CString.h"
#include "Utf8.h"
#include "StringIterator.h"
#include "ReverseIterator.h"
#include "algorithms.h"

namespace nctl {

/// A basic string class made of chars stored on the stack
template <unsigned int Capacity>
class StaticString
{
  public:
	/// Iterator type
	using Iterator = StringIterator<false>;
	/// Constant iterator type
	using ConstIterator = StringIterator<true>;
	/// Reverse iterator type
	using ReverseIterator = nctl::ReverseIterator<Iterator>;
	/// Reverse constant iterator type
	using ConstReverseIterator = nctl::ReverseIterator<ConstIterator>;

	/// Maximum length when creating an object from C-style strings
	static constexpr unsigned int MaxCStringLength = 512 - 1;

	/// Default constructor
	StaticString();
	/// Constructs a string object from a C string
	StaticString(const char *cString);

	/// Copy constructor
	StaticString(const StaticString &other);
	/// Conversion constructor from a string of different capacity
	template <unsigned int Capacity2>
	StaticString(const StaticString<Capacity2> &other);
	/// Assignment operator (it might extend or truncate the original text)
	StaticString &operator=(const StaticString &other);
	/// Assignment operator (it might extend or truncate the original text)
	template <unsigned int Capacity2>
	StaticString &operator=(const StaticString<Capacity2> &other);
	/// Assigns a constant C string to the string object
	StaticString &operator=(const char *cString);

	/// Returns an iterator to the beginning
	inline Iterator begin() { return Iterator(data()); }
	/// Returns a reverse iterator to the beginning
	inline ReverseIterator rBegin() { return ReverseIterator(end()); }
	/// Returns an iterator to the end
	inline Iterator end() { return Iterator(data() + length_); }
	/// Returns a reverse iterator to the end
	inline ReverseIterator rEnd() { return ReverseIterator(begin()); }

	/// Returns a constant iterator to the beginning
	inline ConstIterator begin() const { return ConstIterator(data()); }
	/// Returns a constant reverse iterator to the beginning
	inline ConstReverseIterator rBegin() const { return ConstReverseIterator(cEnd()); }
	/// Returns a constant iterator to the end
	inline ConstIterator end() const { return ConstIterator(data() + length_); }
	/// Returns a constant reverse iterator to the end
	inline ConstReverseIterator rEnd() const { return ConstReverseIterator(cBegin()); }

	/// Returns a constant iterator to the beginning
	inline ConstIterator cBegin() const { return ConstIterator(data()); }
	/// Returns a constant reverse iterator to the beginning
	inline ConstReverseIterator crBegin() const { return ConstReverseIterator(cEnd()); }
	/// Returns a constant iterator to the end
	inline ConstIterator cEnd() const { return ConstIterator(data() + length_); }
	/// Returns a constant reverse iterator to the end
	inline ConstReverseIterator crEnd() const { return ConstReverseIterator(cBegin()); }

	/// Returns true if the string is empty
	inline bool isEmpty() const { return length_ == 0; }
	/// Returns the string length
	inline unsigned int length() const { return length_; }
	/// Returns the string capacity
	inline unsigned int capacity() const { return Capacity; }
	/// Sets the string length
	unsigned int setLength(unsigned int newLength);

	/// Clears the string
	void clear();

	/// Returns a pointer to the internal array
	inline char *data() { return array_; }
	/// Returns a constant pointer to the internal array
	inline const char *data() const { return array_; }

	/// Replaces characters from somewhere in the other string to somewhere in this one (no truncation)
	unsigned int replace(const StaticString &source, unsigned int srcChar, unsigned int numChar, unsigned int destChar);
	/// Replaces characters from somewhere in the other string to somewhere in this one (no truncation)
	template <unsigned int Capacity2>
	unsigned int replace(const StaticString<Capacity2> &source, unsigned int srcChar, unsigned int numChar, unsigned int destChar);
	/// Replaces characters from a C string to somewhere in this one (no truncation)
	unsigned int replace(const char *source, unsigned int numChar, unsigned int destChar);

	/// Copies characters from somewhere in the other string to the beginning of this one
	unsigned int assign(const StaticString &source, unsigned int srcChar, unsigned int numChar);
	/// Copies characters from somewhere in the other string to the beginning of this one
	template <unsigned int Capacity2>
	unsigned int assign(const StaticString<Capacity2> &source, unsigned int srcChar, unsigned int numChar);
	/// Copies all characters from the other string to the beginning of this one
	unsigned int assign(const StaticString &source);
	/// Copies all characters from the other string to the beginning of this one
	template <unsigned int Capacity2>
	unsigned int assign(const StaticString<Capacity2> &source);
	/// Copies characters from a C string to the beginning of this one
	unsigned int assign(const char *source, unsigned int numChar);

	/// Copies characters from somewhere in this string to a C string
	unsigned int copy(char *dest, unsigned int srcChar, unsigned int numChar) const;
	/// Copies all characters from this string to a C string
	unsigned int copy(char *dest) const;

	/// Appends all the characters from the other string to the end of this one
	unsigned int append(const StaticString &other);
	/// Appends all the characters from the other string to the end of this one
	template <unsigned int Capacity2>
	unsigned int append(const StaticString<Capacity2> &other);
	/// Appends all the characters from the C string to the end of this one
	unsigned int append(const char *cString);

	/// Compares the string with another one in lexicographical order
	int compare(const StaticString &other) const;
	/// Compares the string with another one in lexicographical order
	template <unsigned int Capacity2>
	int compare(const StaticString<Capacity2> &other) const;
	/// Compares the string with a constant C string in lexicographical order
	int compare(const char *cString) const;

	/// Finds the first occurrence of a character
	int findFirstChar(char c) const;
	/// Finds the last occurrence of a character
	int findLastChar(char c) const;
	/// Finds the first occurrence of a character after the character at the index
	int findFirstCharAfterIndex(char c, unsigned int index) const;

	/// Finds the first occurrence of the given string
	int find(const StaticString &other) const;
	/// Finds the first occurrence of the given string
	template <unsigned int Capacity2>
	int find(const StaticString<Capacity2> &other) const;
	/// Finds the first occurrence of the given constant C string
	int find(const char *cString) const;

	/// Replaces the string with the formatted result
	StaticString &format(const char *fmt, ...);
	/// Append the formatted result to the string
	StaticString &formatAppend(const char *fmt, ...);

	/// Appends another string to this one
	StaticString &operator+=(const StaticString &other);
	/// Appends another string to this one
	template <unsigned int Capacity2>
	StaticString &operator+=(const StaticString<Capacity2> &other);
	/// Appends a constant C string to the string object
	StaticString &operator+=(const char *cString);
	/// Concatenate two strings together to create a third one
	StaticString operator+(const StaticString &other) const;
	/// Concatenate two strings together to create a third one
	template <unsigned int Capacity2>
	StaticString operator+(const StaticString<Capacity2> &other) const;
	/// Concatenates a string with a constant C string to create a third one
	StaticString operator+(const char *cString) const;

	/// Friend operator to concatenate a constant C string with a string to create a third one
	template <unsigned int Capacity2>
	friend StaticString<Capacity2> operator+(const char *cString, const StaticString<Capacity2> &string);

	inline bool operator==(const StaticString &other) const { return (length_ != other.length_) ? false : (compare(other) == 0); }
	inline bool operator!=(const StaticString &other) const { return (length_ != other.length_) ? true : (compare(other) != 0); }
	inline bool operator>(const StaticString &other) const { return compare(other) > 0; }
	inline bool operator<(const StaticString &other) const { return compare(other) < 0; }
	inline bool operator>=(const StaticString &other) const { return compare(other) >= 0; }
	inline bool operator<=(const StaticString &other) const { return compare(other) <= 0; }

	template <unsigned int Capacity2>
	inline bool operator==(const StaticString<Capacity2> &other) const { return (length_ != other.length()) ? false : (compare(other) == 0); }
	template <unsigned int Capacity2>
	inline bool operator!=(const StaticString<Capacity2> &other) const { return (length_ != other.length()) ? true : (compare(other) != 0); }
	template <unsigned int Capacity2>
	inline bool operator>(const StaticString<Capacity2> &other) const { return compare(other) > 0; }
	template <unsigned int Capacity2>
	inline bool operator<(const StaticString<Capacity2> &other) const { return compare(other) < 0; }
	template <unsigned int Capacity2>
	inline bool operator>=(const StaticString<Capacity2> &other) const { return compare(other) >= 0; }
	template <unsigned int Capacity2>
	inline bool operator<=(const StaticString<Capacity2> &other) const { return compare(other) <= 0; }

	inline bool operator==(const char *cString) const { return compare(cString) == 0; }
	inline bool operator!=(const char *cString) const { return compare(cString) != 0; }
	inline bool operator>(const char *cString) const { return compare(cString) > 0; }
	inline bool operator<(const char *cString) const { return compare(cString) < 0; }
	inline bool operator>=(const char *cString) const { return compare(cString) >= 0; }
	inline bool operator<=(const char *cString) const { return compare(cString) <= 0; }

	friend inline bool operator==(const char *cString, const StaticString &string) { return string.compare(cString) == 0; }
	friend inline bool operator!=(const char *cString, const StaticString &string) { return string.compare(cString) != 0; }
	friend inline bool operator>(const char *cString, const StaticString &string) { return string.compare(cString) <= 0; }
	friend inline bool operator<(const char *cString, const StaticString &string) { return string.compare(cString) >= 0; }
	friend inline bool operator>=(const char *cString, const StaticString &string) { return string.compare(cString) < 0; }
	friend inline bool operator<=(const char *cString, const StaticString &string) { return string.compare(cString) > 0; }

	/// Read-only access to the specified element (with bounds checking)
	const char &at(unsigned int index) const;
	/// Access to the specified element (with bounds checking)
	char &at(unsigned int index);
	/// Read-only subscript operator
	const char &operator[](unsigned int index) const;
	/// Subscript operator
	char &operator[](unsigned int index);

	/// Retrieves the Unicode code point and the UTF-8 code units from the UTF-8 substring starting at the specified position
	/*! \returns The number of code units used by UTF-8 to encode the Unicode code point */
	int utf8ToCodePoint(unsigned int position, unsigned int &codePoint, unsigned int *codeUnits) const;

	/// Retrieves the Unicode code point from the UTF-8 substring starting at the specified position
	/*! \returns The number of code units used by UTF-8 to encode the Unicode code point */
	int utf8ToCodePoint(unsigned int position, unsigned int &codePoint) const;

  private:
	char array_[Capacity];
	unsigned int length_;
};

template <unsigned int Capacity>
StaticString<Capacity>::StaticString()
    : length_(0)
{
	array_[0] = '\0';
}

template <unsigned int Capacity>
StaticString<Capacity>::StaticString(const char *cString)
    : length_(0)
{
	ASSERT(cString);
	length_ = static_cast<unsigned int>(nctl::strnlen(cString, Capacity - 1));

	nctl::strncpy(array_, Capacity, cString, length_);
	array_[length_] = '\0';
}

template <unsigned int Capacity>
StaticString<Capacity>::StaticString(const StaticString &other)
    : length_(other.length_)
{
	nctl::strncpy(array_, Capacity, other.array_, length_);
	array_[length_] = '\0';
}

template <unsigned int Capacity>
template <unsigned int Capacity2>
StaticString<Capacity>::StaticString(const StaticString<Capacity2> &other)
    : length_(min(other.length(), Capacity - 1))
{
	nctl::strncpy(array_, Capacity, other.data(), length_);
	array_[length_] = '\0';
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/*! \note Not implemented with the copy-and-swap idiom because it has to copy data. */
template <unsigned int Capacity>
StaticString<Capacity> &StaticString<Capacity>::operator=(const StaticString &other)
{
	if (this != &other)
		assign(other);

	return *this;
}

/*! \note Not implemented with the copy-and-swap idiom because it has to copy data. */
template <unsigned int Capacity>
template <unsigned int Capacity2>
StaticString<Capacity> &StaticString<Capacity>::operator=(const StaticString<Capacity2> &other)
{
	assign(other);
	return *this;
}

template <unsigned int Capacity>
StaticString<Capacity> &StaticString<Capacity>::operator=(const char *cString)
{
	ASSERT(cString);

	const unsigned int cLength = static_cast<unsigned int>(nctl::strnlen(cString, Capacity));
	length_ = min(cLength, Capacity - 1);
	nctl::strncpy(array_, Capacity, cString, length_);

	array_[length_] = '\0';
	return *this;
}

/*! The method is useful to update the string length after writing into it through the `data()` pointer. */
template <unsigned int Capacity>
unsigned int StaticString<Capacity>::setLength(unsigned int newLength)
{
	length_ = (newLength > Capacity - 1) ? Capacity - 1 : newLength;
	return length_;
}

/*! Length will be reset to zero but capacity remains unmodified. */
template <unsigned int Capacity>
void StaticString<Capacity>::clear()
{
	length_ = 0;
	array_[0] = '\0';
}

/*! The method returns the number of characters copied, to allow truncation. */
template <unsigned int Capacity>
unsigned int StaticString<Capacity>::replace(const StaticString &source, unsigned int srcChar, unsigned int numChar, unsigned int destChar)
{
	// Clamping parameters to string lengths and capacities

	// Cannot copy from beyond the end of the source string
	const unsigned int clampedSrcChar = min(srcChar, source.length_);
	const char *srcStart = source.array_ + clampedSrcChar;
	// It is possible to write beyond the end of the destination string, but without creating holes
	const unsigned int clampedDestChar = min(destChar, length_);

	// Cannot copy more characters than the source has left until its length or more than the destination has until its capacity
	const unsigned int charsToCopy = min(min(numChar, source.length_ - clampedSrcChar), Capacity - clampedDestChar - 1);

	if (charsToCopy > 0)
	{
		char *destStart = array_ + clampedDestChar;

		nctl::strncpy(destStart, Capacity - clampedDestChar, srcStart, charsToCopy);
		// Destination string length can only grow, truncation has to be performed by the calling function using the return value
		length_ = max(length_, static_cast<unsigned int>(destStart - array_) + charsToCopy);
		array_[length_] = '\0';
	}

	return charsToCopy;
}

/*! The method returns the number of characters copied, to allow truncation. */
template <unsigned int Capacity>
template <unsigned int Capacity2>
unsigned int StaticString<Capacity>::replace(const StaticString<Capacity2> &source, unsigned int srcChar, unsigned int numChar, unsigned int destChar)
{
	// Clamping parameters to string lengths and capacities

	// Cannot copy from beyond the end of the source string
	const unsigned int clampedSrcChar = min(srcChar, source.length());
	const char *srcStart = source.data() + clampedSrcChar;
	// It is possible to write beyond the end of the destination string, but without creating holes
	const unsigned int clampedDestChar = min(destChar, length_);

	// Cannot copy more characters than the source has left until its length or more than the destination has until its capacity
	const unsigned int charsToCopy = min(min(numChar, source.length() - clampedSrcChar), Capacity - clampedDestChar - 1);

	if (charsToCopy > 0)
	{
		char *destStart = array_ + clampedDestChar;

		nctl::strncpy(destStart, Capacity - clampedDestChar, srcStart, charsToCopy);
		// Destination string length can only grow, truncation has to be performed by the calling function using the return value
		length_ = max(length_, static_cast<unsigned int>(destStart - array_) + charsToCopy);
		array_[length_] = '\0';
	}

	return charsToCopy;
}

template <unsigned int Capacity>
unsigned int StaticString<Capacity>::replace(const char *source, unsigned int numChar, unsigned int destChar)
{
	ASSERT(source);

	// It is possible to write beyond the end of the destination string, but without creating holes
	const unsigned int clampedDestChar = min(destChar, length_);

	const unsigned int sourceLength = nctl::strnlen(source, MaxCStringLength);
	// Cannot copy more characters than the source has left until its length or more than the destination has until its capacity
	const unsigned int charsToCopy = min(min(numChar, sourceLength), Capacity - clampedDestChar - 1);

	if (charsToCopy > 0)
	{
		char *destStart = array_ + clampedDestChar;

		nctl::strncpy(destStart, Capacity - clampedDestChar, source, charsToCopy);
		// Destination string length can only grow, truncation has to be performed by the calling function using the return value
		length_ = max(length_, static_cast<unsigned int>(destStart - array_) + charsToCopy);
		array_[length_] = '\0';
	}

	return charsToCopy;
}

template <unsigned int Capacity>
unsigned int StaticString<Capacity>::assign(const StaticString &source, unsigned int srcChar, unsigned int numChar)
{
	const unsigned int copiedChars = replace(source, srcChar, numChar, 0);
	length_ = copiedChars;
	array_[length_] = '\0';
	return copiedChars;
}

template <unsigned int Capacity>
template <unsigned int Capacity2>
unsigned int StaticString<Capacity>::assign(const StaticString<Capacity2> &source, unsigned int srcChar, unsigned int numChar)
{
	const unsigned int copiedChars = replace(source, srcChar, numChar, 0);
	length_ = copiedChars;
	array_[length_] = '\0';
	return copiedChars;
}

template <unsigned int Capacity>
unsigned int StaticString<Capacity>::assign(const StaticString &source)
{
	return assign(source, 0, source.length_);
}

template <unsigned int Capacity>
template <unsigned int Capacity2>
unsigned int StaticString<Capacity>::assign(const StaticString<Capacity2> &source)
{
	return assign(source, 0, source.length());
}

template <unsigned int Capacity>
unsigned int StaticString<Capacity>::assign(const char *source, unsigned int numChar)
{
	const unsigned int copiedChars = replace(source, numChar, 0);
	length_ = copiedChars;
	array_[length_] = '\0';
	return copiedChars;
}

template <unsigned int Capacity>
unsigned int StaticString<Capacity>::copy(char *dest, unsigned int srcChar, unsigned int numChar) const
{
	ASSERT(dest);

	// Cannot copy from beyond the end of the source string
	const unsigned int clampedSrcChar = min(srcChar, length_);
	const char *srcStart = array_ + clampedSrcChar;
	// Cannot copy more characters than the source has left until its length
	const unsigned int charsToCopy = min(numChar, length_ - clampedSrcChar);

	// Always assuming that the destination is big enough
	if (charsToCopy > 0)
	{
		nctl::strncpy(dest, srcStart, charsToCopy);
		dest[charsToCopy] = '\0';
	}

	return charsToCopy;
}

template <unsigned int Capacity>
unsigned int StaticString<Capacity>::copy(char *dest) const
{
	return copy(dest, 0, length_);
}

template <unsigned int Capacity>
unsigned int StaticString<Capacity>::append(const StaticString &other)
{
	return replace(other, 0, other.length_, length_);
}

template <unsigned int Capacity>
template <unsigned int Capacity2>
unsigned int StaticString<Capacity>::append(const StaticString<Capacity2> &other)
{
	return replace(other, 0, other.length(), length_);
}

template <unsigned int Capacity>
unsigned int StaticString<Capacity>::append(const char *cString)
{
	return replace(cString, static_cast<unsigned int>(nctl::strnlen(cString, MaxCStringLength)), length_);
}

template <unsigned int Capacity>
int StaticString<Capacity>::compare(const StaticString &other) const
{
	return strncmp(data(), other.data(), Capacity);
}

template <unsigned int Capacity>
template <unsigned int Capacity2>
int StaticString<Capacity>::compare(const StaticString<Capacity2> &other) const
{
	const unsigned int minCapacity = nctl::min(Capacity, Capacity2);
	return strncmp(data(), other.data(), minCapacity);
}

template <unsigned int Capacity>
int StaticString<Capacity>::compare(const char *cString) const
{
	ASSERT(cString);
	return strncmp(data(), cString, Capacity);
}

template <unsigned int Capacity>
int StaticString<Capacity>::findFirstChar(char c) const
{
	const char *foundPtr = strchr(array_, c);

	if (foundPtr)
		return static_cast<int>(foundPtr - array_);
	else
		return -1;
}

template <unsigned int Capacity>
int StaticString<Capacity>::findLastChar(char c) const
{
	const char *foundPtr = strrchr(array_, c);

	if (foundPtr)
		return static_cast<int>(foundPtr - array_);
	else
		return -1;
}

template <unsigned int Capacity>
int StaticString<Capacity>::findFirstCharAfterIndex(char c, unsigned int index) const
{
	if (length_ == 0 || index >= length_ - 1)
		return -1;

	const char *foundPtr = strchr(array_ + index + 1, c);

	if (foundPtr)
		return static_cast<int>(foundPtr - array_);
	else
		return -1;
}

template <unsigned int Capacity>
int StaticString<Capacity>::find(const StaticString &other) const
{
	const char *foundPtr = strstr(data(), other.data());

	if (foundPtr)
		return static_cast<int>(foundPtr - data());
	else
		return -1;
}

template <unsigned int Capacity>
template <unsigned int Capacity2>
int StaticString<Capacity>::find(const StaticString<Capacity2> &other) const
{
	const char *foundPtr = strstr(data(), other.data());

	if (foundPtr)
		return static_cast<int>(foundPtr - data());
	else
		return -1;
}

template <unsigned int Capacity>
int StaticString<Capacity>::find(const char *cString) const
{
	ASSERT(cString);
	const char *foundPtr = strstr(data(), cString);

	if (foundPtr)
		return static_cast<int>(foundPtr - data());
	else
		return -1;
}

template <unsigned int Capacity>
StaticString<Capacity> &StaticString<Capacity>::format(const char *fmt, ...)
{
	ASSERT(fmt);

	int formattedLength = 0;
	va_list args;
	va_start(args, fmt);
	formattedLength = nctl::vsnprintfTrunc(data(), Capacity, fmt, args);
	va_end(args);

#if defined(_WIN32) && !defined(__MINGW32__)
	if (formattedLength < 0)
		formattedLength = Capacity - 1;
#endif

	if (formattedLength > 0)
	{
		length_ = nctl::min(Capacity - 1, static_cast<unsigned int>(formattedLength));
		array_[length_] = '\0';
	}

	return *this;
}

template <unsigned int Capacity>
StaticString<Capacity> &StaticString<Capacity>::formatAppend(const char *fmt, ...)
{
	ASSERT(fmt);

	int formattedLength = 0;
	va_list args;
	va_start(args, fmt);
	formattedLength = nctl::vsnprintfTrunc(data() + length_, Capacity - length_, fmt, args);
	va_end(args);

#if defined(_WIN32) && !defined(__MINGW32__)
	if (formattedLength < 0)
		formattedLength = Capacity - 1;
#endif

	if (formattedLength > 0)
	{
		length_ += min(Capacity - length_ - 1, static_cast<unsigned int>(formattedLength));
		array_[length_] = '\0';
	}

	return *this;
}

template <unsigned int Capacity>
StaticString<Capacity> &StaticString<Capacity>::operator+=(const StaticString &other)
{
	const unsigned int minLength = min(other.length_, Capacity - length_ - 1);

	nctl::strncpy(array_ + length_, Capacity - length_, other.array_, minLength);
	length_ += minLength;

	array_[length_] = '\0';
	return *this;
}

template <unsigned int Capacity>
template <unsigned int Capacity2>
StaticString<Capacity> &StaticString<Capacity>::operator+=(const StaticString<Capacity2> &other)
{
	const unsigned int minLength = min(other.length(), Capacity - length_ - 1);

	nctl::strncpy(array_ + length_, Capacity - length_, other.data(), minLength);
	length_ += minLength;

	array_[length_] = '\0';
	return *this;
}

template <unsigned int Capacity>
StaticString<Capacity> &StaticString<Capacity>::operator+=(const char *cString)
{
	ASSERT(cString);
	const unsigned int otherLength = static_cast<unsigned int>(nctl::strnlen(cString, MaxCStringLength));
	const unsigned int minLength = min(otherLength, Capacity - length_ - 1);

	nctl::strncpy(array_ + length_, Capacity - length_, cString, minLength);
	length_ += minLength;

	array_[length_] = '\0';
	return *this;
}

template <unsigned int Capacity>
StaticString<Capacity> StaticString<Capacity>::operator+(const StaticString &other) const
{
	StaticString<Capacity> result;

	result = *this;
	result += other;

	return result;
}

template <unsigned int Capacity>
template <unsigned int Capacity2>
StaticString<Capacity> StaticString<Capacity>::operator+(const StaticString<Capacity2> &other) const
{
	StaticString<Capacity> result;

	result = *this;
	result += other;

	return result;
}

template <unsigned int Capacity>
StaticString<Capacity> StaticString<Capacity>::operator+(const char *cString) const
{
	ASSERT(cString);
	StaticString<Capacity> result;

	result = *this;
	result += cString;

	return result;
}

template <unsigned int Capacity2>
StaticString<Capacity2> operator+(const char *cString, const StaticString<Capacity2> &string)
{
	ASSERT(cString);
	StaticString<Capacity2> result;

	result = cString;
	result += string;

	return result;
}

template <unsigned int Capacity>
const char &StaticString<Capacity>::at(unsigned int index) const
{
	FATAL_ASSERT_MSG_X(index < length_, "Index %u is out of bounds (length: %u)", index, length_);
	return operator[](index);
}

template <unsigned int Capacity>
char &StaticString<Capacity>::at(unsigned int index)
{
	FATAL_ASSERT_MSG_X(index < length_, "Index %u is out of bounds (length: %u)", index, length_);
	return operator[](index);
}

template <unsigned int Capacity>
const char &StaticString<Capacity>::operator[](unsigned int index) const
{
	ASSERT_MSG_X(index < length_, "Index %u is out of bounds (length: %u)", index, length_);
	return array_[index];
}

template <unsigned int Capacity>
char &StaticString<Capacity>::operator[](unsigned int index)
{
	ASSERT_MSG_X(index < length_, "Index %u is out of bounds (size: %u)", index, length_);
	return array_[index];
}

template <unsigned int Capacity>
int StaticString<Capacity>::utf8ToCodePoint(unsigned int position, unsigned int &codePoint, unsigned int *codeUnits) const
{
	if (position + 1 > length_)
	{
		codePoint = Utf8::InvalidUnicode;
		if (codeUnits)
			*codeUnits = Utf8::InvalidUtf8;
		return 0;
	}

	const char *subString = Utf8::utf8ToCodePoint(&operator[](position), codePoint, codeUnits);
	return (subString - data() - position);
}

template <unsigned int Capacity>
int StaticString<Capacity>::utf8ToCodePoint(unsigned int position, unsigned int &codePoint) const
{
	return utf8ToCodePoint(position, codePoint, nullptr);
}

}

#endif
