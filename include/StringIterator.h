#ifndef CLASS_NCINE_STRINGITERATOR
#define CLASS_NCINE_STRINGITERATOR

#include "common_defines.h"

namespace ncine {

/// A String iterator
class DLL_PUBLIC StringIterator
{
  public:
	explicit StringIterator(char *c)
		: char_(c) { }
	StringIterator(const StringIterator& iterator)
		: char_(iterator.char_) { }

	// Read-only deferencing operator
	inline const char& operator*() const;
	// Deferencing operator
	inline char& operator*();

	// Iterates to the next element (prefix)
	StringIterator operator++() const;
	// Iterates to the next element (postfix)
	StringIterator operator++(int) const;

	// Iterates to the previous element (prefix)
	StringIterator operator--() const;
	// Iterates to the previous element (postfix)
	StringIterator operator--(int) const;

	/// Equality operator
	inline bool operator==(const StringIterator& iterator) const { return char_ == iterator.char_; }
	/// Inequality operator
	inline bool operator!=(const StringIterator& iterator) const { return char_ != iterator.char_; }

  private:
	mutable char *char_;
};

/// Read-only deferencing operator
const char& StringIterator::operator*() const
{
	return *char_;
}

/// Deferencing operator
char& StringIterator::operator*()
{
	return *char_;
}

/// Iterates to the next element (prefix)
inline StringIterator StringIterator::operator++() const
{
	char_++;

	return *this;
}

/// Iterates to the next element (postfix)
inline StringIterator StringIterator::operator++(int) const
{
	// Create an unmodified copy to return
	StringIterator iterator = *this;

	char_++;

	return iterator;
}

/// Iterates to the previous element (prefix)
inline StringIterator StringIterator::operator--() const
{
	char_--;

	return *this;
}

/// Iterates to the previous element (postfix)
inline StringIterator StringIterator::operator--(int) const
{
	// Create an unmodified copy to return
	StringIterator iterator = *this;

	char_--;

	return iterator;
}

}

#endif
