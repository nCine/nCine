#ifndef CLASS_NCTL_STRING
#define CLASS_NCTL_STRING

#include <ncine/common_macros.h>
#include "StringIterator.h"
#include "ReverseIterator.h"
#include "utility.h"

namespace nctl {

/// A basic string class made of chars
class DLL_PUBLIC String
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
	String();
	/// Constructs an empty string with explicit size
	explicit String(unsigned int capacity);
	/// Constructs a string object from a C string
	String(const char *cString);
	~String();

	/// Copy constructor
	String(const String &other);
	/// Move constructor
	String(String &&other);
	/// Assignment operator that preserves the original string capacity
	String &operator=(const String &other);
	/// Move assignment operator
	String &operator=(String &&other);
	/// Assigns a constant C string to the string object
	String &operator=(const char *cString);

	/// Swaps two strings without copying their data
	inline void swap(String &first, String &second)
	{
		nctl::swap(first.array_, second.array_);
		nctl::swap(first.length_, second.length_);
		nctl::swap(first.capacity_, second.capacity_);
	}

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
	/// Sets the string length
	unsigned int setLength(unsigned int length);
	/// Returns the string capacity
	inline unsigned int capacity() const { return capacity_; }

	/// Clears the string
	void clear();

	/// Returns a pointer to the internal array
	inline char *data() { return (capacity_ > SmallBufferSize) ? array_.begin_ : array_.local_; }
	/// Returns a constant pointer to the internal array
	inline const char *data() const { return (capacity_ > SmallBufferSize) ? array_.begin_ : array_.local_; }

	/// Copies characters from somewhere in the other string to somewhere in this one
	unsigned int assign(const String &source, unsigned int srcChar, unsigned int numChar, unsigned int destChar);
	/// Copies characters from somewhere in the other string to the beginning of this one
	unsigned int assign(const String &source, unsigned int srcChar, unsigned int numChar);
	/// Copies all characters from the other string to the beginning of this one
	unsigned int assign(const String &source);
	/// Copies characters from a C string to somewhere in this one
	unsigned int assign(const char *source, unsigned int numChar, unsigned int destChar);
	/// Copies characters from a C string to the beginning of this one
	unsigned int assign(const char *source, unsigned int numChar);

	/// Copies characters from somewhere in this string to a C string
	unsigned int copy(char *dest, unsigned int srcChar, unsigned int numChar) const;
	/// Copies all characters from this string to a C string
	unsigned int copy(char *dest) const;

	/// Appends all the characters from the other string to the end of this one
	unsigned int append(const String &other);
	/// Appends all the characters from the C string to the end of this one
	unsigned int append(const char *cString);

	/// Compares the string with another one in lexicographical order
	int compare(const String &other) const;
	/// Compares the string with a constant C string in lexicographical order
	int compare(const char *cString) const;

	/// Finds the first occurrence of a character
	int findFirstChar(char c) const;
	/// Finds the last occurrence of a character
	int findLastChar(char c) const;
	/// Finds the first occurrence of a character after the character at the index
	int findFirstCharAfterIndex(char c, unsigned int index) const;

	/// Finds the first occurrence of the given string
	int find(const String &other) const;
	/// Finds the first occurrence of the given constant C string
	int find(const char *cString) const;

	/// Replaces the string with the formatted result
	String &format(const char *fmt, ...);
	/// Append the formatted result to the string
	String &formatAppend(const char *fmt, ...);

	/// Appends another string to this one
	String &operator+=(const String &other);
	/// Appends a constant C string to the string object
	String &operator+=(const char *cString);
	/// Concatenate two strings together to create a third one
	String operator+(const String &other) const;
	/// Concatenates a string with a constant C string to create a third one
	String operator+(const char *cString) const;

	/// Friend operator to concatenate a constant C string with a string to create a third one
	friend DLL_PUBLIC String operator+(const char *cString, const String &string);

	inline bool operator==(const String &other) const { return (length_ != other.length_) ? false : (compare(other) == 0); }
	inline bool operator!=(const String &other) const { return (length_ != other.length_) ? true : (compare(other) != 0); }
	inline bool operator>(const String &other) const { return compare(other) > 0; }
	inline bool operator<(const String &other) const { return compare(other) < 0; }
	inline bool operator>=(const String &other) const { return compare(other) >= 0; }
	inline bool operator<=(const String &other) const { return compare(other) <= 0; }

	inline bool operator==(const char *cString) const { return compare(cString) == 0; }
	inline bool operator!=(const char *cString) const { return compare(cString) != 0; }
	inline bool operator>(const char *cString) const { return compare(cString) > 0; }
	inline bool operator<(const char *cString) const { return compare(cString) < 0; }
	inline bool operator>=(const char *cString) const { return compare(cString) >= 0; }
	inline bool operator<=(const char *cString) const { return compare(cString) <= 0; }

	/// Read-only access to the specified element (with bounds checking)
	const char &at(unsigned int index) const;
	/// Access to the specified element (with bounds checking)
	char &at(unsigned int index);
	/// Read-only subscript operator
	const char &operator[](unsigned int index) const;
	/// Subscript operator
	char &operator[](unsigned int index);

  private:
	/// Size of the local buffer
	static const unsigned int SmallBufferSize = 16;

	/// Union used for small buffer optimization
	union Buffer
	{
		char *begin_;
		char local_[SmallBufferSize];
	};

	Buffer array_;
	unsigned int length_;
	unsigned int capacity_;
};

DLL_PUBLIC String operator+(const char *cString, const String &string);

}

#endif
