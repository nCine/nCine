#ifndef CLASS_NCTL_VARIANT
#define CLASS_NCTL_VARIANT

#include <cstdint>
#include <new>
#include <ncine/common_macros.h>
#include "utility.h"

namespace nctl {

namespace detail {

template <class... Ts> struct MaxSizeOf;
template <class T> struct MaxSizeOf<T> { static constexpr size_t value = sizeof(T); };
template <class T, class... Rest> struct MaxSizeOf<T, Rest...>
{
	static constexpr size_t value = sizeof(T) > MaxSizeOf<Rest...>::value ? sizeof(T) : MaxSizeOf<Rest...>::value;
};

template <class T, class... Ts> struct IndexOf;
template <class T> struct IndexOf<T> { static constexpr uint8_t value = 0; }; // not found: dead code, guarded by an `IsOneOf` static_assert
template <class T, class... Rest> struct IndexOf<T, T, Rest...> { static constexpr uint8_t value = 0; };
template <class T, class U, class... Rest> struct IndexOf<T, U, Rest...> { static constexpr uint8_t value = 1 + IndexOf<T, Rest...>::value; };

template <class T, class... Ts> struct IsOneOf;
template <class T> struct IsOneOf<T> { static constexpr bool value = false; };
template <class T, class... Rest> struct IsOneOf<T, T, Rest...> { static constexpr bool value = true; };
template <class T, class U, class... Rest> struct IsOneOf<T, U, Rest...> { static constexpr bool value = IsOneOf<T, Rest...>::value; };

template <class T, class... Ts> struct CountOf;
template <class T> struct CountOf<T> { static constexpr size_t value = 0; };
template <class T, class... Rest> struct CountOf<T, T, Rest...> { static constexpr size_t value = 1 + CountOf<T, Rest...>::value; };
template <class T, class U, class... Rest> struct CountOf<T, U, Rest...> { static constexpr size_t value = CountOf<T, Rest...>::value; };

template <class... Ts> struct HasDuplicates;
template <> struct HasDuplicates<> { static constexpr bool value = false; };
template <class T, class... Rest> struct HasDuplicates<T, Rest...>
{
	static constexpr bool value = (CountOf<T, Rest...>::value > 0) || HasDuplicates<Rest...>::value;
};

template <class T, class...> struct FirstOf { using type = T; };

template <uint8_t I, class... Rest> struct DestroyHelper;
template <uint8_t I>
struct DestroyHelper<I> // empty pack: terminator, unreachable in practice
{
	static void destroy(uint8_t, void *) {}
};

template <uint8_t I, class T, class... Rest>
struct DestroyHelper<I, T, Rest...>
{
	static void destroy(uint8_t tag, void *storage)
	{
		if (tag == I)
			static_cast<T *>(storage)->~T();
		else
			DestroyHelper<I + 1, Rest...>::destroy(tag, storage);
	}
};

template <uint8_t I, class... Rest> struct CopyHelper;
template <uint8_t I> struct CopyHelper<I> { static void copy(uint8_t, void *, const void *) {} };
template <uint8_t I, class T, class... Rest>
struct CopyHelper<I, T, Rest...>
{
	static void copy(uint8_t tag, void *dst, const void *src)
	{
		if (tag == I)
			new (dst) T(*static_cast<const T *>(src));
		else
			CopyHelper<I + 1, Rest...>::copy(tag, dst, src);
	}
};

template <uint8_t I, class... Rest> struct MoveHelper;
template <uint8_t I> struct MoveHelper<I> { static void move(uint8_t, void *, void *) {} };
template <uint8_t I, class T, class... Rest>
struct MoveHelper<I, T, Rest...>
{
	static void move(uint8_t tag, void *dst, void *src)
	{
		if (tag == I)
			new (dst) T(nctl::move(*static_cast<T *>(src)));
		else
			MoveHelper<I + 1, Rest...>::move(tag, dst, src);
	}
};

template <uint8_t I, class... Rest> struct CopyAssignHelper;
template <uint8_t I> struct CopyAssignHelper<I> { static void assign(uint8_t, void *, const void *) {} };
template <uint8_t I, class T, class... Rest>
struct CopyAssignHelper<I, T, Rest...>
{
	static void assign(uint8_t tag, void *dst, const void *src)
	{
		if (tag == I)
			*static_cast<T *>(dst) = *static_cast<const T *>(src);
		else
			CopyAssignHelper<I + 1, Rest...>::assign(tag, dst, src);
	}
};

template <uint8_t I, class... Rest> struct MoveAssignHelper;
template <uint8_t I> struct MoveAssignHelper<I> { static void assign(uint8_t, void *, void *) {} };
template <uint8_t I, class T, class... Rest>
struct MoveAssignHelper<I, T, Rest...>
{
	static void assign(uint8_t tag, void *dst, void *src)
	{
		if (tag == I)
			*static_cast<T *>(dst) = nctl::move(*static_cast<T *>(src));
		else
			MoveAssignHelper<I + 1, Rest...>::assign(tag, dst, src);
	}
};

template <uint8_t I, class Storage, class Visitor, class... Rest> struct VisitHelper;

template <uint8_t I, class Storage, class Visitor, class T>
struct VisitHelper<I, Storage, Visitor, T>
{
	template <class R>
	static R visit(uint8_t, Visitor &&visitor, Storage storage)
	{
		using ElemPtr = typename conditional<isSame<Storage, const void *>::value, const T *, T *>::type;
		return visitor(*static_cast<ElemPtr>(storage));
	}
};

template <uint8_t I, class Storage, class Visitor, class T, class U, class... Rest>
struct VisitHelper<I, Storage, Visitor, T, U, Rest...>
{
	template <class R>
	static R visit(uint8_t tag, Visitor &&visitor, Storage storage)
	{
		if (tag == I)
		{
			using ElemPtr = typename conditional<isSame<Storage, const void *>::value, const T *, T *>::type;
			return visitor(*static_cast<ElemPtr>(storage));
		}
		else
			return VisitHelper<I + 1, Storage, Visitor, U, Rest...>::template visit<R>(tag, nctl::forward<Visitor>(visitor), storage);
	}
};

}

/// A type-safe tagged union that can hold a value of one of its template alternative types
template <class... Ts>
class Variant
{
	static_assert(sizeof...(Ts) <= 256, "Variant supports at most 256 alternative types due to the `uint8_t` tag");
	static_assert(!detail::HasDuplicates<Ts...>::value, "Variant alternative types must be unique");

  public:
	/// Constructs a variant default-constructing its first alternative type
	Variant()
	    : tag_(0)
	{
		new (&storage_) typename detail::FirstOf<Ts...>::type();
	}

	/// Constructs a variant holding a value of one of its alternative types
	template <class T, class D = typename Decay<T>::Type,
	          typename enableIf<!isSame<D, Variant>::value, int>::type = 0>
	Variant(T &&value)
	    : tag_(detail::IndexOf<D, Ts...>::value)
	{
		static_assert(detail::IsOneOf<D, Ts...>::value, "T must be one of the variant's alternative types");
		new (&storage_) D(nctl::forward<T>(value));
	}

	~Variant() { detail::DestroyHelper<0, Ts...>::destroy(tag_, &storage_); }

	/// Copy constructor
	Variant(const Variant &other) : tag_(other.tag_) { detail::CopyHelper<0, Ts...>::copy(tag_, &storage_, &other.storage_); }
	/// Move constructor
	Variant(Variant &&other) noexcept : tag_(other.tag_) { detail::MoveHelper<0, Ts...>::move(tag_, &storage_, &other.storage_); }

	/// Destroys the current value and constructs a new one of the specified type in its place
	template <class T, class... Args>
	void emplace(Args &&... args)
	{
		static_assert(detail::IsOneOf<T, Ts...>::value, "T must be one of the variant's alternative types");
		detail::DestroyHelper<0, Ts...>::destroy(tag_, &storage_);
		new (&storage_) T(nctl::forward<Args>(args)...);
		tag_ = detail::IndexOf<T, Ts...>::value;
	}

	/// Assignment operator
	Variant &operator=(const Variant &other)
	{
		if (this == &other)
			return *this;

		if (tag_ == other.tag_)
			detail::CopyAssignHelper<0, Ts...>::assign(tag_, &storage_, &other.storage_);
		else
		{
			detail::DestroyHelper<0, Ts...>::destroy(tag_, &storage_);
			detail::CopyHelper<0, Ts...>::copy(other.tag_, &storage_, &other.storage_);
			tag_ = other.tag_;
		}
		return *this;
	}

	/// Move assignment operator
	Variant &operator=(Variant &&other) noexcept
	{
		if (this == &other)
			return *this;

		if (tag_ == other.tag_)
			detail::MoveAssignHelper<0, Ts...>::assign(tag_, &storage_, &other.storage_);
		else
		{
			detail::DestroyHelper<0, Ts...>::destroy(tag_, &storage_);
			detail::MoveHelper<0, Ts...>::move(other.tag_, &storage_, &other.storage_);
			tag_ = other.tag_;
		}
		return *this;
	}

	/// Returns true if the variant currently holds a value of the specified type
	template <class T>
	bool holds() const { return tag_ == detail::IndexOf<T, Ts...>::value; }

	/// Returns a reference to the currently held value
	template <class T>
	T &get()
	{
		FATAL_ASSERT(holds<T>());
		return *reinterpret_cast<T *>(&storage_);
	}

	/// Returns a constant reference to the currently held value
	template <class T>
	const T &get() const
	{
		FATAL_ASSERT(holds<T>());
		return *reinterpret_cast<const T *>(&storage_);
	}

	/// Returns a pointer to the currently held value if it is of the specified type, `nullptr` otherwise
	template <class T>
	T *getIf() { return holds<T>() ? reinterpret_cast<T *>(&storage_) : nullptr; }

	/// Returns a constant pointer to the currently held value if it is of the specified type, `nullptr` otherwise
	template <class T>
	const T *getIf() const { return holds<T>() ? reinterpret_cast<const T *>(&storage_) : nullptr; }

	/// Calls the visitor with a reference to the currently held value, and returns its result
	template <class Visitor>
	auto visit(Visitor &&visitor) -> decltype(visitor(nctl::declVal<typename detail::FirstOf<Ts...>::type &>()))
	{
		using R = decltype(visitor(nctl::declVal<typename detail::FirstOf<Ts...>::type &>()));
		return detail::VisitHelper<0, void *, Visitor, Ts...>::template visit<R>(tag_, nctl::forward<Visitor>(visitor), &storage_);
	}

	/// Calls the visitor with a constant reference to the currently held value, and returns its result
	template <class Visitor>
	auto visit(Visitor &&visitor) const -> decltype(visitor(nctl::declVal<const typename detail::FirstOf<Ts...>::type &>()))
	{
		using R = decltype(visitor(nctl::declVal<const typename detail::FirstOf<Ts...>::type &>()));
		return detail::VisitHelper<0, const void *, Visitor, Ts...>::template visit<R>(tag_, nctl::forward<Visitor>(visitor), &storage_);
	}

  private:
	/// Raw storage for the object, intentionally uninitialized
	alignas(Ts...) unsigned char storage_[detail::MaxSizeOf<Ts...>::value];
	/// The index of the currently held type within the template parameter pack
	uint8_t tag_;
};

}

#endif
