#ifndef CLASS_NCTL_UNIQUEPTR
#define CLASS_NCTL_UNIQUEPTR

#include "utility.h"

#include <ncine/config.h>
#if NCINE_WITH_ALLOCATORS
	#include "AllocManager.h"
	#include "IAllocator.h"
#endif

using nullptr_t = decltype(nullptr);

namespace nctl {

namespace detail {

	/// Simple pair implementation for pointer and deleter
	/*! It uses empty base optimization for stateless deleters */
	template <class T1, class T2, bool value>
	struct PairImpl
	{
		T1 first;
		T2 second;

		PairImpl(T1 ff, T2 ss)
		    : first(ff), second(ss) {}
		PairImpl(const PairImpl &other)
		    : first(other.first), second(other.second) {}

		inline void invokeSecond(T1 ff) { second.operator()(ff); }
		inline void swapSecond(PairImpl &other) { nctl::swap(second, other.second); }
	};

	/// Specialization for empty second type
	template <class T1, class T2>
	struct PairImpl<T1, T2, true>
	{
		T1 first;

		PairImpl(T1 ff)
		    : first(ff) {}
		PairImpl(T1 ff, T2 ss)
		    : first(ff) {}
		PairImpl(const PairImpl &other)
		    : first(other.first) {}

		inline void invokeSecond(T1 ff)
		{
			T2 second;
			second.operator()(ff);
		}
		inline void swapSecond(PairImpl &other) {}
	};

	template <class T1, class T2>
	using Pair = PairImpl<T1, T2, isEmpty<T2>::value>;

}

#if !NCINE_WITH_ALLOCATORS
template <class T>
class DefaultDelete
{
  public:
	void operator()(T *ptr) const { delete ptr; }
};

template <class T>
class DefaultDelete<T[]>
{
  public:
	void operator()(T *ptr) const { delete[] ptr; }
};
#else
template <class T>
class DefaultDelete
{
  public:
	void operator()(T *ptr) const { theDefaultAllocator().deleteObject(ptr); }
};

template <class T>
class DefaultDelete<T[]>
{
  public:
	void operator()(T *ptr) const { theDefaultAllocator().deleteArray(ptr); }
};

template <class T>
class AllocDelete
{
  public:
	AllocDelete(IAllocator *alloc)
	    : alloc_(alloc) {}
	void operator()(T *ptr) const { alloc_->deleteObject(ptr); }

  private:
	IAllocator *alloc_;
};

template <class T>
class AllocDelete<T[]>
{
  public:
	AllocDelete(IAllocator *alloc)
	    : alloc_(alloc) {}
	void operator()(T *ptr) const { alloc_->deleteArray(ptr); }

  private:
	IAllocator *alloc_;
};
#endif

/// A unique pointer implementation
template <class T, class Deleter = DefaultDelete<T>>
class UniquePtr
{
  public:
	UniquePtr()
	    : pair_(nullptr) {}
	explicit UniquePtr(T *ptr)
	    : pair_(ptr) {}
	UniquePtr(T *ptr, Deleter del)
	    : pair_(ptr, del) {}
	template <class U>
	explicit UniquePtr(U *ptr)
	    : pair_(ptr) {}
	template <class U>
	UniquePtr(U *ptr, Deleter del)
	    : pair_(ptr, del) {}
	~UniquePtr() { pair_.invokeSecond(pair_.first); }

	UniquePtr(UniquePtr &&other);
	template <class U, class Deleter2> UniquePtr(UniquePtr<U, Deleter2> &&other);
	UniquePtr &operator=(UniquePtr &&other);
	template <class U, class Deleter2> UniquePtr &operator=(UniquePtr<U, Deleter2> &&other);

	T *release();
	void reset(T *newPtr);
	void reset(nullptr_t);
	inline T *get() { return pair_.first; }
	inline const T *get() const { return pair_.first; }

	inline T &operator*() { return *pair_.first; }
	inline const T &operator*() const { return *pair_.first; }

	inline T *operator->() { return pair_.first; }
	inline const T *operator->() const { return pair_.first; }

	inline operator bool() const { return pair_.first != nullptr; }

	friend inline bool operator==(const UniquePtr &x, const UniquePtr &y) { return x.get() == y.get(); }
	friend inline bool operator!=(const UniquePtr &x, const UniquePtr &y) { return x.get() != y.get(); }
	friend inline bool operator==(const UniquePtr &x, nullptr_t) { return x.get() == nullptr; }
	friend inline bool operator==(nullptr_t, const UniquePtr &y) { return nullptr == y.get(); }
	friend inline bool operator!=(const UniquePtr &x, nullptr_t) { return x.get() != nullptr; }
	friend inline bool operator!=(nullptr_t, const UniquePtr &y) { return nullptr != y.get(); }

  private:
	detail::Pair<T *, Deleter> pair_;

	/// Deleted copy constructor
	UniquePtr(const UniquePtr &) = delete;
	/// Deleted assignment operator
	UniquePtr &operator=(const UniquePtr &) = delete;
};

template <class T, class Deleter>
UniquePtr<T, Deleter>::UniquePtr(UniquePtr &&other)
    : pair_(other.pair_)
{
	other.pair_.first = nullptr;
}

template <class T, class Deleter>
template <class U, class Deleter2>
UniquePtr<T, Deleter>::UniquePtr(UniquePtr<U, Deleter2> &&other)
    : pair_(reinterpret_cast<UniquePtr<T, Deleter> &>(other).pair_)
{
	reinterpret_cast<UniquePtr<T, Deleter> &>(other).pair_.first = nullptr;
}

template <class T, class Deleter>
UniquePtr<T, Deleter> &UniquePtr<T, Deleter>::operator=(UniquePtr &&other)
{
	reset(other.release());
	pair_.swapSecond(other.pair_);
	return *this;
}

template <class T, class Deleter>
template <class U, class Deleter2>
UniquePtr<T, Deleter> &UniquePtr<T, Deleter>::operator=(UniquePtr<U, Deleter2> &&other)
{
	reset(other.release());
	pair_.swapSecond(reinterpret_cast<UniquePtr<T, Deleter> &>(other).pair_);
	return *this;
}

template <class T, class Deleter>
T *UniquePtr<T, Deleter>::release()
{
	T *oldPtr = pair_.first;
	pair_.first = nullptr;
	return oldPtr;
}

template <class T, class Deleter>
void UniquePtr<T, Deleter>::reset(T *newPtr)
{
	// check for self reset
	if (pair_.first != newPtr)
	{
		T *oldPtr = pair_.first;
		pair_.first = newPtr;
		pair_.invokeSecond(oldPtr);
	}
}

template <class T, class Deleter>
void UniquePtr<T, Deleter>::reset(nullptr_t)
{
	pair_.invokeSecond(pair_.first);
	pair_.first = nullptr;
}

/// Array specialization of the unique pointer implementation
template <class T, class Deleter>
class UniquePtr<T[], Deleter>
{
  public:
	UniquePtr()
	    : pair_(nullptr) {}
	explicit UniquePtr(T *ptr)
	    : pair_(ptr) {}
	UniquePtr(T *ptr, Deleter del)
	    : pair_(ptr, del) {}
	~UniquePtr() { pair_.invokeSecond(pair_.first); }

	UniquePtr(UniquePtr &&other);
	UniquePtr &operator=(UniquePtr &&other);

	T *release();
	void reset(T *newPtr);
	void reset(nullptr_t);
	inline T *get() { return pair_.first; }
	inline const T *get() const { return pair_.first; }

	inline const T &operator[](size_t index) const { return pair_.first[index]; }
	inline T &operator[](size_t index) { return pair_.first[index]; }

	inline operator bool() const { return pair_.first != nullptr; }

	friend inline bool operator==(const UniquePtr &x, const UniquePtr &y) { return x.get() == y.get(); }
	friend inline bool operator!=(const UniquePtr &x, const UniquePtr &y) { return x.get() != y.get(); }
	friend inline bool operator==(const UniquePtr &x, nullptr_t) { return x.get() == nullptr; }
	friend inline bool operator==(nullptr_t, const UniquePtr &y) { return nullptr == y.get(); }
	friend inline bool operator!=(const UniquePtr &x, nullptr_t) { return x.get() != nullptr; }
	friend inline bool operator!=(nullptr_t, const UniquePtr &y) { return nullptr != y.get(); }

  private:
	detail::Pair<T *, Deleter> pair_;

	/// Deleted copy constructor
	UniquePtr(const UniquePtr &) = delete;
	/// Deleted assignment operator
	UniquePtr &operator=(const UniquePtr &) = delete;
};

template <class T, class Deleter>
UniquePtr<T[], Deleter>::UniquePtr(UniquePtr &&other)
    : pair_(other.pair_)
{
	other.pair_.first = nullptr;
}

template <class T, class Deleter>
UniquePtr<T[], Deleter> &UniquePtr<T[], Deleter>::operator=(UniquePtr<T[], Deleter> &&other)
{
	reset(other.release());
	pair_.swapSecond(other.pair_);
	return *this;
}

template <class T, class Deleter>
T *UniquePtr<T[], Deleter>::release()
{
	T *oldPtr = pair_.first;
	pair_.first = nullptr;
	return oldPtr;
}

template <class T, class Deleter>
void UniquePtr<T[], Deleter>::reset(T *newPtr)
{
	// check for self reset
	if (pair_.first != newPtr)
	{
		T *oldPtr = pair_.first;
		pair_.first = newPtr;
		pair_.invokeSecond(oldPtr);
	}
}

template <class T, class Deleter>
void UniquePtr<T[], Deleter>::reset(nullptr_t)
{
	pair_.invokeSecond(pair_.first);
	pair_.first = nullptr;
}

template <class T, class Deleter = DefaultDelete<T>>
struct MakeUniqueReturn
{
	typedef UniquePtr<T, Deleter> singleObject;
};

template <class T, class Deleter>
struct MakeUniqueReturn<T[], Deleter>
{
	typedef UniquePtr<T[], Deleter> array;
};

template <class T, class Deleter, unsigned long int size>
struct MakeUniqueReturn<T[size], Deleter>
{
	struct invalid
	{};
};

#if !NCINE_WITH_ALLOCATORS
// `makeUnique()` for single objects
template <class T, class Deleter = DefaultDelete<T>, typename... Args>
typename MakeUniqueReturn<T, Deleter>::singleObject makeUnique(Args &&... args)
{
	return UniquePtr<T, Deleter>(new T(nctl::forward<Args>(args)...));
}

/// `makeUnique()` for arrays of unknown bound
template <class T, class Deleter = DefaultDelete<T>>
typename MakeUniqueReturn<T, Deleter>::array makeUnique(unsigned long int size)
{
	return UniquePtr<T, Deleter>(new nctl::removeExtentT<T>[size]());
}
#else
// `makeUnique()` for single objects
template <class T, class Deleter = DefaultDelete<T>, typename... Args>
typename MakeUniqueReturn<T, Deleter>::singleObject makeUnique(Args &&... args)
{
	return UniquePtr<T, Deleter>(theDefaultAllocator().newObject<T>(nctl::forward<Args>(args)...));
}

/// `makeUnique()` for arrays of unknown bound
template <class T, class Deleter = DefaultDelete<T>>
typename MakeUniqueReturn<T, Deleter>::array makeUnique(unsigned long int size)
{
	return UniquePtr<T, Deleter>(theDefaultAllocator().newArray<nctl::removeExtentT<T>>(size));
}
#endif

/// Disable `makeUnique()` for arrays of known bound
template <class T, class Deleter, typename... Args>
typename MakeUniqueReturn<T, Deleter>::invalid makeUnique(Args &&...) = delete;

#if NCINE_WITH_ALLOCATORS
/// `allocateUnique()` for single objects
template <class T, typename... Args>
typename MakeUniqueReturn<T, AllocDelete<T>>::singleObject allocateUnique(IAllocator &allocator, Args &&... args)
{
	AllocDelete<T> deleter(&allocator);
	return UniquePtr<T, AllocDelete<T>>(allocator.newObject<T>(nctl::forward<Args>(args)...), deleter);
}

/// `allocateUnique()` for arrays of unknown bound
template <class T>
typename MakeUniqueReturn<T, AllocDelete<T>>::array allocateUnique(IAllocator &allocator, unsigned long int size)
{
	AllocDelete<T> deleter(&allocator);
	return UniquePtr<T, AllocDelete<T>>(allocator.newArray<nctl::removeExtentT<T>>(size), deleter);
}

/// Disable `allocateUnique()` for arrays of known bound
template <class T, class Deleter, typename... Args>
typename MakeUniqueReturn<T, Deleter>::invalid allocateUnique(Args &&...) = delete;
#endif

}

#endif
