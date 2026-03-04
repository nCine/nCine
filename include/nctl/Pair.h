#ifndef CLASS_NCTL_PAIR
#define CLASS_NCTL_PAIR

#include <ncine/common_macros.h>

namespace nctl {

/// A class template that provides a way to store two heterogeneous objects as a single unit
template <class T1, class T2>
struct Pair
{
	using firstType = T1;
	using secondType = T2;

	T1 first;
	T2 second;

	constexpr Pair() = default;

	constexpr Pair(const T1 &a, const T2 &b)
	    : first(a), second(b) {}

	template <class U1, class U2>
	CONSTEXPR14 Pair(U1 &&a, U2 &&b)
	    : first(static_cast<U1 &&>(a)),
	      second(static_cast<U2 &&>(b)) {}

	void swap(Pair &other)
	{
		using nctl::swap;
		swap(first, other.first);
		swap(second, other.second);
	}

	friend void swap(Pair &a, Pair &b)
	{
		a.swap(b);
	}

	friend constexpr bool operator==(const Pair &a, const Pair &b)
	{
		return a.first == b.first && a.second == b.second;
	}

	friend constexpr bool operator!=(const Pair &a, const Pair &b)
	{
		return !(a == b);
	}

	friend constexpr bool operator<(const Pair &a, const Pair &b)
	{
		if (a.first < b.first)
			return true;
		if (b.first < a.first)
			return false;
		return a.second < b.second;
	}

	friend constexpr bool operator>(const Pair &a, const Pair &b) { return b < a; }
	friend constexpr bool operator<=(const Pair &a, const Pair &b) { return !(b < a); }
	friend constexpr bool operator>=(const Pair &a, const Pair &b) { return !(a < b); }
};

template <class T1, class T2>
CONSTEXPR14 Pair<typename Decay<T1>::Type, typename Decay<T2>::Type>
makePair(T1 &&a, T2 &&b)
{
	return Pair<typename Decay<T1>::Type, typename Decay<T2>::Type>(
		static_cast<T1 &&>(a),
		static_cast<T2 &&>(b));
}

}

#endif
