#ifndef NCINE_SHAREDPTR
#define NCINE_SHAREDPTR

#include "UniquePtr.h"
#include "Atomic.h"

namespace nctl {

/// A shared pointer implementation
/*! \note It lacks the aliasing constructor and pointer reset when using `makeShared()` */
template <class T>
class SharedPtr
{
  public:
	SharedPtr() : ptr_(nullptr), ctrlBlock_(nullptr) { }
	explicit SharedPtr(T *ptr);
	explicit SharedPtr(UniquePtr<T> &&unique);
	~SharedPtr();

	SharedPtr(const SharedPtr &other);
	SharedPtr &operator=(SharedPtr other);
	SharedPtr(SharedPtr &&other);

	void reset(T *newPtr);
	void reset(nullptr_t);
	inline T *get() { return ptr_; }
	inline const T *get() const { return ptr_; }
	int32_t useCount() const;

	inline T &operator*() { return *ptr_; }
	inline const T &operator*() const { return *ptr_; }

	inline T *operator->() { return ptr_; }
	inline const T *operator->() const { return ptr_; }

	inline operator bool() const { return ptr_ != nullptr; }

	friend inline bool operator==(const SharedPtr &x, const SharedPtr &y) { return x.get() == y.get(); }
	friend inline bool operator!=(const SharedPtr &x, const SharedPtr &y) { return x.get() != y.get(); }
	friend inline bool operator==(const SharedPtr &x, nullptr_t) { return x.get() == nullptr; }
	friend inline bool operator==(nullptr_t, const SharedPtr &y) { return nullptr == y.get(); }
	friend inline bool operator!=(const SharedPtr &x, nullptr_t) { return x.get() != nullptr; }
	friend inline bool operator!=(nullptr_t, const SharedPtr &y) { return nullptr != y.get(); }

  private:
	/// The shared pointer control block with an atomic counter
	struct ControlBlock
	{
		ControlBlock() : counter_(0) { }
		ControlBlock(int32_t count) : counter_(count) { }
		virtual ~ControlBlock() { }
		virtual void dispose() = 0;
		virtual void reset(T *ptr) = 0;
		virtual void reset(nullptr_t) = 0;

		Atomic32 counter_;
	};

	/// A control block that also stores a pointer to the object
	struct ControlBlockAndPointer : public ControlBlock
	{
		ControlBlockAndPointer() : ptr_(nullptr) { }
		ControlBlockAndPointer(T *ptr) : ControlBlock(1), ptr_(ptr) { FATAL_ASSERT(ptr); }
		~ControlBlockAndPointer() override { dispose(); }
		void dispose() override { delete ptr_; }
		void reset(T *ptr) override { ptr_ = ptr; }
		void reset(nullptr_t) override { ptr_ = nullptr; }

		T *ptr_;
	};

	/// A control block that also stores the pointed object
	struct ControlBlockAndObject : public ControlBlock
	{
		template<class ... Args>
		ControlBlockAndObject(Args&& ...args) : ControlBlock(1), object_(nctl::forward<Args>(args)...) { }
		void dispose() override { }
		void reset(T *) override { FATAL_MSG("Can't reset a control block that stores the object"); }
		void reset(nullptr_t) override { FATAL_MSG("Can't reset a control block that stores the object"); }

		T object_;
	};

	T *ptr_;
	ControlBlock *ctrlBlock_;

	template<class S, class ... Args> friend SharedPtr<S> makeShared(Args&& ... args);
};

template <class T>
SharedPtr<T>::SharedPtr(T *ptr)
	: ptr_(ptr), ctrlBlock_(new ControlBlockAndPointer(ptr_))
{

}

template <class T>
SharedPtr<T>::SharedPtr(UniquePtr<T> &&unique)
	: ptr_(unique.release()), ctrlBlock_(new ControlBlockAndPointer(ptr_))
{

}

template <class T>
SharedPtr<T>::~SharedPtr()
{
	if (ctrlBlock_)
	{
		ctrlBlock_->counter_--;
		if (ctrlBlock_->counter_ <= 0)
			delete ctrlBlock_;
	}
}

template <class T>
SharedPtr<T>::SharedPtr(const SharedPtr &other)
	: ptr_(other.ptr_), ctrlBlock_(other.ctrlBlock_)
{
		ctrlBlock_->counter_++;
}

template <class T>
SharedPtr<T> &SharedPtr<T>::operator=(SharedPtr other)
{
	nctl::swap(ptr_, other.ptr_);
	nctl::swap(ctrlBlock_, other.ctrlBlock_);
	return *this;
}

template <class T>
SharedPtr<T>::SharedPtr(SharedPtr &&other)
	: ptr_(nullptr), ctrlBlock_(nullptr)
{
	nctl::swap(ptr_, other.ptr_);
	nctl::swap(ctrlBlock_, other.ctrlBlock_);
}

template <class T>
int32_t SharedPtr<T>::useCount() const
{
	return (ctrlBlock_ ? ctrlBlock_->counter_ : 0);
}

template <class T>
void SharedPtr<T>::reset(T *newPtr)
{
	// check for self reset
	if (ptr_ != newPtr)
	{
		ctrlBlock_->counter_--;
		if (ctrlBlock_->counter_ <= 0)
			ctrlBlock_->dispose();

		ptr_ = newPtr;
		ctrlBlock_->reset(newPtr);
		ctrlBlock_->counter_ = 1;
	}
}

template <class T>
void SharedPtr<T>::reset(nullptr_t)
{
	ctrlBlock_->counter_--;
	if (ctrlBlock_->counter_ <= 0)
		ctrlBlock_->dispose();

	ptr_ = nullptr;
	ctrlBlock_->reset(nullptr);
	ctrlBlock_->counter_ = 0;
}

template<class T, class ... Args>
SharedPtr<T> makeShared(Args&& ... args)
{
	SharedPtr<T> ptr;
	auto ctrlBlock = new typename SharedPtr<T>::ControlBlockAndObject(args...);
	ptr.ptr_ = &ctrlBlock->object_;
	ptr.ctrlBlock_ = ctrlBlock;

	return ptr;
}

}

#endif
