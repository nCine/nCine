#include <cstring>
#include "common_macros.h"
#include "ncString.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

String::String()
	: array_(NULL), length_(0), capacity_(DefaultCapacity)
{
	array_ = new char[capacity_];
	array_[0] = '\0';
}

String::String(unsigned int capacity)
	: array_(NULL), length_(0), capacity_(capacity)
{
	FATAL_ASSERT_MSG(capacity > 0, "Zero is not a valid capacity");

	array_ = new char[capacity_];
	array_[0] = '\0';
}

String::String(const char *cString)
	: array_(NULL), length_(0), capacity_(0)
{
	ASSERT(cString);
#if defined(_WIN32) && !defined(__MINGW32__)
	capacity_ = static_cast<unsigned int>(strnlen_s(cString, MaxCStringLength)) + 1;
#else
	capacity_ = static_cast<unsigned int>(strnlen(cString, MaxCStringLength)) + 1;
#endif
	length_ = capacity_ - 1;
	array_ = new char[capacity_];

#if defined(_WIN32) && !defined(__MINGW32__)
	strncpy_s(array_, capacity_, cString, length_);
#else
	strncpy(array_, cString, length_);
#endif
	array_[length_] = '\0';
}

String::~String()
{
	delete[] array_;
}

String::String(const String &other)
	: array_(NULL), length_(other.length_), capacity_(other.capacity_)
{
	array_ = new char[capacity_];

#if defined(_WIN32) && !defined(__MINGW32__)
	strncpy_s(array_, capacity_, other.array_, length_);
#else
	strncpy(array_, other.array_, length_);
#endif
	array_[length_] = '\0';
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
		unsigned int copiedChars = copy(other);
		length_ = copiedChars;
		array_[length_] = '\0';
	}

	return *this;
}

String &String::operator=(const char *cString)
{
	ASSERT(cString);
#if defined(_WIN32) && !defined(__MINGW32__)
	length_ = static_cast<unsigned int>(strnlen_s(cString, capacity_));
	strncpy_s(array_, capacity_, cString, length_);
#else
	length_ = static_cast<unsigned int>(strnlen(cString, capacity_));
	strncpy(array_, cString, length_);
#endif

	array_[length_] = '\0';
	return *this;
}

/*! The method returns the number of characters copied, to allow truncation. */
unsigned int String::copy(const String &source, unsigned int srcChar, unsigned int numChar, unsigned int destChar)
{
	// Clamping parameters to string lengths and capacities

	// Cannot copy from beyond the end of the source string
	unsigned int clampedSrcChar = min(srcChar, source.length_);
	char *srcStart = source.array_ + clampedSrcChar;
	// It is possible to write beyond the end of the destination string, but without creating holes
	unsigned int clampedDestChar = min(destChar, length_);
	char *destStart = array_ + clampedDestChar;
	// Cannot copy more characters than the source has left until its length or more than the destination has until its capacity
	unsigned int charsToCopy = min(min(numChar, source.length_ - clampedSrcChar), capacity_ - clampedDestChar);

	if (charsToCopy > 0)
	{
#if defined(_WIN32) && !defined(__MINGW32__)
		strncpy_s(destStart, capacity_ - clampedDestChar, srcStart, charsToCopy);
#else
		strncpy(destStart, srcStart, charsToCopy);
#endif
		// Source string length can only grow, truncation has to be performed by the calling function using the return value
		length_ = max(length_, static_cast<unsigned int>(destStart - array_) + charsToCopy);
		array_[length_] = '\0';
	}

	return charsToCopy;
}

unsigned int String::copy(const String &source)
{
	return copy(source, 0, source.length_, 0);
}

unsigned int String::append(const String &source)
{
	return copy(source, 0, source.length_, length_);
}

unsigned int String::copyTo(char *dest, unsigned int srcChar, unsigned int numChar) const
{
	ASSERT(dest);

	// Cannot copy from beyond the end of the source string
	unsigned int clampedSrcChar = min(srcChar, length_);
	char *srcStart = array_ + clampedSrcChar;
	// Cannot copy more characters than the source has left until its length
	unsigned int charsToCopy = min(numChar, length_ - clampedSrcChar);

	// Always assuming that the destination is big enough
	if (charsToCopy > 0)
	{
#if defined(_WIN32) && !defined(__MINGW32__)
		strncpy_s(dest, charsToCopy, srcStart, charsToCopy);
#else
		strncpy(dest, srcStart, charsToCopy);
#endif
	}

	return charsToCopy;
}

int String::compare(const String &other) const
{
	unsigned int minCapacity = nc::min(capacity_, other.capacity_);
	return strncmp(array_, other.array_, minCapacity);
}

int String::compare(const char *cString) const
{
	ASSERT(cString);
	return strncmp(array_, cString, capacity_);
}

int String::findFirstChar(char c) const
{
	return findFirstCharAfterIndex(c, 0);
}

int String::findLastChar(char c) const
{
	const char *foundPtr = strrchr(array_, c);

	if (foundPtr)
	{
		return static_cast<int>(foundPtr - array_);
	}
	else
	{
		return -1;
	}
}

int String::findFirstCharAfterIndex(char c, unsigned int index) const
{
	if (index >= length_ - 1)
	{
		return -1;
	}

	const char *foundPtr = strchr(array_ + index + 1, c);

	if (foundPtr)
	{
		return static_cast<int>(foundPtr - array_);
	}
	else
	{
		return -1;
	}
}

int String::find(const String &other) const
{
	const char *foundPtr = strstr(array_, other.array_);

	if (foundPtr)
	{
		return static_cast<int>(foundPtr - array_);
	}
	else
	{
		return -1;
	}
}

int String::find(const char *cString) const
{
	ASSERT(cString);
	const char *foundPtr = strstr(array_, cString);

	if (foundPtr)
	{
		return static_cast<int>(foundPtr - array_);
	}
	else
	{
		return -1;
	}
}

String &String::format(const char *fmt, ...)
{
	ASSERT(fmt);

	va_list args;
	va_start(args, fmt);
#if defined(_WIN32) && !defined(__MINGW32__)
	int formattedLength = vsnprintf_s(array_, capacity_, capacity_, fmt, args);
#else
	int formattedLength = vsnprintf(array_, capacity_, fmt, args);
#endif
	va_end(args);

	if (formattedLength > 0)
	{
		length_ = nc::min(capacity_, static_cast<unsigned int>(formattedLength));
		array_[length_] = '\0';
	}

	return *this;
}

String &String::formatAppend(const char *fmt, ...)
{
	ASSERT(fmt);

	va_list args;
	va_start(args, fmt);
#if defined(_WIN32) && !defined(__MINGW32__)
	int formattedLength = vsnprintf_s(array_ + length_, capacity_ - length_, capacity_ - length_, fmt, args);
#else
	int formattedLength = vsnprintf(array_ + length_, capacity_ - length_, fmt, args);
#endif
	va_end(args);

	if (formattedLength > 0)
	{
		length_ += min(capacity_ - length_, static_cast<unsigned int>(formattedLength));
		array_[length_] = '\0';
	}

	return *this;
}

String &String::operator+=(const String &other)
{
	unsigned int availCapacity = capacity_ - length_;
	unsigned int minLength = min(other.length_, availCapacity);

#if defined(_WIN32) && !defined(__MINGW32__)
	strncpy_s(array_ + length_, capacity_ - length_, other.array_, minLength);
#else
	strncpy(array_ + length_, other.array_, minLength);
#endif
	length_ += minLength;

	array_[length_] = '\0';
	return *this;
}

String &String::operator+=(const char *cString)
{
	ASSERT(cString);
	unsigned int availCapacity = capacity_ - length_;

#if defined(_WIN32) && !defined(__MINGW32__)
	unsigned int cLength = static_cast<unsigned int>(strnlen_s(cString, availCapacity));
	strncpy_s(array_ + length_, capacity_ - length_, cString, cLength);
#else
	unsigned int cLength = static_cast<unsigned int>(strnlen(cString, availCapacity));
	strncpy(array_ + length_, cString, cLength);
#endif
	length_ += cLength;

	array_[length_] = '\0';
	return *this;
}

String String::operator+(const String &other) const
{
	unsigned int sumLength = length_ + other.length_ + 1;
	String result(sumLength);

	result = *this;
	result += other;

	return result;
}

String String::operator+(const char *cString) const
{
	ASSERT(cString);
	unsigned int sumLength = length_ + static_cast<unsigned int>(strlen(cString)) + 1;
	String result(sumLength);

	result = *this;
	result += cString;

	return result;
}

}
