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
inline bool equalTo(const T &a, const T &b) { return a == b; }

/// Compare two objects of the same type
template <class T>
inline bool equalTo(const T *a, const T *b) { return a == b; }

template <>
inline bool equalTo<char>(const char *a, const char *b)
{
	while (*a && (*a == *b))
	{
		a++;
		b++;
	}
	return (*a == *b);
}

namespace detail {

// ---- Tags ----
struct TrivialTag {}; // Trivially copyable types
struct MovableTag {}; // Movable and copyable types
struct MoveOnlyTag {}; // Movable but non-copyable types
struct CopyableTag {}; // Copyable but non-movable types
struct NonMovableTag {}; // Non-movable and non-copyable types

template <typename T>
struct isTrivial
{
	static constexpr bool value =
		__is_trivially_constructible(T) &&
		__is_trivially_copyable(T) &&
		!__is_union(T) &&
		!__is_abstract(T);
};

// ---- ObjectPolicyTag ----
template <class T>
struct ObjectPolicyTag
{
	using type = typename conditional<
		isTrivial<T>::value, TrivialTag,
		typename conditional<
			isMoveConstructible<T>::value && isCopyConstructible<T>::value, MovableTag,
			typename conditional<
				isMoveConstructible<T>::value, MoveOnlyTag,
				typename conditional<
					isCopyConstructible<T>::value, CopyableTag,
					NonMovableTag
				>::type
			>::type
		>::type
	>::type;
};

// ---- constructHelpersImplSingle ----
template <typename T, typename Tag>
struct constructHelpersImplSingle;

template <typename T>
struct constructHelpersImplSingle<T, TrivialTag>
{
	static void constructObject(T *ptr) {}
	static void copyConstructObject(T *dest, const T *src) { memcpy(dest, src, sizeof(T)); }
	static void moveConstructObject(T *dest, T *src) { memcpy(dest, src, sizeof(T)); }
};

template <typename T>
struct constructHelpersImplSingle<T, MovableTag>
{
	static void constructObject(T *ptr) { new (ptr) T(); }
	static void copyConstructObject(T *dest, const T *src) { new (dest) T(*src); }
	static void moveConstructObject(T *dest, T *src) { new (dest) T(nctl::move(*src)); }
};

template <typename T>
struct constructHelpersImplSingle<T, MoveOnlyTag>
{
	static void constructObject(T *ptr) { new (ptr) T(); }
	static void copyConstructObject(T *dest, const T *src) { new (dest) T(nctl::move(*src)); }
	static void moveConstructObject(T *dest, T *src) { new (dest) T(nctl::move(*src)); }
};

template <typename T>
struct constructHelpersImplSingle<T, CopyableTag>
{
	static void constructObject(T *ptr) { new (ptr) T(); }
	static void copyConstructObject(T *dest, const T *src) { new (dest) T(*src); }
	static void moveConstructObject(T *dest, T *src) { new (dest) T(*src); }
};

template <typename T>
struct constructHelpersImplSingle<T, NonMovableTag>
{
	static void constructObject(T *) { static_assert(sizeof(T) == 0, "Cannot construct this type"); }
	static void copyConstructObject(T *, const T *) { static_assert(sizeof(T) == 0, "Cannot copy construct this type"); }
	static void moveConstructObject(T *, T *) { static_assert(sizeof(T) == 0, "Cannot move construct this type"); }
};

// ---- constructHelpersImplSingleArray ----
template <typename T, typename Tag>
struct constructHelpersImplSingleArray
{
	static void constructArray(T *ptr, unsigned int numElements)
	{
		for (unsigned int i = 0; i < numElements; i++)
			new (ptr + i) T();
	}
};

template <typename T>
struct constructHelpersImplSingleArray<T, TrivialTag>
{
	static void constructArray(T *, unsigned int) {}
};

template <typename T>
struct constructHelpersImplSingleArray<T, NonMovableTag>
{
	static void constructArray(T *, unsigned int numElements)
	{
		if (numElements > 0)
			static_assert(sizeof(T) == 0, "Cannot construct elements of this type");
	}
};

// ---- constructHelpersImpl ----
template <typename T, typename Tag>
struct constructHelpersImpl;

template <typename T>
struct constructHelpersImpl<T, TrivialTag>
{
	static void moveConstructArray(T *dest, T *src, unsigned int numElements)
	{
		memcpy(dest, src, numElements * sizeof(T));
	}

	static void copyConstructArray(T *dest, const T *src, unsigned int numElements)
	{
		memcpy(dest, src, numElements * sizeof(T));
	}
};

template <typename T>
struct constructHelpersImpl<T, MovableTag>
{
	static void moveConstructArray(T *dest, T *src, unsigned int numElements)
	{
		for (unsigned int i = 0; i < numElements; i++)
			new (dest + i) T(nctl::move(src[i]));
	}

	static void copyConstructArray(T *dest, const T *src, unsigned int numElements)
	{
		for (unsigned int i = 0; i < numElements; i++)
			new (dest + i) T(src[i]);
	}
};

template <typename T>
struct constructHelpersImpl<T, MoveOnlyTag>
{
	static void moveConstructArray(T *dest, T *src, unsigned int numElements)
	{
		for (unsigned int i = 0; i < numElements; i++)
			new (dest + i) T(nctl::move(src[i]));
	}

	static void copyConstructArray(T *dest, const T *src, unsigned int numElements)
	{
		for (unsigned int i = 0; i < numElements; i++)
			new (dest + i) T(nctl::move(src[i]));
	}
};

template <typename T>
struct constructHelpersImpl<T, CopyableTag>
{
	static void moveConstructArray(T *dest, T *src, unsigned int numElements)
	{
		for (unsigned int i = 0; i < numElements; i++)
			new (dest + i) T(src[i]);
	}

	static void copyConstructArray(T *dest, const T *src, unsigned int numElements)
	{
		for (unsigned int i = 0; i < numElements; i++)
			new (dest + i) T(src[i]);
	}
};

template <typename T>
struct constructHelpersImpl<T, NonMovableTag>
{
	static void moveConstructArray(T *, T *, unsigned int numElements)
	{
		if (numElements > 0)
			static_assert(sizeof(T) == 0, "Cannot move construct elements of this type");
	}

	static void copyConstructArray(T *, const T *, unsigned int numElements)
	{
		if (numElements > 0)
			static_assert(sizeof(T) == 0, "Cannot copy construct elements of this type");
	}
};

// ---- constructHelpers ----
template <class T>
struct constructHelpers
{
	using Tag = typename ObjectPolicyTag<T>::type;

	static void constructObject(T *ptr)
	{
		constructHelpersImplSingle<T, Tag>::constructObject(ptr);
	}
	static void constructArray(T *ptr, unsigned int numElements)
	{
		constructHelpersImplSingleArray<T, Tag>::constructArray(ptr, numElements);
	}

	static void copyConstructObject(T *dest, const T *src)
	{
		constructHelpersImplSingle<T, Tag>::copyConstructObject(dest, src);
	}
	static void copyConstructArray(T *dest, const T *src, unsigned int numElements)
	{
		if (numElements > 0)
			constructHelpersImpl<T, Tag>::copyConstructArray(dest, src, numElements);
	}

	static void moveConstructObject(T *dest, T *src)
	{
		constructHelpersImplSingle<T, Tag>::moveConstructObject(dest, src);
	}
	static void moveConstructArray(T *dest, T *src, unsigned int numElements)
	{
		if (numElements > 0)
			constructHelpersImpl<T, Tag>::moveConstructArray(dest, src, numElements);
	}
};

// ---- destructHelpers ----
template <bool Trivial>
struct destructHelpers
{
	template <class T>
	static void destructObject(T *ptr) { ptr->~T(); }

	template <class T>
	static void destructArray(T *ptr, unsigned int numElements)
	{
		for (unsigned int i = 0; i < numElements; i++)
			ptr[numElements - i - 1].~T();
	}
};

template <>
struct destructHelpers<true>
{
	template <class T>
	static void destructObject(T *) {}

	template <class T>
	static void destructArray(T *, unsigned int) {}
};

// ---- copyHelpersImpl ----
template <typename T, typename Tag>
struct copyHelpersImpl;

template <typename T>
struct copyHelpersImpl<T, TrivialTag>
{
	static void moveAssignArray(T *dest, T *src, unsigned int numElements)
	{
		memcpy(dest, src, numElements * sizeof(T));
	}

	static void copyAssignArray(T *dest, const T *src, unsigned int numElements)
	{
		memcpy(dest, src, numElements * sizeof(T));
	}
};

template <typename T>
struct copyHelpersImpl<T, MovableTag>
{
	static void moveAssignArray(T *dest, T *src, unsigned int numElements)
	{
		for (unsigned int i = 0; i < numElements; i++)
			dest[i] = nctl::move(src[i]);
	}

	static void copyAssignArray(T *dest, const T *src, unsigned int numElements)
	{
		for (unsigned int i = 0; i < numElements; i++)
			dest[i] = src[i];
	}
};

template <typename T>
struct copyHelpersImpl<T, MoveOnlyTag>
{
	static void moveAssignArray(T *dest, T *src, unsigned int numElements)
	{
		for (unsigned int i = 0; i < numElements; i++)
			dest[i] = nctl::move(src[i]);
	}

	static void copyAssignArray(T *dest, const T *src, unsigned int numElements)
	{
		for (unsigned int i = 0; i < numElements; i++)
			dest[i] = nctl::move(src[i]);
	}
};

template <typename T>
struct copyHelpersImpl<T, CopyableTag>
{
	static void moveAssignArray(T *dest, T *src, unsigned int numElements)
	{
		for (unsigned int i = 0; i < numElements; i++)
			dest[i] = src[i];
	}

	static void copyAssignArray(T *dest, const T *src, unsigned int numElements)
	{
		for (unsigned int i = 0; i < numElements; i++)
			dest[i] = src[i];
	}
};

template <typename T>
struct copyHelpersImpl<T, NonMovableTag>
{
	static void moveAssignArray(T *dest, T *src, unsigned int numElements)
	{
		if (numElements > 0)
			static_assert(sizeof(T) == 0, "Cannot move assign elements of this type");
	}

	static void copyAssignArray(T *, const T *, unsigned int numElements)
	{
		if (numElements > 0)
			static_assert(sizeof(T) == 0, "Cannot copy assign elements of this type");
	}
};

}

template <class T>
inline void constructObject(T *ptr) { detail::constructHelpers<T>::constructObject(ptr); }

template <class T>
inline void constructArray(T *ptr, unsigned int numElements) { detail::constructHelpers<T>::constructArray(ptr, numElements); }

template <class T>
inline void copyConstructObject(T *dest, const T *src) { detail::constructHelpers<T>::copyConstructObject(dest, src); }

template <class T>
inline void copyConstructArray(T *dest, const T *src, unsigned int numElements) { detail::constructHelpers<T>::copyConstructArray(dest, src, numElements); }

template <class T>
inline void moveConstructObject(T *dest, T *src) { detail::constructHelpers<T>::moveConstructObject(dest, src); }

template <class T>
inline void moveConstructArray(T *dest, T *src, unsigned int numElements) { detail::constructHelpers<T>::moveConstructArray(dest, src, numElements); }

template <class T>
inline void destructObject(T *ptr) { detail::destructHelpers<isTriviallyDestructible<T>::value>::destructObject(ptr); }

template <class T>
inline void destructArray(T *ptr, unsigned int numElements) { detail::destructHelpers<isTriviallyDestructible<T>::value>::destructArray(ptr, numElements); }

template <class T>
inline void copyAssignObject(T *dest, const T *src)
{
	using Tag = typename detail::ObjectPolicyTag<T>::type;
	detail::copyHelpersImpl<T, Tag>::copyAssignArray(dest, src, 1);
}

template <class T>
inline void copyAssignArray(T *dest, const T *src, unsigned int numElements)
{
	using Tag = typename detail::ObjectPolicyTag<T>::type;
	detail::copyHelpersImpl<T, Tag>::copyAssignArray(dest, src, numElements);
}

template <class T>
inline void moveAssignObject(T *dest, T *src)
{
	using Tag = typename detail::ObjectPolicyTag<T>::type;
	detail::copyHelpersImpl<T, Tag>::moveAssignArray(dest, src, 1);
}

template <class T>
inline void moveAssignArray(T *dest, T *src, unsigned int numElements)
{
	using Tag = typename detail::ObjectPolicyTag<T>::type;
	detail::copyHelpersImpl<T, Tag>::moveAssignArray(dest, src, numElements);
}

}

#endif
