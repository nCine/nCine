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

template <class T, T v>
struct integralConstant
{
	static constexpr T value = v;
	using valueType = T;
	using type = integralConstant;

	constexpr operator valueType() const noexcept { return value; }
	constexpr valueType operator()() const noexcept { return value; }
};

using trueType  = integralConstant<bool, true>;
using falseType = integralConstant<bool, false>;

template <class T>
struct removeConst
{
	using type = T;
};

template <class T>
struct removeConst<const T>
{
	using type = T;
};

template <class T>
struct removeVolatile
{
	using type = T;
};

template <class T>
struct removeVolatile<volatile T>
{
	using type = T;
};

template <class T>
struct removeCv
{
	using type = typename removeConst<typename removeVolatile<T>::type>::type;
};

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
struct isClass : falseType {};

template <class T>
struct isClass<T, typename detail::voidType<int T::*>::type>
    : integralConstant<bool, !__is_union(T)> {};

template <class T>
struct isTriviallyConstructible
{
	static constexpr bool value = __is_trivially_constructible(T);
};

template <typename T>
struct isMoveConstructible
{
	static constexpr bool value = __is_constructible(T, T &&);
};

template <class T>
struct isCopyConstructible
{
	static constexpr bool value = __is_constructible(T, const T &);
};

template <class T, class... Args>
struct isNoThrowConstructible
    : integralConstant<
          bool,
          __is_nothrow_constructible(T, Args...)
      >
{};

template <class T>
struct isTriviallyCopyable
{
	static constexpr bool value = __is_trivially_copyable(T);
};

template <class T, typename = void> struct isDestructible : falseType {};

template <class T>
struct isDestructible<T, decltype(declVal<T&>().~T())>
    : integralConstant<bool, !__is_union(T)> {};

// Use `__has_trivial_destructor()` only on GCC
#if defined(__GNUC__) && !defined(__clang__) && !defined(__INTEL_COMPILER)
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
#else
template <class T>
struct isTriviallyDestructible
{
	static constexpr bool value = __is_trivially_destructible(T);
};
#endif

template <bool B, typename T, typename F>
struct conditional
{
	using type = T;
};
template <typename T, typename F>
struct conditional<false, T, F>
{
	using type = F;
};

template <bool Condition, typename T = void>
struct enableIf
{
};
template <typename T>
struct enableIf<true, T>
{
	using type = T;
};

template <typename T, typename U> struct isSame : falseType {};
template <typename T> struct isSame<T, T> : trueType {};

template <class T> struct isIntegral : falseType {};
template <> struct isIntegral<bool> : trueType {};
template <> struct isIntegral<char> : trueType {};
template <> struct isIntegral<unsigned char> : trueType {};
template <> struct isIntegral<short int> : trueType {};
template <> struct isIntegral<unsigned short int> : trueType {};
template <> struct isIntegral<int> : trueType {};
template <> struct isIntegral<unsigned int> : trueType {};
template <> struct isIntegral<long> : trueType {};
template <> struct isIntegral<unsigned long> : trueType {};
template <> struct isIntegral<long long> : trueType {};
template <> struct isIntegral<unsigned long long> : trueType {};

}

#endif
