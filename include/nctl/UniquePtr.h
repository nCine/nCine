#ifndef NCINE_UNIQUEPTR
#define NCINE_UNIQUEPTR

#include "utility.h"

using nullptr_t = decltype(nullptr);

namespace nctl {

/// A unique pointer implementation
template <class T>
class UniquePtr
{
  public:
	UniquePtr()
	    : ptr_(nullptr) {}
	explicit UniquePtr(T *ptr)
	    : ptr_(ptr) {}
	template <class U>
	explicit UniquePtr(U *ptr)
	    : ptr_(ptr) {}
	~UniquePtr() { delete ptr_; }

	UniquePtr(UniquePtr &&other);
	template <class U> UniquePtr(UniquePtr<U> &&other);
	UniquePtr &operator=(UniquePtr &&other);
	template <class U> UniquePtr &operator=(UniquePtr<U> &&other);

	T *release();
	void reset(T *newPtr);
	void reset(nullptr_t);
	inline T *get() { return ptr_; }
	inline const T *get() const { return ptr_; }

	inline T &operator*() { return *ptr_; }
	inline const T &operator*() const { return *ptr_; }

	inline T *operator->() { return ptr_; }
	inline const T *operator->() const { return ptr_; }

	inline operator bool() const { return ptr_ != nullptr; }

	friend inline bool operator==(const UniquePtr &x, const UniquePtr &y) { return x.get() == y.get(); }
	friend inline bool operator!=(const UniquePtr &x, const UniquePtr &y) { return x.get() != y.get(); }
	friend inline bool operator==(const UniquePtr &x, nullptr_t) { return x.get() == nullptr; }
	friend inline bool operator==(nullptr_t, const UniquePtr &y) { return nullptr == y.get(); }
	friend inline bool operator!=(const UniquePtr &x, nullptr_t) { return x.get() != nullptr; }
	friend inline bool operator!=(nullptr_t, const UniquePtr &y) { return nullptr != y.get(); }

  private:
	T *ptr_;

	/// Deleted copy constructor
	UniquePtr(const UniquePtr &) = delete;
	/// Deleted assignment operator
	UniquePtr &operator=(const UniquePtr &) = delete;
};

template <class T>
UniquePtr<T>::UniquePtr(UniquePtr &&other)
    : ptr_(other.ptr_)
{
	other.ptr_ = nullptr;
}

template <class T>
template <class U>
UniquePtr<T>::UniquePtr(UniquePtr<U> &&other)
    : ptr_(other.release())
{
}

template <class T>
UniquePtr<T> &UniquePtr<T>::operator=(UniquePtr &&other)
{
	nctl::swap(ptr_, other.ptr_);
	return *this;
}

template <class T>
template <class U>
UniquePtr<T> &UniquePtr<T>::operator=(UniquePtr<U> &&other)
{
	reset(other.release());

	return *this;
}

template <class T>
T *UniquePtr<T>::release()
{
	T *oldPtr = ptr_;
	ptr_ = nullptr;
	return oldPtr;
}

template <class T>
void UniquePtr<T>::reset(T *newPtr)
{
	// check for self reset
	if (ptr_ != newPtr)
	{
		T *oldPtr = ptr_;
		ptr_ = newPtr;
		delete oldPtr;
	}
}

template <class T>
void UniquePtr<T>::reset(nullptr_t)
{
	delete ptr_;
	ptr_ = nullptr;
}

/// Array specialization of the unique pointer implementation
template <class T>
class UniquePtr<T[]>
{
  public:
	UniquePtr()
	    : ptr_(nullptr) {}
	explicit UniquePtr(T *ptr)
	    : ptr_(ptr) {}
	~UniquePtr() { delete[] ptr_; }

	UniquePtr(UniquePtr &&other);
	UniquePtr &operator=(UniquePtr &&other);

	T *release();
	void reset(T *newPtr);
	void reset(nullptr_t);
	inline T *get() { return ptr_; }
	inline const T *get() const { return ptr_; }

	inline const T &operator[](unsigned int index) const { return ptr_[index]; }
	inline T &operator[](unsigned int index) { return ptr_[index]; }

	inline operator bool() const { return ptr_ != nullptr; }

	friend inline bool operator==(const UniquePtr &x, const UniquePtr &y) { return x.get() == y.get(); }
	friend inline bool operator!=(const UniquePtr &x, const UniquePtr &y) { return x.get() != y.get(); }
	friend inline bool operator==(const UniquePtr &x, nullptr_t) { return x.get() == nullptr; }
	friend inline bool operator==(nullptr_t, const UniquePtr &y) { return nullptr == y.get(); }
	friend inline bool operator!=(const UniquePtr &x, nullptr_t) { return x.get() != nullptr; }
	friend inline bool operator!=(nullptr_t, const UniquePtr &y) { return nullptr != y.get(); }

  private:
	T *ptr_;

	/// Deleted copy constructor
	UniquePtr(const UniquePtr &) = delete;
	/// Deleted assignment operator
	UniquePtr &operator=(const UniquePtr &) = delete;
};

template <class T>
UniquePtr<T[]>::UniquePtr(UniquePtr &&other)
    : ptr_(other.ptr_)
{
	other.ptr_ = nullptr;
}

template <class T>
UniquePtr<T[]> &UniquePtr<T[]>::operator=(UniquePtr<T[]> &&other)
{
	nctl::swap(ptr_, other.ptr_);
	return *this;
}

template <class T>
T *UniquePtr<T[]>::release()
{
	T *oldPtr = ptr_;
	ptr_ = nullptr;
	return oldPtr;
}

template <class T>
void UniquePtr<T[]>::reset(T *newPtr)
{
	// check for self reset
	if (ptr_ != newPtr)
	{
		T *oldPtr = ptr_;
		ptr_ = newPtr;
		delete[] oldPtr;
	}
}

template <class T>
void UniquePtr<T[]>::reset(nullptr_t)
{
	delete[] ptr_;
	ptr_ = nullptr;
}

template <class T>
struct MakeUniqueReturn
{
	typedef UniquePtr<T> singleObject;
};

template <class T>
struct MakeUniqueReturn<T[]>
{
	typedef UniquePtr<T[]> array;
};

template <class T, unsigned long int size>
struct MakeUniqueReturn<T[size]>
{
	struct invalid
	{};
};

/// `makeUnique()` for single objects
template <class T, typename... Args>
typename MakeUniqueReturn<T>::singleObject makeUnique(Args &&... args)
{
	return UniquePtr<T>(new T(nctl::forward<Args>(args)...));
}

/// `makeUnique()` for arrays of unknown bound
template <class T>
typename MakeUniqueReturn<T>::array makeUnique(unsigned long int size)
{
	return UniquePtr<T>(new nctl::removeExtentT<T>[size]());
}

/// Disable `makeUnique()` for arrays of known bound
template <class T, typename... Args>
typename MakeUniqueReturn<T>::invalid makeUnique(Args &&...) = delete;

}

#endif
