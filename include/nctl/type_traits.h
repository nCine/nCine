#ifndef NCTL_TYPETRAITS
#define NCTL_TYPETRAITS

namespace nctl {

template <class T>
struct removeReference
{
	using type = T;
};
template <class T>
struct removeReference<T &>
{
	using type = T;
};
template <class T>
struct removeReference<T &&>
{
	using type = T;
};

template <class T>
struct isLValueReference
{
	static constexpr bool value = false;
};
template <class T>
struct isLValueReference<T &>
{
	static constexpr bool value = true;
};

template <class T>
struct removeExtent
{
	typedef T type;
};
template <class T>
struct removeExtent<T[]>
{
	typedef T type;
};
template <class T, unsigned int N>
struct removeExtent<T[N]>
{
	typedef T type;
};
template <class T>
using removeExtentT = typename removeExtent<T>::type;

}

#endif
