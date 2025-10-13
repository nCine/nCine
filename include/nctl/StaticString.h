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
template <unsigned int C>
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
	static const unsigned int MaxCStringLength = 512 - 1;

	/// Default constructor
	StaticString();
	/// Constructs a string object from a C string
	StaticString(const char *cString);

	/// Copy constructor
	StaticString(const StaticString &other);
	/// Conversion constructor from a string of different capacity
	template <unsigned int U>
	StaticString(const StaticString<U> &other);
	/// Assignment operator (it might extend or truncate the original text)
	StaticString &operator=(const StaticString &other);
	/// Assignment operator (it might extend or truncate the original text)
	template <unsigned int U>
	StaticString &operator=(const StaticString<U> &other);
	/// Assigns a constant C string to the string object
	StaticString &operator=(const char *cString);

	/// Returns an iterator to the first character
	inline Iterator begin() { return Iterator(data()); }
	/// Returns a reverse iterator to the last character
	inline ReverseIterator rBegin() { return ReverseIterator(Iterator(data() + length_ - 1)); }
	/// Returns an iterator to the termination character
	inline Iterator end() { return Iterator(data() + length_); }
	/// Returns a reverse iterator to the byte preceding the first character
	inline ReverseIterator rEnd() { return ReverseIterator(Iterator(data() - 1)); }

	/// Returns a constant iterator to the first character
	inline ConstIterator begin() const { return ConstIterator(data()); }
	/// Returns a constant reverse iterator to the last character
	inline ConstReverseIterator rBegin() const { return ConstReverseIterator(ConstIterator(data() + length_ - 1)); }
	/// Returns a constant iterator to the termination character
	inline ConstIterator end() const { return ConstIterator(data() + length_); }
	/// Returns a constant reverse iterator to the byte preceding the first character
	inline ConstReverseIterator rEnd() const { return ConstReverseIterator(ConstIterator(data() - 1)); }

	/// Returns a constant iterator to the first character
	inline ConstIterator cBegin() const { return ConstIterator(data()); }
	/// Returns a constant reverse iterator to the last character
	inline ConstReverseIterator crBegin() const { return ConstReverseIterator(ConstIterator(data() + length_ - 1)); }
	/// Returns a constant iterator to the termination character
	inline ConstIterator cEnd() const { return ConstIterator(data() + length_); }
	/// Returns a constant reverse iterator to the byte preceding the first character
	inline ConstReverseIterator crEnd() const { return ConstReverseIterator(ConstIterator(data() - 1)); }

	/// Returns true if the string is empty
	inline bool isEmpty() const { return length_ == 0; }
	/// Returns the string length
	inline unsigned int length() const { return length_; }
	/// Returns the string capacity
	inline unsigned int capacity() const { return capacity_; }
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
	template <unsigned int U>
	unsigned int replace(const StaticString<U> &source, unsigned int srcChar, unsigned int numChar, unsigned int destChar);
	/// Replaces characters from a C string to somewhere in this one (no truncation)
	unsigned int replace(const char *source, unsigned int numChar, unsigned int destChar);

	/// Copies characters from somewhere in the other string to the beginning of this one
	unsigned int assign(const StaticString &source, unsigned int srcChar, unsigned int numChar);
	/// Copies characters from somewhere in the other string to the beginning of this one
	template <unsigned int U>
	unsigned int assign(const StaticString<U> &source, unsigned int srcChar, unsigned int numChar);
	/// Copies all characters from the other string to the beginning of this one
	unsigned int assign(const StaticString &source);
	/// Copies all characters from the other string to the beginning of this one
	template <unsigned int U>
	unsigned int assign(const StaticString<U> &source);
	/// Copies characters from a C string to the beginning of this one
	unsigned int assign(const char *source, unsigned int numChar);

	/// Copies characters from somewhere in this string to a C string
	unsigned int copy(char *dest, unsigned int srcChar, unsigned int numChar) const;
	/// Copies all characters from this string to a C string
	unsigned int copy(char *dest) const;

	/// Appends all the characters from the other string to the end of this one
	unsigned int append(const StaticString &other);
	/// Appends all the characters from the other string to the end of this one
	template <unsigned int U>
	unsigned int append(const StaticString<U> &other);
	/// Appends all the characters from the C string to the end of this one
	unsigned int append(const char *cString);

	/// Compares the string with another one in lexicographical order
	int compare(const StaticString &other) const;
	/// Compares the string with another one in lexicographical order
	template <unsigned int U>
	int compare(const StaticString<U> &other) const;
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
	template <unsigned int U>
	int find(const StaticString<U> &other) const;
	/// Finds the first occurrence of the given constant C string
	int find(const char *cString) const;

	/// Replaces the string with the formatted result
	StaticString &format(const char *fmt, ...);
	/// Append the formatted result to the string
	StaticString &formatAppend(const char *fmt, ...);

	/// Appends another string to this one
	StaticString &operator+=(const StaticString &other);
	/// Appends another string to this one
	template <unsigned int U>
	StaticString &operator+=(const StaticString<U> &other);
	/// Appends a constant C string to the string object
	StaticString &operator+=(const char *cString);
	/// Concatenate two strings together to create a third one
	StaticString operator+(const StaticString &other) const;
	/// Concatenate two strings together to create a third one
	template <unsigned int U>
	StaticString operator+(const StaticString<U> &other) const;
	/// Concatenates a string with a constant C string to create a third one
	StaticString operator+(const char *cString) const;

	/// Friend operator to concatenate a constant C string with a string to create a third one
	template <unsigned int U> friend StaticString<U> operator+(const char *cString, const StaticString<U> &string);

	inline bool operator==(const StaticString &other) const { return (length_ != other.length_) ? false : (compare(other) == 0); }
	inline bool operator!=(const StaticString &other) const { return (length_ != other.length_) ? true : (compare(other) != 0); }
	inline bool operator>(const StaticString &other) const { return compare(other) > 0; }
	inline bool operator<(const StaticString &other) const { return compare(other) < 0; }
	inline bool operator>=(const StaticString &other) const { return compare(other) >= 0; }
	inline bool operator<=(const StaticString &other) const { return compare(other) <= 0; }

	template <unsigned int U> inline bool operator==(const StaticString<U> &other) const { return (length_ != other.length()) ? false : (compare(other) == 0); }
	template <unsigned int U> inline bool operator!=(const StaticString<U> &other) const { return (length_ != other.length()) ? true : (compare(other) != 0); }
	template <unsigned int U> inline bool operator>(const StaticString<U> &other) const { return compare(other) > 0; }
	template <unsigned int U> inline bool operator<(const StaticString<U> &other) const { return compare(other) < 0; }
	template <unsigned int U> inline bool operator>=(const StaticString<U> &other) const { return compare(other) >= 0; }
	template <unsigned int U> inline bool operator<=(const StaticString<U> &other) const { return compare(other) <= 0; }

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
	char array_[C];
	unsigned int length_;
	unsigned int capacity_;
};

template <unsigned int C>
StaticString<C>::StaticString()
    : length_(0), capacity_(C)
{
	array_[0] = '\0';
}

template <unsigned int C>
StaticString<C>::StaticString(const char *cString)
    : length_(0), capacity_(C)
{
	ASSERT(cString);
	length_ = static_cast<unsigned int>(nctl::strnlen(cString, capacity_ - 1));

	nctl::strncpy(array_, capacity_, cString, length_);
	array_[length_] = '\0';
}

template <unsigned int C>
StaticString<C>::StaticString(const StaticString &other)
    : length_(other.length_), capacity_(other.capacity_)
{
	nctl::strncpy(array_, capacity_, other.array_, length_);
	array_[length_] = '\0';
}

template <unsigned int C>
template <unsigned int U>
StaticString<C>::StaticString(const StaticString<U> &other)
    : length_(min(other.length(), C - 1)), capacity_(C)
{
	nctl::strncpy(array_, capacity_, other.data(), length_);
	array_[length_] = '\0';
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/*! \note Not implemented with the copy-and-swap idiom because it has to copy data. */
template <unsigned int C>
StaticString<C> &StaticString<C>::operator=(const StaticString &other)
{
	if (this != &other)
		assign(other);

	return *this;
}

/*! \note Not implemented with the copy-and-swap idiom because it has to copy data. */
template <unsigned int C>
template <unsigned int U>
StaticString<C> &StaticString<C>::operator=(const StaticString<U> &other)
{
	assign(other);
	return *this;
}

template <unsigned int C>
StaticString<C> &StaticString<C>::operator=(const char *cString)
{
	ASSERT(cString);

	const unsigned int cLength = static_cast<unsigned int>(nctl::strnlen(cString, capacity_));
	length_ = min(cLength, capacity_ - 1);
	nctl::strncpy(array_, capacity_, cString, length_);

	array_[length_] = '\0';
	return *this;
}

/*! The method is useful to update the string length after writing into it through the `data()` pointer. */
template <unsigned int C>
unsigned int StaticString<C>::setLength(unsigned int newLength)
{
	length_ = (newLength > capacity_ - 1) ? capacity_ - 1 : newLength;
	return length_;
}

/*! Length will be reset to zero but capacity remains unmodified. */
template <unsigned int C>
void StaticString<C>::clear()
{
	length_ = 0;
	array_[0] = '\0';
}

/*! The method returns the number of characters copied, to allow truncation. */
template <unsigned int C>
unsigned int StaticString<C>::replace(const StaticString &source, unsigned int srcChar, unsigned int numChar, unsigned int destChar)
{
	// Clamping parameters to string lengths and capacities

	// Cannot copy from beyond the end of the source string
	const unsigned int clampedSrcChar = min(srcChar, source.length_);
	const char *srcStart = source.array_ + clampedSrcChar;
	// It is possible to write beyond the end of the destination string, but without creating holes
	const unsigned int clampedDestChar = min(destChar, length_);

	// Cannot copy more characters than the source has left until its length or more than the destination has until its capacity
	const unsigned int charsToCopy = min(min(numChar, source.length_ - clampedSrcChar), capacity_ - clampedDestChar - 1);

	if (charsToCopy > 0)
	{
		char *destStart = array_ + clampedDestChar;

		nctl::strncpy(destStart, capacity_ - clampedDestChar, srcStart, charsToCopy);
		// Destination string length can only grow, truncation has to be performed by the calling function using the return value
		length_ = max(length_, static_cast<unsigned int>(destStart - array_) + charsToCopy);
		array_[length_] = '\0';
	}

	return charsToCopy;
}

/*! The method returns the number of characters copied, to allow truncation. */
template <unsigned int C>
template <unsigned int U>
unsigned int StaticString<C>::replace(const StaticString<U> &source, unsigned int srcChar, unsigned int numChar, unsigned int destChar)
{
	// Clamping parameters to string lengths and capacities

	// Cannot copy from beyond the end of the source string
	const unsigned int clampedSrcChar = min(srcChar, source.length());
	const char *srcStart = source.data() + clampedSrcChar;
	// It is possible to write beyond the end of the destination string, but without creating holes
	const unsigned int clampedDestChar = min(destChar, length_);

	// Cannot copy more characters than the source has left until its length or more than the destination has until its capacity
	const unsigned int charsToCopy = min(min(numChar, source.length() - clampedSrcChar), capacity_ - clampedDestChar - 1);

	if (charsToCopy > 0)
	{
		char *destStart = array_ + clampedDestChar;

		nctl::strncpy(destStart, capacity_ - clampedDestChar, srcStart, charsToCopy);
		// Destination string length can only grow, truncation has to be performed by the calling function using the return value
		length_ = max(length_, static_cast<unsigned int>(destStart - array_) + charsToCopy);
		array_[length_] = '\0';
	}

	return charsToCopy;
}

template <unsigned int C>
unsigned int StaticString<C>::replace(const char *source, unsigned int numChar, unsigned int destChar)
{
	ASSERT(source);

	// It is possible to write beyond the end of the destination string, but without creating holes
	const unsigned int clampedDestChar = min(destChar, length_);

	const unsigned int sourceLength = nctl::strnlen(source, MaxCStringLength);
	// Cannot copy more characters than the source has left until its length or more than the destination has until its capacity
	const unsigned int charsToCopy = min(min(numChar, sourceLength), capacity_ - clampedDestChar - 1);

	if (charsToCopy > 0)
	{
		char *destStart = array_ + clampedDestChar;

		nctl::strncpy(destStart, capacity_ - clampedDestChar, source, charsToCopy);
		// Destination string length can only grow, truncation has to be performed by the calling function using the return value
		length_ = max(length_, static_cast<unsigned int>(destStart - array_) + charsToCopy);
		array_[length_] = '\0';
	}

	return charsToCopy;
}

template <unsigned int C>
unsigned int StaticString<C>::assign(const StaticString &source, unsigned int srcChar, unsigned int numChar)
{
	const unsigned int copiedChars = replace(source, srcChar, numChar, 0);
	length_ = copiedChars;
	array_[length_] = '\0';
	return copiedChars;
}

template <unsigned int C>
template <unsigned int U>
unsigned int StaticString<C>::assign(const StaticString<U> &source, unsigned int srcChar, unsigned int numChar)
{
	const unsigned int copiedChars = replace(source, srcChar, numChar, 0);
	length_ = copiedChars;
	array_[length_] = '\0';
	return copiedChars;
}

template <unsigned int C>
unsigned int StaticString<C>::assign(const StaticString &source)
{
	return assign(source, 0, source.length_);
}

template <unsigned int C>
template <unsigned int U>
unsigned int StaticString<C>::assign(const StaticString<U> &source)
{
	return assign(source, 0, source.length());
}

template <unsigned int C>
unsigned int StaticString<C>::assign(const char *source, unsigned int numChar)
{
	const unsigned int copiedChars = replace(source, numChar, 0);
	length_ = copiedChars;
	array_[length_] = '\0';
	return copiedChars;
}

template <unsigned int C>
unsigned int StaticString<C>::copy(char *dest, unsigned int srcChar, unsigned int numChar) const
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

template <unsigned int C>
unsigned int StaticString<C>::copy(char *dest) const
{
	return copy(dest, 0, length_);
}

template <unsigned int C>
unsigned int StaticString<C>::append(const StaticString &other)
{
	return replace(other, 0, other.length_, length_);
}

template <unsigned int C>
template <unsigned int U>
unsigned int StaticString<C>::append(const StaticString<U> &other)
{
	return replace(other, 0, other.length(), length_);
}

template <unsigned int C>
unsigned int StaticString<C>::append(const char *cString)
{
	return replace(cString, static_cast<unsigned int>(nctl::strnlen(cString, MaxCStringLength)), length_);
}

template <unsigned int C>
int StaticString<C>::compare(const StaticString &other) const
{
	const unsigned int minCapacity = nctl::min(capacity_, other.capacity_);
	return strncmp(data(), other.data(), minCapacity);
}

template <unsigned int C>
template <unsigned int U>
int StaticString<C>::compare(const StaticString<U> &other) const
{
	const unsigned int minCapacity = nctl::min(capacity_, other.capacity());
	return strncmp(data(), other.data(), minCapacity);
}

template <unsigned int C>
int StaticString<C>::compare(const char *cString) const
{
	ASSERT(cString);
	return strncmp(data(), cString, capacity_);
}

template <unsigned int C>
int StaticString<C>::findFirstChar(char c) const
{
	const char *foundPtr = strchr(array_, c);

	if (foundPtr)
		return static_cast<int>(foundPtr - array_);
	else
		return -1;
}

template <unsigned int C>
int StaticString<C>::findLastChar(char c) const
{
	const char *foundPtr = strrchr(array_, c);

	if (foundPtr)
		return static_cast<int>(foundPtr - array_);
	else
		return -1;
}

template <unsigned int C>
int StaticString<C>::findFirstCharAfterIndex(char c, unsigned int index) const
{
	if (length_ == 0 || index >= length_ - 1)
		return -1;

	const char *foundPtr = strchr(array_ + index + 1, c);

	if (foundPtr)
		return static_cast<int>(foundPtr - array_);
	else
		return -1;
}

template <unsigned int C>
int StaticString<C>::find(const StaticString &other) const
{
	const char *foundPtr = strstr(data(), other.data());

	if (foundPtr)
		return static_cast<int>(foundPtr - data());
	else
		return -1;
}

template <unsigned int C>
template <unsigned int U>
int StaticString<C>::find(const StaticString<U> &other) const
{
	const char *foundPtr = strstr(data(), other.data());

	if (foundPtr)
		return static_cast<int>(foundPtr - data());
	else
		return -1;
}

template <unsigned int C>
int StaticString<C>::find(const char *cString) const
{
	ASSERT(cString);
	const char *foundPtr = strstr(data(), cString);

	if (foundPtr)
		return static_cast<int>(foundPtr - data());
	else
		return -1;
}

template <unsigned int C>
StaticString<C> &StaticString<C>::format(const char *fmt, ...)
{
	ASSERT(fmt);

	int formattedLength = 0;
	va_list args;
	va_start(args, fmt);
	formattedLength = nctl::vsnprintfTrunc(data(), capacity_, fmt, args);
	va_end(args);

#if defined(_WIN32) && !defined(__MINGW32__)
	if (formattedLength < 0)
		formattedLength = capacity_ - 1;
#endif

	if (formattedLength > 0)
	{
		length_ = nctl::min(capacity_ - 1, static_cast<unsigned int>(formattedLength));
		array_[length_] = '\0';
	}

	return *this;
}

template <unsigned int C>
StaticString<C> &StaticString<C>::formatAppend(const char *fmt, ...)
{
	ASSERT(fmt);

	int formattedLength = 0;
	va_list args;
	va_start(args, fmt);
	formattedLength = nctl::vsnprintfTrunc(data() + length_, capacity_ - length_, fmt, args);
	va_end(args);

#if defined(_WIN32) && !defined(__MINGW32__)
	if (formattedLength < 0)
		formattedLength = capacity_ - 1;
#endif

	if (formattedLength > 0)
	{
		length_ += min(capacity_ - length_ - 1, static_cast<unsigned int>(formattedLength));
		array_[length_] = '\0';
	}

	return *this;
}

template <unsigned int C>
StaticString<C> &StaticString<C>::operator+=(const StaticString &other)
{
	const unsigned int minLength = min(other.length_, capacity_ - length_ - 1);

	nctl::strncpy(array_ + length_, capacity_ - length_, other.array_, minLength);
	length_ += minLength;

	array_[length_] = '\0';
	return *this;
}

template <unsigned int C>
template <unsigned int U>
StaticString<C> &StaticString<C>::operator+=(const StaticString<U> &other)
{
	const unsigned int minLength = min(other.length(), capacity_ - length_ - 1);

	nctl::strncpy(array_ + length_, capacity_ - length_, other.data(), minLength);
	length_ += minLength;

	array_[length_] = '\0';
	return *this;
}

template <unsigned int C>
StaticString<C> &StaticString<C>::operator+=(const char *cString)
{
	ASSERT(cString);
	const unsigned int otherLength = static_cast<unsigned int>(nctl::strnlen(cString, MaxCStringLength));
	const unsigned int minLength = min(otherLength, capacity_ - length_ - 1);

	nctl::strncpy(array_ + length_, capacity_ - length_, cString, minLength);
	length_ += minLength;

	array_[length_] = '\0';
	return *this;
}

template <unsigned int C>
StaticString<C> StaticString<C>::operator+(const StaticString &other) const
{
	StaticString<C> result;

	result = *this;
	result += other;

	return result;
}

template <unsigned int C>
template <unsigned int U>
StaticString<C> StaticString<C>::operator+(const StaticString<U> &other) const
{
	StaticString<C> result;

	result = *this;
	result += other;

	return result;
}

template <unsigned int C>
StaticString<C> StaticString<C>::operator+(const char *cString) const
{
	ASSERT(cString);
	StaticString<C> result;

	result = *this;
	result += cString;

	return result;
}

template <unsigned int U>
StaticString<U> operator+(const char *cString, const StaticString<U> &string)
{
	ASSERT(cString);
	StaticString<U> result;

	result = cString;
	result += string;

	return result;
}

template <unsigned int C>
const char &StaticString<C>::at(unsigned int index) const
{
	FATAL_ASSERT_MSG_X(index < length_, "Index %u is out of bounds (length: %u)", index, length_);
	return operator[](index);
}

template <unsigned int C>
char &StaticString<C>::at(unsigned int index)
{
	FATAL_ASSERT_MSG_X(index < length_, "Index %u is out of bounds (length: %u)", index, length_);
	return operator[](index);
}

template <unsigned int C>
const char &StaticString<C>::operator[](unsigned int index) const
{
	ASSERT_MSG_X(index < length_, "Index %u is out of bounds (length: %u)", index, length_);
	return array_[index];
}

template <unsigned int C>
char &StaticString<C>::operator[](unsigned int index)
{
	ASSERT_MSG_X(index < length_, "Index %u is out of bounds (size: %u)", index, length_);
	return array_[index];
}

template <unsigned int C>
int StaticString<C>::utf8ToCodePoint(unsigned int position, unsigned int &codePoint, unsigned int *codeUnits) const
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

template <unsigned int C>
int StaticString<C>::utf8ToCodePoint(unsigned int position, unsigned int &codePoint) const
{
	return utf8ToCodePoint(position, codePoint, nullptr);
}

}

#endif
