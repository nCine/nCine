#include "common_macros.h"
#include <nctl/CString.h>
#include <nctl/String.h>
#include <nctl/algorithms.h>

#ifdef WITH_ALLOCATORS
	#include <nctl/AllocManager.h>
	#include <nctl/IAllocator.h>
#endif

namespace nctl {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

String::String()
    : length_(0), capacity_(SmallBufferSize), fixedCapacity_(false)
{
	array_.local_[0] = '\0';
}

String::String(unsigned int capacity, StringMode mode)
    : length_(0), capacity_(capacity), fixedCapacity_(mode == StringMode::FIXED_CAPACITY)
{
	array_.local_[0] = '\0';

	if (capacity_ <= SmallBufferSize)
		capacity_ = SmallBufferSize;
	else
	{
#if !defined(WITH_ALLOCATORS)
		array_.begin_ = new char[capacity_];
#else
		array_.begin_ = theStringAllocator().newArray<char>(capacity_);
#endif
		array_.begin_[0] = '\0';
	}
}

String::String(const char *cString, StringMode mode)
    : length_(0), capacity_(0), fixedCapacity_(mode == StringMode::FIXED_CAPACITY)
{
	ASSERT(cString);

	length_ = static_cast<unsigned int>(nctl::strnlen(cString, MaxCStringLength));
	capacity_ = length_ + 1;

	if (capacity_ <= SmallBufferSize)
		capacity_ = SmallBufferSize;
	else
	{
#if !defined(WITH_ALLOCATORS)
		array_.begin_ = new char[capacity_];
#else
		array_.begin_ = theStringAllocator().newArray<char>(capacity_);
#endif
	}

	nctl::strncpy(data(), capacity_, cString, length_);
	data()[length_] = '\0';
}

String::~String()
{
	if (capacity_ > SmallBufferSize)
	{
#if !defined(WITH_ALLOCATORS)
		delete[] array_.begin_;
#else
		theStringAllocator().deleteArray<char>(array_.begin_);
#endif
	}
}

String::String(const String &other)
    : length_(other.length_), capacity_(other.capacity_), fixedCapacity_(other.fixedCapacity_)
{
	if (capacity_ > SmallBufferSize)
	{
#if !defined(WITH_ALLOCATORS)
		array_.begin_ = new char[capacity_];
#else
		array_.begin_ = theStringAllocator().newArray<char>(capacity_);
#endif
	}

	nctl::strncpy(data(), capacity_, other.data(), length_);
	data()[length_] = '\0';
}

String::String(String &&other)
    : length_(0), capacity_(0), fixedCapacity_(false)
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
		assign(other);

	return *this;
}

String &String::operator=(String &&other)
{
	if (this != &other)
		swap(*this, other);
	return *this;
}

String &String::operator=(const char *cString)
{
	ASSERT(cString);

	const unsigned int cLength = static_cast<unsigned int>(nctl::strnlen(cString, MaxCStringLength));
	extendCapacity(cLength + 1);
	length_ = min(cLength, capacity_ - 1);
	nctl::strncpy(data(), capacity_, cString, length_);

	data()[length_] = '\0';
	return *this;
}

/*! The method is useful to update the string length after writing into it through the `data()` pointer. */
unsigned int String::setLength(unsigned int newLength)
{
	length_ = (newLength > capacity_ - 1) ? capacity_ - 1 : newLength;
	return length_;
}

void String::setCapacity(unsigned int newCapacity)
{
	// Setting a new capacity is disabled if the array is fixed
	if (fixedCapacity_)
	{
		LOGW_X("Trying to change the capacity of a fixed string, from from %u to %u", capacity_, newCapacity);
		return;
	}

	if (newCapacity == capacity_)
	{
		LOGW_X("String capacity already equal to %u", capacity_);
		return;
	}
	else if (newCapacity < capacity_)
		LOGI_X("String capacity shrinking from %u to %u", capacity_, newCapacity);
	else if (newCapacity > capacity_)
		LOGD_X("String capacity growing from %u to %u", capacity_, newCapacity);

	char *newArray = nullptr;
	if (newCapacity > SmallBufferSize)
	{
#if !defined(WITH_ALLOCATORS)
		newArray = static_cast<char *>(::operator new[](newCapacity * sizeof(char)));
#else
		newArray = theStringAllocator().newArray<char>(capacity_);
#endif
		if (length_ > 0)
		{
			if (newCapacity < length_) // shrinking
				length_ = newCapacity; // cropping last elements

			if (capacity_ > SmallBufferSize)
				nctl::strncpy(newArray, capacity_, array_.begin_, length_);
			else
				nctl::strncpy(newArray, SmallBufferSize, array_.local_, length_);

			newArray[length_] = '\0';
		}
	}
	else
	{
		// Capacity can't be smaller than the local buffer size
		newCapacity = SmallBufferSize;
		if (newCapacity < length_) // shrinking
			length_ = newCapacity; // cropping last elements
	}

	if (capacity_ > SmallBufferSize)
	{
		char tempLocal[SmallBufferSize];
		if (newCapacity <= SmallBufferSize)
		{
			nctl::strncpy(tempLocal, newCapacity, array_.begin_, length_);
			tempLocal[length_] = '\0';
		}

#if !defined(WITH_ALLOCATORS)
		delete[] array_.begin_;
#else
		theStringAllocator().deleteArray<char>(array_.begin_);
#endif

		if (newCapacity <= SmallBufferSize)
		{
			nctl::strncpy(array_.local_, newCapacity, tempLocal, length_);
			array_.local_[length_] = '\0';
		}
	}

	if (newCapacity > SmallBufferSize)
		array_.begin_ = newArray;
	capacity_ = newCapacity;
}

void String::shrinkToFit()
{
	if (length_ > 0)
		setCapacity(length_ + 1);
}

/*! Length will be reset to zero but capacity remains unmodified. */
void String::clear()
{
	length_ = 0;
	data()[0] = '\0';
}

/*! The method returns the number of characters copied, to allow truncation. */
unsigned int String::replace(const String &source, unsigned int srcChar, unsigned int numChar, unsigned int destChar)
{
	// Clamping parameters to string lengths and capacities

	// Cannot copy from beyond the end of the source string
	const unsigned int clampedSrcChar = min(srcChar, source.length_);
	const char *srcStart = source.data() + clampedSrcChar;
	// It is possible to write beyond the end of the destination string, but without creating holes
	const unsigned int clampedDestChar = min(destChar, length_);

	// Cannot copy more characters than the source has left until its length or more than the destination has until its capacity (if fixed)
	const unsigned int charsToCopy = fixedCapacity_ ? min(min(numChar, source.length_ - clampedSrcChar), capacity_ - clampedDestChar - 1)
	                                                : min(numChar, source.length_ - clampedSrcChar);

	if (charsToCopy > 0)
	{
		extendCapacity(clampedDestChar + charsToCopy + 1);
		char *destStart = data() + clampedDestChar;

		nctl::strncpy(destStart, capacity_ - clampedDestChar, srcStart, charsToCopy);
		// Destination string length can only grow, truncation has to be performed by the calling function using the return value
		length_ = max(length_, static_cast<unsigned int>(destStart - data()) + charsToCopy);
		data()[length_] = '\0';
	}

	return charsToCopy;
}

unsigned int String::replace(const char *source, unsigned int numChar, unsigned int destChar)
{
	ASSERT(source);

	// It is possible to write beyond the end of the destination string, but without creating holes
	const unsigned int clampedDestChar = min(destChar, length_);

	const unsigned int sourceLength = nctl::strnlen(source, MaxCStringLength);
	// Cannot copy more characters than the source has left until its length or more than the destination has until its capacity (if fixed)
	const unsigned int charsToCopy = fixedCapacity_ ? min(min(numChar, sourceLength), capacity_ - clampedDestChar - 1)
	                                                : min(numChar, sourceLength);

	if (charsToCopy > 0)
	{
		extendCapacity(clampedDestChar + charsToCopy + 1);
		char *destStart = data() + clampedDestChar;

		nctl::strncpy(destStart, capacity_ - clampedDestChar, source, charsToCopy);
		// Destination string length can only grow, truncation has to be performed by the calling function using the return value
		length_ = max(length_, static_cast<unsigned int>(destStart - data()) + charsToCopy);
		data()[length_] = '\0';
	}

	return charsToCopy;
}

unsigned int String::assign(const String &source, unsigned int srcChar, unsigned int numChar)
{
	const unsigned int copiedChars = replace(source, srcChar, numChar, 0);
	length_ = copiedChars;
	data()[length_] = '\0';
	return copiedChars;
}

unsigned int String::assign(const String &source)
{
	return assign(source, 0, source.length_);
}

unsigned int String::assign(const char *source, unsigned int numChar)
{
	const unsigned int copiedChars = replace(source, numChar, 0);
	length_ = copiedChars;
	data()[length_] = '\0';
	return copiedChars;
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
		nctl::strncpy(dest, srcStart, charsToCopy);
		dest[charsToCopy] = '\0';
	}

	return charsToCopy;
}

unsigned int String::copy(char *dest) const
{
	return copy(dest, 0, length_);
}

unsigned int String::append(const String &other)
{
	return replace(other, 0, other.length_, length_);
}

unsigned int String::append(const char *cString)
{
	return replace(cString, static_cast<unsigned int>(nctl::strnlen(cString, MaxCStringLength)), length_);
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
	if (length_ == 0 || index >= length_ - 1)
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

	int formattedLength = 0;
	bool extended = false;
	do
	{
		va_list args;
		va_start(args, fmt);
		formattedLength = nctl::vsnprintfTrunc(data(), capacity_, fmt, args);
		va_end(args);

		extended = false;
#if defined(_WIN32) && !defined(__MINGW32__)
		if (formattedLength < 0)
			extended = extendCapacity(capacity_ * 2);
		else
#endif
		if (formattedLength > capacity_ - 1)
			extended = extendCapacity(formattedLength + 1);
	} while (extended);

#if defined(_WIN32) && !defined(__MINGW32__)
	if (formattedLength < 0)
		formattedLength = capacity_ - 1;
#endif

	if (formattedLength > 0)
	{
		length_ = nctl::min(capacity_ - 1, static_cast<unsigned int>(formattedLength));
		data()[length_] = '\0';
	}

	return *this;
}

String &String::formatAppend(const char *fmt, ...)
{
	ASSERT(fmt);

	int formattedLength = 0;
	bool extended = false;
	do
	{
		va_list args;
		va_start(args, fmt);
		formattedLength = nctl::vsnprintfTrunc(data() + length_, capacity_ - length_, fmt, args);
		va_end(args);

		extended = false;
#if defined(_WIN32) && !defined(__MINGW32__)
		if (formattedLength < 0)
			extended = extendCapacity(capacity_ * 2);
		else
#endif
		if (formattedLength > capacity_ - length_ - 1)
			extended = extendCapacity(length_ + formattedLength + 1);
	} while (extended);

#if defined(_WIN32) && !defined(__MINGW32__)
	if (formattedLength < 0)
		formattedLength = capacity_ - 1;
#endif

	if (formattedLength > 0)
	{
		length_ += min(capacity_ - length_ - 1, static_cast<unsigned int>(formattedLength));
		data()[length_] = '\0';
	}

	return *this;
}

String &String::operator+=(const String &other)
{
	extendCapacity(length_ + other.length_ + 1);
	const unsigned int minLength = min(other.length_, capacity_ - length_ - 1);

	nctl::strncpy(data() + length_, capacity_ - length_, other.data(), minLength);
	length_ += minLength;

	data()[length_] = '\0';
	return *this;
}

String &String::operator+=(const char *cString)
{
	ASSERT(cString);
	const unsigned int otherLength = static_cast<unsigned int>(nctl::strnlen(cString, MaxCStringLength));
	extendCapacity(length_ + otherLength + 1);
	const unsigned int minLength = min(otherLength, capacity_ - length_ - 1);

	nctl::strncpy(data() + length_, capacity_ - length_, cString, minLength);
	length_ += minLength;

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

int String::utf8ToCodePoint(unsigned int position, unsigned int &codePoint, unsigned int *codeUnits) const
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

int String::utf8ToCodePoint(unsigned int position, unsigned int &codePoint) const
{
	return utf8ToCodePoint(position, codePoint, nullptr);
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

bool String::extendCapacity(unsigned int minimum)
{
	if (fixedCapacity_ == false && capacity_ < minimum)
	{
		// Double the capacity until it can contain the required minimum
		unsigned int newCapacity_ = capacity_ * 2;
		while (newCapacity_ < minimum)
			newCapacity_ *= 2;
		setCapacity(newCapacity_);
		return true;
	}

	return false;
}

}
