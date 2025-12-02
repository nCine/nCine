#ifndef CLASS_NCTL_STRINGVIEW
#define CLASS_NCTL_STRINGVIEW

#include <ncine/common_macros.h>
#include "StringIterator.h"
#include "ReverseIterator.h"
#include "utility.h"

namespace nctl {

/// A string view class over an array of chars
class DLL_PUBLIC StringView
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

	/// Maximum length when creating an view from a C-style string with no specified capacity
	static constexpr unsigned int MaxCStringLength = 512 - 1;

	/// Constructs a string view that needs to be set before use
	StringView();
	/// Constructs a string view over an array of chars
	StringView(char *array);
	/// Constructs a string view over an array of chars of the specified capacity
	StringView(char *array, unsigned int capacity);
	/// Constructs a string view over an array of chars of the specified capacity and length
	StringView(char *array, unsigned int capacity, unsigned int length);

	/// Copy constructor
	StringView(const StringView &other);
	/// Move constructor
	StringView(StringView &&other);
	/// Assignment operator (it might extend or truncate the original text)
	StringView &operator=(const StringView &other);
	/// Move assignment operator
	StringView &operator=(StringView &&other);
	/// Assigns a constant C string to the string view object
	StringView &operator=(const char *cString);

	/// Swaps two strings without copying their data
	inline void swap(StringView &first, StringView &second)
	{
		nctl::swap(first.array_, second.array_);
		nctl::swap(first.length_, second.length_);
		nctl::swap(first.capacity_, second.capacity_);
	}

	/// Returns an iterator to the first character
	inline Iterator begin() { return Iterator(data()); }
	/// Returns a reverse iterator to the last character
	inline ReverseIterator rBegin()
	{
		if (length_ == 0)
			return ReverseIterator(end());
		return ReverseIterator(Iterator(data() + length_ - 1));
	}
	/// Returns an iterator to the termination character
	inline Iterator end() { return Iterator(data() + length_); }
	/// Returns a reverse iterator to the byte preceding the first character
	inline ReverseIterator rEnd()
	{
		if (length_ == 0)
			return ReverseIterator(end());
		return ReverseIterator(Iterator(data() - 1));
	}

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
	inline ConstReverseIterator crBegin() const
	{
		if (length_ == 0)
			return ConstReverseIterator(end());
		return ConstReverseIterator(ConstIterator(data() + length_ - 1));
	}
	/// Returns a constant iterator to the termination character
	inline ConstIterator cEnd() const { return ConstIterator(data() + length_); }
	/// Returns a constant reverse iterator to the byte preceding the first character
	inline ConstReverseIterator crEnd() const
	{
		if (length_ == 0)
			return ConstReverseIterator(end());
		return ConstReverseIterator(ConstIterator(data() - 1));
	}

	/// Returns true if the string view is empty
	inline bool isEmpty() const { return length_ == 0; }
	/// Returns the string view length
	inline unsigned int length() const { return length_; }
	/// Returns the array capacity
	inline unsigned int capacity() const { return capacity_; }
	/// Sets the string view length
	unsigned int setLength(unsigned int newLength);

	/// Sets the string view over an array of chars
	void set(char *array);
	/// Sets the string view over an array of chars of the specified capacity
	void set(char *array, unsigned int capacity);
	/// Sets the string view over an array of chars of the specified capacity and length
	void set(char *array, unsigned int capacity, unsigned int length);
	/// Clears the string view
	void clear();

	/// Returns a pointer to the internal array
	inline char *data() { return array_; }
	/// Returns a constant pointer to the internal array
	inline const char *data() const { return array_; }

	/// Moves the start of the view forward by the specified number of characters
	void removePrefix(unsigned int num);
	/// Moves the end of the view back by the specified number of characters
	void removeSuffix(unsigned int num);

	/// Replaces characters from somewhere in the other string view to somewhere in this one (no truncation)
	unsigned int replace(const StringView &source, unsigned int srcChar, unsigned int numChar, unsigned int destChar);
	/// Replaces characters from a C string to somewhere in this one (no truncation)
	unsigned int replace(const char *source, unsigned int numChar, unsigned int destChar);

	/// Copies characters from somewhere in the other string view to the beginning of this one
	unsigned int assign(const StringView &source, unsigned int srcChar, unsigned int numChar);
	/// Copies all characters from the other string view to the beginning of this one
	unsigned int assign(const StringView &source);
	/// Copies characters from a C string to the beginning of this one
	unsigned int assign(const char *source, unsigned int numChar);

	/// Copies characters from somewhere in this string to a C string
	unsigned int copy(char *dest, unsigned int srcChar, unsigned int numChar) const;
	/// Copies all characters from this string to a C string
	unsigned int copy(char *dest) const;

	/// Appends all the characters from the other string view to the end of this one
	unsigned int append(const StringView &other);
	/// Appends all the characters from the C string to the end of this one
	unsigned int append(const char *cString);

	/// Compares the string view with another one in lexicographical order
	int compare(const StringView &other) const;
	/// Compares the string with a constant C string in lexicographical order
	int compare(const char *cString) const;

	/// Finds the first occurrence of a character
	int findFirstChar(char c) const;
	/// Finds the last occurrence of a character
	int findLastChar(char c) const;
	/// Finds the first occurrence of a character after the character at the index
	int findFirstCharAfterIndex(char c, unsigned int index) const;

	/// Finds the first occurrence of the given string view
	int find(const StringView &other) const;
	/// Finds the first occurrence of the given constant C string
	int find(const char *cString) const;

	/// Replaces the string view with the formatted result
	StringView &format(const char *fmt, ...);
	/// Append the formatted result to the string view
	StringView &formatAppend(const char *fmt, ...);

	/// Appends another string view to this one
	StringView &operator+=(const StringView &other);
	/// Appends a constant C string to the string view obejct
	StringView &operator+=(const char *cString);

	inline bool operator==(const StringView &other) const { return (length_ != other.length_) ? false : (compare(other) == 0); }
	inline bool operator!=(const StringView &other) const { return (length_ != other.length_) ? true : (compare(other) != 0); }
	inline bool operator>(const StringView &other) const { return compare(other) > 0; }
	inline bool operator<(const StringView &other) const { return compare(other) < 0; }
	inline bool operator>=(const StringView &other) const { return compare(other) >= 0; }
	inline bool operator<=(const StringView &other) const { return compare(other) <= 0; }

	inline bool operator==(const char *cString) const { return compare(cString) == 0; }
	inline bool operator!=(const char *cString) const { return compare(cString) != 0; }
	inline bool operator>(const char *cString) const { return compare(cString) > 0; }
	inline bool operator<(const char *cString) const { return compare(cString) < 0; }
	inline bool operator>=(const char *cString) const { return compare(cString) >= 0; }
	inline bool operator<=(const char *cString) const { return compare(cString) <= 0; }

	friend inline bool operator==(const char *cString, const StringView &string) { return string.compare(cString) == 0; }
	friend inline bool operator!=(const char *cString, const StringView &string) { return string.compare(cString) != 0; }
	friend inline bool operator>(const char *cString, const StringView &string) { return string.compare(cString) <= 0; }
	friend inline bool operator<(const char *cString, const StringView &string) { return string.compare(cString) >= 0; }
	friend inline bool operator>=(const char *cString, const StringView &string) { return string.compare(cString) < 0; }
	friend inline bool operator<=(const char *cString, const StringView &string) { return string.compare(cString) > 0; }

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
	char *array_;
	unsigned int length_;
	unsigned int capacity_;
};

DLL_PUBLIC StringView operator+(const char *cString, const StringView &string);

}

#endif
