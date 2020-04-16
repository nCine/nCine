#ifndef NCTL_TYPETRAITS
#define NCTL_TYPETRAITS

namespace nctl {

namespace detail {

	template <class T>
	struct typeIdentity
	{
		using type = T;
	};

	template <class T>
	auto tryAddLValueReference(int) -> typeIdentity<T &>;
	template <class T>
	auto tryAddLValueReference(...) -> typeIdentity<T>;

	template <class T>
	auto tryAddRValueReference(int) -> typeIdentity<T &&>;
	template <class T>
	auto tryAddRValueReference(...) -> typeIdentity<T>;

	template <class>
	struct voidType
	{
		typedef void type;
	};

}

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

template <class T>
struct addLValueReference : decltype(detail::tryAddLValueReference<T>(0))
{};
template <class T>
struct addRValueReference : decltype(detail::tryAddRValueReference<T>(0))
{};
template <class T>
typename addRValueReference<T>::type declVal();

template <class T>
struct isEmpty
{
	static constexpr bool value = __is_empty(T);
};

template <class T, typename = void>
struct isClass
{
	static constexpr bool value = false;
};
template <class T>
struct isClass<T, typename detail::voidType<int T::*>::type>
{
	static constexpr bool value = (true && !__is_union(T));
};

template <class T>
struct isTriviallyConstructible
{
	static constexpr bool value = __is_trivially_constructible(T);
};

template <class T>
struct isTriviallyCopyable
{
	static constexpr bool value = __is_trivially_copyable(T);
};

template <class T, typename = void>
struct isDestructible
{
	static constexpr bool value = false;
};
template <class T>
struct isDestructible<T, decltype(declVal<T &>().~T())>
{
	static constexpr bool value = (true && !__is_union(T));
};

template <class T>
struct hasTrivialDestructor
{
	static constexpr bool value = __has_trivial_destructor(T);
};

template <class T>
struct isTriviallyDestructible
{
	static constexpr bool value = isDestructible<T>::value && hasTrivialDestructor<T>::value;
};

template <class T>
struct isIntegral
{
	static constexpr bool value = false;
};
template <>
struct isIntegral<bool>
{
	static constexpr bool value = true;
};
template <>
struct isIntegral<char>
{
	static constexpr bool value = true;
};
template <>
struct isIntegral<unsigned char>
{
	static constexpr bool value = true;
};
template <>
struct isIntegral<short int>
{
	static constexpr bool value = true;
};
template <>
struct isIntegral<unsigned short int>
{
	static constexpr bool value = true;
};
template <>
struct isIntegral<int>
{
	static constexpr bool value = true;
};
template <>
struct isIntegral<unsigned int>
{
	static constexpr bool value = true;
};
template <>
struct isIntegral<long>
{
	static constexpr bool value = true;
};
template <>
struct isIntegral<unsigned long>
{
	static constexpr bool value = true;
};
template <>
struct isIntegral<long long>
{
	static constexpr bool value = true;
};
template <>
struct isIntegral<unsigned long long>
{
	static constexpr bool value = true;
};

}

#endif
