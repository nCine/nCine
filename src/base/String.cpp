#include <cstring>
#include <cstdlib> // for exit()
#include "ncString.h"
#include "ServiceLocator.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

/// Default constructor
String::String()
	: array_(NULL), length_(0), capacity_(DefaultCapacity)
{
	array_ = new char[capacity_];
	array_[0] = '\0';
}

/// Constructs an empty string with explicit size
String::String(unsigned int capacity)
	: array_(NULL), length_(0), capacity_(capacity)
{
	if (capacity_ == 0)
	{
		LOGF("Zero is not a valid capacity");
		exit(EXIT_FAILURE);
	}

	array_ = new char[capacity_];
	array_[0] = '\0';
}

/// Constructs a string from a C string
String::String(const char *cString)
	: array_(NULL), length_(0), capacity_(0)
{
	capacity_ = static_cast<unsigned int>(strlen(cString)) + 1;
	length_ = capacity_ - 1;
	array_ = new char[capacity_];

#ifdef _WIN32
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

/// Copy constructor
String::String(const String& other)
	: array_(NULL), length_(other.length_), capacity_(other.capacity_)
{
	array_ = new char[capacity_];

#ifdef _WIN32
	strncpy_s(array_, capacity_, other.array_, length_);
#else
	strncpy(array_, other.array_, length_);
#endif
	array_[length_] = '\0';
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/// Copy-and-swap assignment operator
/** The parameter should be passed by value for the idiom to work */
String& String::operator=(String other)
{
	swap(*this, other);
	return *this;
}

/// Assigns a constant C string to the string object
String& String::operator=(const char *cString)
{
#ifdef _WIN32
	length_ = static_cast<unsigned int>(strnlen_s(cString, capacity_));
	strncpy_s(array_, capacity_, cString, length_);
#else
	length_ = static_cast<unsigned int>(strnlen(cString, capacity_));
	strncpy(array_, cString, length_);
#endif

	array_[length_] = '\0';
	return *this;
}

/// Copies characters from another string
void String::copyFrom(const String &source, unsigned int fromChar, unsigned int charNum)
{
	if (source.length_ >= fromChar + charNum && charNum <= (capacity_ - length_))
	{
#ifdef _WIN32
		strncpy_s(array_, capacity_, source.array_ + fromChar, charNum);
#else
		strncpy(array_, source.array_ + fromChar, charNum);
#endif
		length_ += charNum;
		array_[length_] = '\0';
	}
}

/// Compares the string with another one in lexicographical order
int String::compare(const String &other) const
{
	unsigned int minCapacity = nc::min(capacity_, other.capacity_);
	return strncmp(array_, other.array_, minCapacity);
}

/// Compares the string with a constant C string in lexicographical order
int String::compare(const char *cString) const
{
	return strncmp(array_, cString, capacity_);
}

/// Finds the first occurrence of a character
int String::findFirstChar(char c) const
{
	const char *foundPtr = strchr(array_, c);

	if (foundPtr)
	{
		return static_cast<int>(foundPtr - array_);
	}
	else
	{
		return -1;
	}
}

/// Finds the last occurrence of a character
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

/// Finds the first occurrence of the given string
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

/// Finds the first occurrence of the given constant C string
int String::find(const char *cString) const
{
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

/// Replaces the string with the formatted result
String& String::format(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
#ifdef _WIN32
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

/// Append the formatted result to the string
String& String::formatAppend(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
#ifdef _WIN32
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

/// Appends another string to this one
String& String::operator+=(const String &other)
{
	unsigned int availCapacity = capacity_ - length_;
	unsigned int minLength = min(other.length_, availCapacity);

#ifdef _WIN32
	strncpy_s(array_ + length_, capacity_ - length_, other.array_, minLength);
#else
	strncpy(array_ + length_, other.array_, minLength);
#endif
	length_ += minLength;

	array_[length_] = '\0';
	return *this;
}

/// Appends a constant C string to the string object
String& String::operator+=(const char *cString)
{
	unsigned int availCapacity = capacity_ - length_;

#ifdef _WIN32
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

/// Concatenates two strings together to create a third one
String String::operator+(const String &other)
{
	unsigned int sumLength = length_ + other.length_ + 1;
	String result(sumLength);

	result = *this;
	result += other;

	return result;
}

/// Concatenates a string with a constant C string to create a third one
String String::operator+(const char *cString)
{
	unsigned int sumLength = length_ + static_cast<unsigned int>(strlen(cString)) + 1;
	String result(sumLength);

	result = *this;
	result += cString;

	return result;
}

}
