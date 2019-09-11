#ifndef NCTL_ATOMIC
#define NCTL_ATOMIC

#include <cstdint>
#include <ncine/common_macros.h>

#if defined(_WIN32) && !defined(__MINGW32__)
	#include <ncine/common_windefines.h>
	#include <windef.h>
#elif defined(__APPLE__)
	#include <atomic>
#endif

namespace nctl {

/// An atomic `int32_t` class
class Atomic32
{
  public:
	enum class MemoryModel
	{
		RELAXED,
		ACQUIRE,
		RELEASE,
		SEQ_CST
	};

	Atomic32()
	    : value_(0) {}
	explicit Atomic32(int32_t value)
	    : value_(value) {}
	~Atomic32() = default;

	int32_t load(MemoryModel memModel);
	inline int32_t load() { return load(MemoryModel::SEQ_CST); }
	void store(int32_t value, MemoryModel memModel);
	inline void store(int32_t value) { store(value, MemoryModel::SEQ_CST); }

	int32_t fetchAdd(int32_t value, MemoryModel memModel);
	inline int32_t fetchAdd(int32_t value) { return fetchAdd(value, MemoryModel::SEQ_CST); }
	int32_t fetchSub(int32_t value, MemoryModel memModel);
	inline int32_t fetchSub(int32_t value) { return fetchSub(value, MemoryModel::SEQ_CST); }
	bool cmpExchange(int32_t newValue, int32_t cmpValue, MemoryModel memModel);
	inline bool cmpExchange(int32_t newValue, int32_t cmpValue) { return cmpExchange(newValue, cmpValue, MemoryModel::SEQ_CST); }

	int32_t operator=(int32_t value)
	{
		store(value);
		return value_;
	}

	operator int32_t() { return load(); }

	inline int32_t operator++() { return fetchAdd(1) + 1; }
	inline int32_t operator++(int) { return fetchAdd(1); }
	inline int32_t operator--() { return fetchSub(1) - 1; }
	inline int32_t operator--(int) { return fetchSub(1); }

  private:
#ifdef __APPLE__
	std::atomic<int32_t> value_;
#else
	int32_t value_;
#endif

	/// Deleted copy constructor
	Atomic32(const Atomic32 &) = delete;
	/// Deleted move constructor
	Atomic32(Atomic32 &&) = delete;
	/// Deleted assignment operator
	Atomic32 &operator=(const Atomic32 &) = delete;
};

/// An atomic `int64_t` class
class Atomic64
{
  public:
	enum class MemoryModel
	{
		RELAXED,
		ACQUIRE,
		RELEASE,
		SEQ_CST
	};

	Atomic64()
	    : value_(0) {}
	explicit Atomic64(int64_t value)
	    : value_(value) {}
	~Atomic64() = default;

	int64_t load(MemoryModel memModel);
	inline int64_t load() { return load(MemoryModel::SEQ_CST); }
	void store(int64_t value, MemoryModel memModel);
	inline void store(int64_t value) { store(value, MemoryModel::SEQ_CST); }

	int64_t fetchAdd(int64_t value, MemoryModel memModel);
	inline int64_t fetchAdd(int64_t value) { return fetchAdd(value, MemoryModel::SEQ_CST); }
	int64_t fetchSub(int64_t value, MemoryModel memModel);
	inline int64_t fetchSub(int64_t value) { return fetchSub(value, MemoryModel::SEQ_CST); }
	bool cmpExchange(int64_t newValue, int64_t cmpValue, MemoryModel memModel);
	inline bool cmpExchange(int64_t newValue, int64_t cmpValue) { return cmpExchange(newValue, cmpValue, MemoryModel::SEQ_CST); }

	inline int64_t operator=(int64_t value)
	{
		store(value);
		return value_;
	}

	operator int64_t() { return load(); }

	inline int64_t operator++() { return fetchAdd(1) + 1; }
	inline int64_t operator++(int) { return fetchAdd(1); }
	inline int64_t operator--() { return fetchSub(1) - 1; }
	inline int64_t operator--(int) { return fetchSub(1); }

  private:
#ifdef __APPLE__
	std::atomic<int64_t> value_;
#else
	int64_t value_;
#endif

	/// Deleted copy constructor
	Atomic64(const Atomic64 &) = delete;
	/// Deleted move constructor
	Atomic64(Atomic64 &&) = delete;
	/// Deleted assignment operator
	Atomic64 &operator=(const Atomic64 &) = delete;
};

#if defined(_WIN32) && !defined(__MINGW32__)
	#include "WindowsAtomic.inl"
#elif defined(__APPLE__)
	#include "StdAtomic.inl"
#else
	#include "GccAtomic.inl"
#endif

}

#endif
