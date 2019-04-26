#include <cstdio> // for vsnprintf()
#include <cstring>
#include "common_macros.h"
#include <nctl/String.h>
#include <nctl/algorithms.h>

namespace nctl {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

String::String()
    : length_(0), capacity_(SmallBufferSize)
{
	array_.local_[0] = '\0';
}

String::String(unsigned int capacity)
    : length_(0), capacity_(capacity)
{
	FATAL_ASSERT_MSG(capacity > 0, "Zero is not a valid capacity");

	array_.local_[0] = '\0';
	if (capacity > SmallBufferSize)
	{
		array_.begin_ = new char[capacity_];
		array_.begin_[0] = '\0';
	}
}

String::String(const char *cString)
    : length_(0), capacity_(0)
{
	ASSERT(cString);
#if defined(_WIN32) && !defined(__MINGW32__)
	capacity_ = static_cast<unsigned int>(strnlen_s(cString, MaxCStringLength)) + 1;
#else
	capacity_ = static_cast<unsigned int>(strnlen(cString, MaxCStringLength)) + 1;
#endif
	length_ = capacity_ - 1;

	char *dest = array_.local_;
	if (capacity_ > SmallBufferSize)
	{
		array_.begin_ = new char[capacity_];
		dest = array_.begin_;
	}

#if defined(_WIN32) && !defined(__MINGW32__)
	strncpy_s(dest, capacity_, cString, length_);
#else
	strncpy(dest, cString, length_);
#endif
	dest[length_] = '\0';
}

String::~String()
{
	if (capacity_ > SmallBufferSize)
		delete[] array_.begin_;
}

String::String(const String &other)
    : length_(other.length_), capacity_(other.capacity_)
{
	const char *src = other.array_.local_;
	char *dest = array_.local_;
	if (capacity_ > SmallBufferSize)
	{
		array_.begin_ = new char[capacity_];
		src = other.array_.begin_;
		dest = array_.begin_;
	}

#if defined(_WIN32) && !defined(__MINGW32__)
	strncpy_s(dest, capacity_, src, length_);
#else
	strncpy(dest, src, length_);
#endif
	dest[length_] = '\0';
}

String::String(String &&other)
    : length_(0), capacity_(0)
{
	swap(*this, other);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/*! \note Not implemented with the copy-and-swap idiom because it has to copy data. */
String &String::operator=(const String &other)
{
	if (this != &other)
	{
		// copy and truncate
		const unsigned int copiedChars = other.copy(*this);
		length_ = copiedChars;
		data()[length_] = '\0';
	}

	return *this;
}

String &String::operator=(String &&other)
{
	swap(*this, other);
	return *this;
}

String &String::operator=(const char *cString)
{
	ASSERT(cString);

#if defined(_WIN32) && !defined(__MINGW32__)
	length_ = static_cast<unsigned int>(strnlen_s(cString, capacity_));
	strncpy_s(data(), capacity_, cString, length_);
#else
	length_ = static_cast<unsigned int>(strnlen(cString, capacity_));
	strncpy(data(), cString, length_);
#endif

	data()[length_] = '\0';
	return *this;
}

/*! The method is useful to update the string length after writing into it through the `data()` pointer. */
unsigned int String::setLength(unsigned int length)
{
	length_ = (length > capacity_ - 1) ? capacity_ - 1 : length;
	return length_;
}

/*! Length will be zero but capacity remains unmodified. */
void String::clear()
{
	length_ = 0;
	data()[0] = '\0';
}

/*! The method returns the number of characters copied, to allow truncation. */
unsigned int String::copy(String &dest, unsigned int srcChar, unsigned int numChar, unsigned int destChar) const
{
	// Clamping parameters to string lengths and capacities

	// Cannot copy from beyond the end of the source string
	const unsigned int clampedSrcChar = min(srcChar, length_);
	const char *srcStart = data() + clampedSrcChar;
	// It is possible to write beyond the end of the destination string, but without creating holes
	const unsigned int clampedDestChar = min(destChar, dest.length_);
	char *destStart = dest.data() + clampedDestChar;
	// Cannot copy more characters than the source has left until its length or more than the destination has until its capacity
	const unsigned int charsToCopy = min(min(numChar, length_ - clampedSrcChar), dest.capacity_ - clampedDestChar - 1);

	if (charsToCopy > 0)
	{
#if defined(_WIN32) && !defined(__MINGW32__)
		strncpy_s(destStart, dest.capacity_ - clampedDestChar, srcStart, charsToCopy);
#else
		strncpy(destStart, srcStart, charsToCopy);
#endif
		// Source string length can only grow, truncation has to be performed by the calling function using the return value
		dest.length_ = max(dest.length_, static_cast<unsigned int>(destStart - dest.data()) + charsToCopy);
		dest.data()[dest.length_] = '\0';
	}

	return charsToCopy;
}

unsigned int String::copy(String &dest, unsigned int srcChar, unsigned int numChar) const
{
	return copy(dest, srcChar, numChar, 0);
}

unsigned int String::copy(String &dest) const
{
	return copy(dest, 0, length_, 0);
}

unsigned int String::copy(char *dest, unsigned int srcChar, unsigned int numChar) const
{
	ASSERT(dest);

	// Cannot copy from beyond the end of the source string
	const unsigned int clampedSrcChar = min(srcChar, length_);
	const char *srcStart = data() + clampedSrcChar;
	// Cannot copy more characters than the source has left until its length
	const unsigned int charsToCopy = min(numChar, length_ - clampedSrcChar);

	// Always assuming that the destination is big enough
	if (charsToCopy > 0)
	{
#if defined(_WIN32) && !defined(__MINGW32__)
		strncpy_s(dest, charsToCopy + 1, srcStart, charsToCopy);
#else
		strncpy(dest, srcStart, charsToCopy);
#endif
	}

	return charsToCopy;
}

unsigned int String::append(const String &source)
{
	return source.copy(*this, 0, source.length_, length_);
}

int String::compare(const String &other) const
{
	const unsigned int minCapacity = nctl::min(capacity_, other.capacity_);
	return strncmp(data(), other.data(), minCapacity);
}

int String::compare(const char *cString) const
{
	ASSERT(cString);
	return strncmp(data(), cString, capacity_);
}

int String::findFirstChar(char c) const
{
	const char *foundPtr = strchr(data(), c);

	if (foundPtr)
		return static_cast<int>(foundPtr - data());
	else
		return -1;
}

int String::findLastChar(char c) const
{
	const char *foundPtr = strrchr(data(), c);

	if (foundPtr)
		return static_cast<int>(foundPtr - data());
	else
		return -1;
}

int String::findFirstCharAfterIndex(char c, unsigned int index) const
{
	if (index >= length_ - 1)
		return -1;

	const char *foundPtr = strchr(data() + index + 1, c);

	if (foundPtr)
		return static_cast<int>(foundPtr - data());
	else
		return -1;
}

int String::find(const String &other) const
{
	const char *foundPtr = strstr(data(), other.data());

	if (foundPtr)
		return static_cast<int>(foundPtr - data());
	else
		return -1;
}

int String::find(const char *cString) const
{
	ASSERT(cString);
	const char *foundPtr = strstr(data(), cString);

	if (foundPtr)
		return static_cast<int>(foundPtr - data());
	else
		return -1;
}

String &String::format(const char *fmt, ...)
{
	ASSERT(fmt);

	va_list args;
	va_start(args, fmt);
#if defined(_WIN32) && !defined(__MINGW32__)
	const int formattedLength = vsnprintf_s(data(), capacity_, capacity_ - 1, fmt, args);
#else
	const int formattedLength = vsnprintf(data(), capacity_, fmt, args);
#endif
	va_end(args);

	if (formattedLength > 0)
	{
		length_ = nctl::min(capacity_, static_cast<unsigned int>(formattedLength));
		data()[length_] = '\0';
	}

	return *this;
}

String &String::formatAppend(const char *fmt, ...)
{
	ASSERT(fmt);

	va_list args;
	va_start(args, fmt);
#if defined(_WIN32) && !defined(__MINGW32__)
	const int formattedLength = vsnprintf_s(data() + length_, capacity_ - length_, capacity_ - length_ - 1, fmt, args);
#else
	const int formattedLength = vsnprintf(data() + length_, capacity_ - length_, fmt, args);
#endif
	va_end(args);

	if (formattedLength > 0)
	{
		length_ += min(capacity_ - length_, static_cast<unsigned int>(formattedLength));
		data()[length_] = '\0';
	}

	return *this;
}

String &String::operator+=(const String &other)
{
	const unsigned int availCapacity = capacity_ - length_ - 1;
	const unsigned int minLength = min(other.length_, availCapacity);

#if defined(_WIN32) && !defined(__MINGW32__)
	strncpy_s(data() + length_, capacity_ - length_, other.data(), minLength);
#else
	strncpy(data() + length_, other.data(), minLength);
#endif
	length_ += minLength;

	data()[length_] = '\0';
	return *this;
}

String &String::operator+=(const char *cString)
{
	ASSERT(cString);
	const unsigned int availCapacity = capacity_ - length_ - 1;

#if defined(_WIN32) && !defined(__MINGW32__)
	const unsigned int cLength = static_cast<unsigned int>(strnlen_s(cString, availCapacity));
	strncpy_s(data() + length_, capacity_ - length_, cString, cLength);
#else
	const unsigned int cLength = static_cast<unsigned int>(strnlen(cString, availCapacity));
	strncpy(data() + length_, cString, cLength);
#endif
	length_ += cLength;

	data()[length_] = '\0';
	return *this;
}

String String::operator+(const String &other) const
{
	const unsigned int sumLength = length_ + other.length_ + 1;
	String result(sumLength);

	result = *this;
	result += other;

	return result;
}

String String::operator+(const char *cString) const
{
	ASSERT(cString);
	const unsigned int sumLength = length_ + static_cast<unsigned int>(strlen(cString)) + 1;
	String result(sumLength);

	result = *this;
	result += cString;

	return result;
}

String operator+(const char *cString, const String &string)
{
	ASSERT(cString);
	const unsigned int sumLength = string.length_ + static_cast<unsigned int>(strlen(cString)) + 1;
	String result(sumLength);

	result = cString;
	result += string;

	return result;
}

const char &String::at(unsigned int index) const
{
	FATAL_ASSERT_MSG_X(index < length_, "Index %u is out of bounds (length: %u)", index, length_);
	return operator[](index);
}

char &String::at(unsigned int index)
{
	FATAL_ASSERT_MSG_X(index < length_, "Index %u is out of bounds (length: %u)", index, length_);
	return operator[](index);
}

const char &String::operator[](unsigned int index) const
{
	ASSERT_MSG_X(index < length_, "Index %u is out of bounds (length: %u)", index, length_);
	return data()[index];
}

char &String::operator[](unsigned int index)
{
	ASSERT_MSG_X(index < length_, "Index %u is out of bounds (size: %u)", index, length_);
	return data()[index];
}

}
