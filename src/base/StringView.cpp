#include "common_macros.h"
#include <nctl/CString.h>
#include <nctl/StringView.h>
#include <nctl/algorithms.h>
#include <nctl/Utf8.h>

namespace nctl {

static char EmptyString[1] = { '\0' };

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

StringView::StringView()
    : array_(EmptyString), length_(0), capacity_(1)
{
}

/*! \note The capacity will be set to the string array length.
 *  \warning The string array must be null terminated.
 */
StringView::StringView(char *array)
    : StringView()
{
	set(array);
}

StringView::StringView(char *array, unsigned int capacity)
    : StringView()
{
	set(array, capacity);
}

StringView::StringView(char *array, unsigned int capacity, unsigned int length)
    : StringView()
{
	set(array, capacity, length);
}

StringView::StringView(const StringView &other)
    : array_(other.array_), length_(other.length_), capacity_(other.capacity_)
{
}

StringView::StringView(StringView &&other)
    : array_(EmptyString), length_(0), capacity_(1)
{
	swap(*this, other);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

StringView &StringView::operator=(const StringView &other)
{
	if (this != &other)
	{
		array_ = other.array_;
		length_ = other.length_;
		capacity_ = other.capacity_;
	}
	return *this;
}

StringView &StringView::operator=(StringView &&other)
{
	if (this != &other)
	{
		swap(*this, other);
		other.array_ = EmptyString;
		other.length_ = 0;
		other.capacity_ = 1;
	}
	return *this;
}

StringView &StringView::operator=(const char *cString)
{
	if (cString != array_ && cString != nullptr && capacity_ > 1)
	{
		length_ = static_cast<unsigned int>(nctl::strnlen(cString, capacity_ - 1));
		nctl::strncpy(array_, capacity_, cString, length_);

		array_[length_] = '\0';
	}
	return *this;
}

/*! The method is useful to update the string length after writing into it through the `data()` pointer. */
unsigned int StringView::setLength(unsigned int newLength)
{
	if (capacity_ > 1)
		length_ = (newLength > capacity_ - 1) ? capacity_ - 1 : newLength;
	return length_;
}

void StringView::set(char *array)
{
	array_ = EmptyString;
	capacity_ = 1;
	length_ = 0;

	if (array != nullptr)
	{
		length_ = nctl::strnlen(array, MaxCStringLength);
		capacity_ = length_ + 1;
		array_ = array;
	}
}

void StringView::set(char *array, unsigned int capacity)
{
	array_ = EmptyString;
	capacity_ = 1;
	length_ = 0;

	if (array != nullptr && capacity > 1)
	{
		capacity_ = capacity;
		length_ = nctl::strnlen(array, capacity);
		array_ = array;
	}
}

void StringView::set(char *array, unsigned int capacity, unsigned int length)
{
	array_ = EmptyString;
	capacity_ = 1;
	length_ = 0;

	if (array != nullptr && capacity > 1)
	{
		capacity_ = capacity;
		length_ = (length < capacity_) ? length : capacity_ - 1;
		array_ = array;
		array_[length_] = '\0';
	}
}

/*! \note Length will be reset to zero but capacity remains unmodified. */
void StringView::clear()
{
	length_ = 0;
	array_[0] = '\0';
}

void StringView::removePrefix(unsigned int num)
{
	if (num <= length_)
	{
		length_ -= num;
		capacity_ -= num;
		array_ += num;
	}
}

void StringView::removeSuffix(unsigned int num)
{
	if (num <= length_)
	{
		length_ -= num;
		capacity_ -= num;
		array_[length_] = '\0';
	}
}

/*! The method returns the number of characters copied, to allow truncation. */
unsigned int StringView::replace(const StringView &source, unsigned int srcChar, unsigned int numChar, unsigned int destChar)
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

unsigned int StringView::replace(const char *source, unsigned int numChar, unsigned int destChar)
{
	ASSERT(source);

	// It is possible to write beyond the end of the destination string, but without creating holes
	const unsigned int clampedDestChar = min(destChar, length_);

	const unsigned int sourceLength = nctl::strnlen(source, capacity_);
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

unsigned int StringView::assign(const StringView &source, unsigned int srcChar, unsigned int numChar)
{
	const unsigned int copiedChars = replace(source, srcChar, numChar, 0);
	length_ = copiedChars;
	array_[length_] = '\0';
	return copiedChars;
}

unsigned int StringView::assign(const StringView &source)
{
	return assign(source, 0, source.length_);
}

unsigned int StringView::assign(const char *source, unsigned int numChar)
{
	const unsigned int copiedChars = replace(source, numChar, 0);
	length_ = copiedChars;
	array_[length_] = '\0';
	return copiedChars;
}

unsigned int StringView::copy(char *dest, unsigned int srcChar, unsigned int numChar) const
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

unsigned int StringView::copy(char *dest) const
{
	return copy(dest, 0, length_);
}

unsigned int StringView::append(const StringView &other)
{
	return replace(other, 0, other.length_, length_);
}

unsigned int StringView::append(const char *cString)
{
	return replace(cString, static_cast<unsigned int>(nctl::strnlen(cString, capacity_)), length_);
}

int StringView::compare(const StringView &other) const
{
	const unsigned int minCapacity = nctl::min(capacity_, other.capacity_);
	return strncmp(array_, other.array_, minCapacity);
}

int StringView::compare(const char *cString) const
{
	ASSERT(cString);
	return strncmp(array_, cString, capacity_);
}

int StringView::findFirstChar(char c) const
{
	const char *foundPtr = strchr(array_, c);

	if (foundPtr)
		return static_cast<int>(foundPtr - array_);
	else
		return -1;
}

int StringView::findLastChar(char c) const
{
	const char *foundPtr = strrchr(array_, c);

	if (foundPtr)
		return static_cast<int>(foundPtr - array_);
	else
		return -1;
}

int StringView::findFirstCharAfterIndex(char c, unsigned int index) const
{
	if (length_ == 0 || index >= length_ - 1)
		return -1;

	const char *foundPtr = strchr(array_ + index + 1, c);

	if (foundPtr)
		return static_cast<int>(foundPtr - array_);
	else
		return -1;
}

int StringView::find(const StringView &other) const
{
	const char *foundPtr = strstr(array_, other.array_);

	if (foundPtr)
		return static_cast<int>(foundPtr - array_);
	else
		return -1;
}

int StringView::find(const char *cString) const
{
	ASSERT(cString);
	const char *foundPtr = strstr(array_, cString);

	if (foundPtr)
		return static_cast<int>(foundPtr - array_);
	else
		return -1;
}

StringView &StringView::format(const char *fmt, ...)
{
	ASSERT(fmt);

	int formattedLength = 0;
	do
	{
		va_list args;
		va_start(args, fmt);
		formattedLength = nctl::vsnprintfTrunc(array_, capacity_, fmt, args);
		va_end(args);
	} while (static_cast<unsigned int>(formattedLength) < capacity_ - 1);

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

StringView &StringView::formatAppend(const char *fmt, ...)
{
	ASSERT(fmt);

	int formattedLength = 0;
	va_list args;
	va_start(args, fmt);
	formattedLength = nctl::vsnprintfTrunc(array_ + length_, capacity_ - length_, fmt, args);
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

StringView &StringView::operator+=(const StringView &other)
{
	ASSERT(array_ != other.array_);

	if (array_ != other.array_ && capacity_ > 0)
	{
		const unsigned int minLength = min(other.length_, capacity_ - length_ - 1);

		nctl::strncpy(array_ + length_, capacity_ - length_, other.array_, minLength);
		length_ += minLength;

		array_[length_] = '\0';
	}
	return *this;
}

StringView &StringView::operator+=(const char *cString)
{
	ASSERT(cString);
	ASSERT(array_ != cString);

	if (array_ != cString && cString != nullptr && capacity_ > 0)
	{
		const unsigned int otherLength = static_cast<unsigned int>(nctl::strnlen(cString, capacity_));
		const unsigned int minLength = min(otherLength, capacity_ - length_ - 1);

		nctl::strncpy(array_ + length_, capacity_ - length_, cString, minLength);
		length_ += minLength;

		array_[length_] = '\0';
	}
	return *this;
}

const char &StringView::at(unsigned int index) const
{
	FATAL_ASSERT_MSG_X(index < length_, "Index %u is out of bounds (length: %u)", index, length_);
	return operator[](index);
}

char &StringView::at(unsigned int index)
{
	FATAL_ASSERT_MSG_X(index < length_, "Index %u is out of bounds (length: %u)", index, length_);
	return operator[](index);
}

const char &StringView::operator[](unsigned int index) const
{
	ASSERT_MSG_X(index < length_, "Index %u is out of bounds (length: %u)", index, length_);
	return array_[index];
}

char &StringView::operator[](unsigned int index)
{
	ASSERT_MSG_X(index < length_, "Index %u is out of bounds (size: %u)", index, length_);
	return array_[index];
}

int StringView::utf8ToCodePoint(unsigned int position, unsigned int &codePoint, unsigned int *codeUnits) const
{
	if (position + 1 > length_)
	{
		codePoint = Utf8::InvalidUnicode;
		if (codeUnits)
			*codeUnits = Utf8::InvalidUtf8;
		return 0;
	}

	const char *subString = Utf8::utf8ToCodePoint(&operator[](position), codePoint, codeUnits);
	return (subString - array_ - position);
}

int StringView::utf8ToCodePoint(unsigned int position, unsigned int &codePoint) const
{
	return utf8ToCodePoint(position, codePoint, nullptr);
}

}
