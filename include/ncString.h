#ifndef CLASS_NCINE_STRING
#define CLASS_NCINE_STRING

#include "algorithms.h"
#include "StringIterator.h"

namespace ncine {

/// A basic string class made of chars
class DLL_PUBLIC String
{
  public:
	/// Iterator type
	using Iterator = StringIterator<false>;
	/// Constant iterator type
	using ConstIterator = StringIterator<true>;

	/// Default capacity for objects created by the default constructor
	static const unsigned int DefaultCapacity = 128;
	/// Maximum length when creating an object from C-style strings
	static const unsigned int MaxCStringLength = DefaultCapacity - 1;

	/// Default constructor
	String();
	/// Constructs an empty string with explicit size
	explicit String(unsigned int capacity);
	/// Constructs a string object from a C string
	String(const char *cString);
	~String();

	/// Copy constructor
	String(const String &other);
	/// Assignment operator that preserves the original string capacity
	String &operator=(const String &other);
	/// Assigns a constant C string to the string object
	String &operator=(const char *cString);

	/// Swaps two strings without copying their data
	void swap(String &first, String &second)
	{
		nc::swap(first.array_, second.array_);
		nc::swap(first.length_, second.length_);
		nc::swap(first.capacity_, second.capacity_);
	}

	/// Returns an iterator to the first character
	inline Iterator begin() { return Iterator(array_); }
	/// Returns an iterator to the last character
	inline Iterator rBegin() { return Iterator(array_ + length_ - 1); }
	/// Returns an iterator to the termination character
	inline Iterator end() { return Iterator(array_ + length_); }
	/// Returns an iterator to the byte preceding the first character
	inline Iterator rEnd() { return Iterator(array_ - 1); }

	/// Returns a constant iterator to the first character
	inline ConstIterator begin() const { return ConstIterator(array_); }
	/// Returns a constant iterator to the last character
	inline ConstIterator rBegin() const { return ConstIterator(array_ + length_ - 1); }
	/// Returns a constant iterator to the termination character
	inline ConstIterator end() const { return ConstIterator(array_ + length_); }
	/// Returns a constant iterator to the byte preceding the first character
	inline ConstIterator rEnd() const { return ConstIterator(array_ - 1); }

	/// Returns true if the string is empty
	inline bool isEmpty() const { return length_ == 0; }
	/// Returns the string length
	inline unsigned int length() const { return length_; }
	/// Returns the string capacity
	inline unsigned int capacity() const { return capacity_; }

	/// Clears the string
	/*! Length will be zero but capacity remains unmodified. */
	inline void clear() { length_ = 0; array_[0] = '\0'; }

	/// Returns a pointer to the internal array
	char *data() { return array_; }
	/// Returns a constant pointer to the internal array
	const char *data() const { return array_; }

	/// Copies characters from somewhere in the source to somewhere in the destination
	unsigned int copy(String &dest, unsigned int srcChar, unsigned int numChar, unsigned int destChar) const;
	/// Copies characters from somewhere in the source to the beginning of the destination
	unsigned int copy(String &dest, unsigned int srcChar, unsigned int numChar) const;
	/// Copies all the characters from the source to the beginning of the destination
	unsigned int copy(String &dest) const;
	/// Copies characters from the source string to a C string
	unsigned int copy(char *dest, unsigned int srcChar, unsigned int numChar) const;
	/// Appends all the characters from the source to the end of the destination
	unsigned int append(const String &source);

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

	char operator[](unsigned int index) const;

  private:
	char *array_;
	unsigned int length_;
	unsigned int capacity_;
};

inline char String::operator[](unsigned int index) const
{
	if (index < length_)
		return array_[index];
	else
		return '\0';
}

DLL_PUBLIC String operator+(const char *cString, const String &string);

}

#endif
