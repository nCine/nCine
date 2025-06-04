#ifndef NCTL_UTILITY
#define NCTL_UTILITY

#include <cstring> // for `memcpy()`
#include "type_traits.h"

namespace nctl {

/// It is used to indicate that an object may be "moved from"
template <class T>
inline typename removeReference<T>::type &&move(T &&arg)
{
	return static_cast<typename removeReference<T>::type &&>(arg);
}

namespace detail {

	/// A container for functions to construct objects and arrays of objects
	template <bool value>
	struct constructHelpers
	{
		template <class T>
		inline static void constructObject(T *ptr)
		{
			new (ptr) T();
		}

		template <class T>
		inline static void constructArray(T *ptr, unsigned int numElements)
		{
			for (unsigned int i = 0; i < numElements; i++)
				new (ptr + i) T();
		}

		template <class T>
		inline static void copyConstructArray(T *dest, const T *src, unsigned int numElements)
		{
			for (unsigned int i = 0; i < numElements; i++)
				new (dest + i) T(src[i]);
		}

		template <class T>
		inline static void moveConstructArray(T *dest, T *src, unsigned int numElements)
		{
			for (unsigned int i = 0; i < numElements; i++)
				new (dest + i) T(nctl::move(src[i]));
		}
	};

	/// Specialization for trivially constructible types
	template <>
	struct constructHelpers<true>
	{
		template <class T>
		inline static void constructObject(T *ptr)
		{
		}

		template <class T>
		inline static void constructArray(T *ptr, unsigned int numElements)
		{
		}

		template <class T>
		inline static void copyConstructArray(T *dest, const T *src, unsigned int numElements)
		{
			memcpy(dest, src, numElements * sizeof(T));
		}

		template <class T>
		inline static void moveConstructArray(T *dest, T *src, unsigned int numElements)
		{
			memcpy(dest, src, numElements * sizeof(T));
		}
	};

	/// A container for functions to destruct objects and arrays of objects
	template <bool value>
	struct destructHelpers
	{
		template <class T>
		inline static void destructObject(T *ptr)
		{
			ptr->~T();
		}

		template <class T>
		inline static void destructArray(T *ptr, unsigned int numElements)
		{
			for (unsigned int i = 0; i < numElements; i++)
				ptr[numElements - i - 1].~T();
		}
	};

	/// Specialization for trivially destructible types
	template <>
	struct destructHelpers<true>
	{
		template <class T>
		inline static void destructObject(T *ptr)
		{
		}

		template <class T>
		inline static void destructArray(T *ptr, unsigned int numElements)
		{
		}
	};

	/// A container for functions to copy arrays of objects
	template <bool value>
	struct copyHelpers
	{
		template <class T>
		inline static void copyAssignArray(T *dest, const T *src, unsigned int numElements)
		{
			for (unsigned int i = 0; i < numElements; i++)
				dest[i] = src[i];
		}

		template <class T>
		inline static void moveAssignArray(T *dest, T *src, unsigned int numElements)
		{
			for (unsigned int i = 0; i < numElements; i++)
				dest[i] = nctl::move(src[i]);
		}
	};

	/// Specialization for trivially copyable types
	template <>
	struct copyHelpers<true>
	{
		template <class T>
		inline static void copyAssignArray(T *dest, const T *src, unsigned int numElements)
		{
			memcpy(dest, src, numElements * sizeof(T));
		}

		template <class T>
		inline static void moveAssignArray(T *dest, T *src, unsigned int numElements)
		{
			memcpy(dest, src, numElements * sizeof(T));
		}
	};

}

/// Forwards lvalues as either lvalues or as rvalues, depending on T
template <typename T>
inline constexpr T &&forward(typename removeReference<T>::type &arg)
{
	return static_cast<T &&>(arg);
}

template <typename T>
inline constexpr T &&forward(typename removeReference<T>::type &&arg)
{
	static_assert(!isLValueReference<T>::value, "Invalid lvalue to rvalue conversion");
	return static_cast<T &&>(arg);
}

/// Swaps the content of two objects of the same type
template <class T>
inline void swap(T &a, T &b)
{
	T temp = nctl::move(a);
	a = nctl::move(b);
	b = nctl::move(temp);
}

/// Compare two objects of the same type
template <class T>
inline bool equalTo(const T &a, const T &b)
{
	return (a == b);
}

/// Compare two objects of the same type
template <class T>
inline bool equalTo(const T *a, const T *b)
{
	return (a == b);
}

template <>
inline bool equalTo<char>(const char *a, const char *b)
{
	// `strcmp()` reimplementation to avoid additional dependencies
	while (*a && (*a == *b))
	{
		a++;
		b++;
	}
	const unsigned char result = *reinterpret_cast<const unsigned char *>(a) -
	                             *reinterpret_cast<const unsigned char *>(b);

	return (result == 0);
}

template <class T>
void constructObject(T *ptr)
{
	detail::constructHelpers<isTriviallyConstructible<T>::value>::constructObject(ptr);
}

template <class T>
void constructArray(T *ptr, unsigned int numElements)
{
	detail::constructHelpers<isTriviallyConstructible<T>::value>::constructArray(ptr, numElements);
}

template <class T>
void copyConstructArray(T *dest, const T *src, unsigned int numElements)
{
	detail::constructHelpers<isTriviallyConstructible<T>::value>::copyConstructArray(dest, src, numElements);
}

template <class T>
void moveConstructArray(T *dest, T *src, unsigned int numElements)
{
	detail::constructHelpers<isTriviallyConstructible<T>::value>::moveConstructArray(dest, src, numElements);
}

template <class T>
void destructObject(T *ptr)
{
	detail::destructHelpers<isTriviallyDestructible<T>::value>::destructObject(ptr);
}

template <class T>
void destructArray(T *ptr, unsigned int numElements)
{
	detail::destructHelpers<isTriviallyDestructible<T>::value>::destructArray(ptr, numElements);
}

template <class T>
void copyAssignArray(T *dest, const T *src, unsigned int numElements)
{
	detail::copyHelpers<isTriviallyCopyable<T>::value>::copyAssignArray(dest, src, numElements);
}

template <class T>
void moveAssignArray(T *dest, T *src, unsigned int numElements)
{
	detail::copyHelpers<isTriviallyCopyable<T>::value>::moveAssignArray(dest, src, numElements);
}

}

#endif
