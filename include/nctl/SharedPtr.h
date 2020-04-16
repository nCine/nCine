#ifndef CLASS_NCTL_SHAREDPTR
#define CLASS_NCTL_SHAREDPTR

#include "UniquePtr.h"
#include "Atomic.h"

namespace nctl {

/// A shared pointer implementation
/*! \note It lacks the aliasing constructor and pointer reset when using `makeShared()` */
template <class T>
class SharedPtr
{
  public:
	SharedPtr()
	    : ptr_(nullptr), ctrlBlock_(nullptr) {}
	explicit SharedPtr(T *ptr);
#if NCINE_WITH_ALLOCATORS
	SharedPtr(T *ptr, IAllocator &alloc);
#endif
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
#if !NCINE_WITH_ALLOCATORS
		ControlBlock()
		    : counter_(0) {}
		ControlBlock(int32_t count)
		    : counter_(count) {}
#else
		explicit ControlBlock(IAllocator &alloc)
		    : alloc_(alloc), counter_(0) {}
		ControlBlock(IAllocator &alloc, int32_t count)
		    : alloc_(alloc), counter_(count) {}
#endif
		virtual ~ControlBlock() {}
		virtual void dispose() = 0;
		virtual void reset(T *ptr) = 0;
		virtual void reset(nullptr_t) = 0;

#if NCINE_WITH_ALLOCATORS
		/// The custom memory allocator for the shared pointer
		IAllocator &alloc_;
#endif
		Atomic32 counter_;
	};

	/// A control block that also stores a pointer to the object
	struct ControlBlockAndPointer : public ControlBlock
	{
#if !NCINE_WITH_ALLOCATORS
		ControlBlockAndPointer()
		    : ptr_(nullptr) {}
		explicit ControlBlockAndPointer(T *ptr)
		    : ControlBlock(1), ptr_(ptr) { FATAL_ASSERT(ptr); }
#else
		ControlBlockAndPointer()
		    : ControlBlock(theDefaultAllocator()), ptr_(nullptr) {}
		explicit ControlBlockAndPointer(T *ptr)
		    : ControlBlock(theDefaultAllocator(), 1), ptr_(ptr) { FATAL_ASSERT(ptr); }
		explicit ControlBlockAndPointer(IAllocator &alloc)
		    : ControlBlock(alloc, 1), ptr_(nullptr) {}
		ControlBlockAndPointer(IAllocator &alloc, T *ptr)
		    : ControlBlock(alloc, 1), ptr_(ptr) { FATAL_ASSERT(ptr); }
#endif
		~ControlBlockAndPointer() override { dispose(); }

		void dispose() override
		{
#if !NCINE_WITH_ALLOCATORS
			delete ptr_;
#else
			ControlBlock::alloc_.deleteObject(ptr_);
#endif
		}
		void reset(T *ptr) override { ptr_ = ptr; }
		void reset(nullptr_t) override { ptr_ = nullptr; }

		T *ptr_;
	};

	/// A control block that also stores the pointed object
	struct ControlBlockAndObject : public ControlBlock
	{
#if !NCINE_WITH_ALLOCATORS
		template <class... Args>
		ControlBlockAndObject(Args &&... args)
		    : ControlBlock(1), object_(nctl::forward<Args>(args)...) {}
#else
		template <class... Args>
		ControlBlockAndObject(Args &&... args)
		    : ControlBlock(theDefaultAllocator(), 1), object_(nctl::forward<Args>(args)...) {}
		template <class... Args>
		ControlBlockAndObject(IAllocator &alloc, Args &&... args)
		    : ControlBlock(alloc, 1), object_(nctl::forward<Args>(args)...) {}
#endif

		void dispose() override {}
		void reset(T *) override { FATAL_MSG("Can't reset a control block that stores the object"); }
		void reset(nullptr_t) override { FATAL_MSG("Can't reset a control block that stores the object"); }

		T object_;
	};

	T *ptr_;
	ControlBlock *ctrlBlock_;

	template <class S, class... Args>
	friend SharedPtr<S> makeShared(Args &&... args);
#if NCINE_WITH_ALLOCATORS
	template <class S, class... Args>
	friend SharedPtr<S> allocateShared(IAllocator &alloc, Args &&... args);
#endif
};

template <class T>
SharedPtr<T>::SharedPtr(T *ptr)
    : ptr_(ptr),
#if !NCINE_WITH_ALLOCATORS
      ctrlBlock_(new ControlBlockAndPointer(ptr_))
#else
      ctrlBlock_(theDefaultAllocator().newObject<ControlBlockAndPointer>(theDefaultAllocator(), ptr_))
#endif
{
}

#if NCINE_WITH_ALLOCATORS
template <class T>
SharedPtr<T>::SharedPtr(T *ptr, IAllocator &alloc)
    : ptr_(ptr), ctrlBlock_(alloc.newObject<ControlBlockAndPointer>(alloc, ptr_))
{
}
#endif

template <class T>
SharedPtr<T>::SharedPtr(UniquePtr<T> &&unique)
    : ptr_(unique.release()),
#if !NCINE_WITH_ALLOCATORS
      ctrlBlock_(new ControlBlockAndPointer(ptr_))
#else
      ctrlBlock_(theDefaultAllocator().newObject<ControlBlockAndPointer>(theDefaultAllocator(), ptr_))
#endif
{
}

template <class T>
SharedPtr<T>::~SharedPtr()
{
	if (ctrlBlock_)
	{
		ctrlBlock_->counter_--;
		if (ctrlBlock_->counter_ <= 0)
#if !NCINE_WITH_ALLOCATORS
			delete ctrlBlock_;
#else
			ctrlBlock_->alloc_.deleteObject(ctrlBlock_);
#endif
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

template <class T, class... Args>
SharedPtr<T> makeShared(Args &&... args)
{
	SharedPtr<T> ptr;
#if !NCINE_WITH_ALLOCATORS
	auto ctrlBlock = new typename SharedPtr<T>::ControlBlockAndObject(args...);
#else
	auto ctrlBlock = theDefaultAllocator().newObject<typename SharedPtr<T>::ControlBlockAndObject>(theDefaultAllocator(), args...);
#endif
	ptr.ptr_ = &ctrlBlock->object_;
	ptr.ctrlBlock_ = ctrlBlock;

	return ptr;
}

#if NCINE_WITH_ALLOCATORS
template <class T, class... Args>
SharedPtr<T> allocateShared(IAllocator &alloc, Args &&... args)
{
	SharedPtr<T> ptr;
	auto ctrlBlock = alloc.newObject<typename SharedPtr<T>::ControlBlockAndObject>(alloc, args...);
	ptr.ptr_ = &ctrlBlock->object_;
	ptr.ctrlBlock_ = ctrlBlock;

	return ptr;
}
#endif

}

#endif
