#ifndef NCTL_ATOMIC
#define NCTL_ATOMIC

#include <cstdint>
#include <ncine/common_defines.h>

#if defined(__APPLE__)
	#include <atomic>
#endif

namespace nctl {

enum class MemoryModel
{
	RELAXED,
	ACQUIRE,
	RELEASE,
	ACQ_REL,
	SEQ_CST
};

/// Atomic memory and compiler fences
namespace AtomicFences
{
	DLL_PUBLIC void threadFence(MemoryModel memModel);
	DLL_PUBLIC inline void threadFence() { threadFence(MemoryModel::SEQ_CST); }

	DLL_PUBLIC void signalFence(MemoryModel memModel);
	DLL_PUBLIC inline void signalFence() { signalFence(MemoryModel::SEQ_CST); }
}

/// An atomic `int32_t` class
class DLL_PUBLIC Atomic32
{
  public:
	using Type = int32_t;

	Atomic32()
	    : value_(0) {}
	explicit Atomic32(int32_t value)
	    : value_(value) {}
	~Atomic32() = default;

	int32_t load(MemoryModel memModel) const;
	inline int32_t load() const { return load(MemoryModel::SEQ_CST); }
	void store(int32_t value, MemoryModel memModel);
	inline void store(int32_t value) { store(value, MemoryModel::SEQ_CST); }

	int32_t fetchAdd(int32_t value, MemoryModel memModel);
	inline int32_t fetchAdd(int32_t value) { return fetchAdd(value, MemoryModel::SEQ_CST); }
	int32_t fetchSub(int32_t value, MemoryModel memModel);
	inline int32_t fetchSub(int32_t value) { return fetchSub(value, MemoryModel::SEQ_CST); }

	int32_t fetchAnd(int32_t value, MemoryModel memModel);
	inline int32_t fetchAnd(int32_t value) { return fetchAnd(value, MemoryModel::SEQ_CST); }
	int32_t fetchOr(int32_t value, MemoryModel memModel);
	inline int32_t fetchOr(int32_t value) { return fetchOr(value, MemoryModel::SEQ_CST); }
	int32_t fetchXor(int32_t value, MemoryModel memModel);
	inline int32_t fetchXor(int32_t value) { return fetchXor(value, MemoryModel::SEQ_CST); }

	bool cmpExchange(int32_t &cmpValue, int32_t newValue, MemoryModel memModel);
	inline bool cmpExchange(int32_t &cmpValue, int32_t newValue) { return cmpExchange(cmpValue, newValue, MemoryModel::SEQ_CST); }

	inline int32_t operator=(int32_t value)
	{
		store(value);
		return value;
	}

	operator int32_t() const { return load(); }

	inline int32_t operator++() { return fetchAdd(1) + 1; }
	inline int32_t operator++(int) { return fetchAdd(1); }
	inline int32_t operator--() { return fetchSub(1) - 1; }
	inline int32_t operator--(int) { return fetchSub(1); }

	inline int32_t operator+=(int32_t value) { return fetchAdd(value) + value; }
	inline int32_t operator-=(int32_t value) { return fetchSub(value) - value; }

	inline int32_t operator&=(int32_t value) { return fetchAnd(value); }
	inline int32_t operator|=(int32_t value) { return fetchOr(value); }
	inline int32_t operator^=(int32_t value) { return fetchXor(value); }

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

/// An atomic `uint32_t` class
class DLL_PUBLIC AtomicU32
{
  public:
	using Type = uint32_t;

	AtomicU32()
	    : value_(0) {}
	explicit AtomicU32(uint32_t value)
	    : value_(value) {}
	~AtomicU32() = default;

	uint32_t load(MemoryModel memModel) const;
	inline uint32_t load() const { return load(MemoryModel::SEQ_CST); }
	void store(uint32_t value, MemoryModel memModel);
	inline void store(uint32_t value) { store(value, MemoryModel::SEQ_CST); }

	uint32_t fetchAdd(uint32_t value, MemoryModel memModel);
	inline uint32_t fetchAdd(uint32_t value) { return fetchAdd(value, MemoryModel::SEQ_CST); }
	uint32_t fetchSub(uint32_t value, MemoryModel memModel);
	inline uint32_t fetchSub(uint32_t value) { return fetchSub(value, MemoryModel::SEQ_CST); }

	uint32_t fetchAnd(uint32_t value, MemoryModel memModel);
	inline uint32_t fetchAnd(uint32_t value) { return fetchAnd(value, MemoryModel::SEQ_CST); }
	uint32_t fetchOr(uint32_t value, MemoryModel memModel);
	inline uint32_t fetchOr(uint32_t value) { return fetchOr(value, MemoryModel::SEQ_CST); }
	uint32_t fetchXor(uint32_t value, MemoryModel memModel);
	inline uint32_t fetchXor(uint32_t value) { return fetchXor(value, MemoryModel::SEQ_CST); }

	bool cmpExchange(uint32_t &cmpValue, uint32_t newValue, MemoryModel memModel);
	inline bool cmpExchange(uint32_t &cmpValue, uint32_t newValue) { return cmpExchange(cmpValue, newValue, MemoryModel::SEQ_CST); }

	inline uint32_t operator=(uint32_t value)
	{
		store(value);
		return value;
	}

	operator uint32_t() const { return load(); }

	inline uint32_t operator++() { return fetchAdd(1) + 1; }
	inline uint32_t operator++(int) { return fetchAdd(1); }
	inline uint32_t operator--() { return fetchSub(1) - 1; }
	inline uint32_t operator--(int) { return fetchSub(1); }

	inline uint32_t operator+=(uint32_t value) { return fetchAdd(value) + value; }
	inline uint32_t operator-=(uint32_t value) { return fetchSub(value) - value; }

	inline uint32_t operator&=(uint32_t value) { return fetchAnd(value); }
	inline uint32_t operator|=(uint32_t value) { return fetchOr(value); }
	inline uint32_t operator^=(uint32_t value) { return fetchXor(value); }

  private:
#ifdef __APPLE__
	std::atomic<uint32_t> value_;
#else
	uint32_t value_;
#endif

	/// Deleted copy constructor
	AtomicU32(const AtomicU32 &) = delete;
	/// Deleted move constructor
	AtomicU32(AtomicU32 &&) = delete;
	/// Deleted assignment operator
	AtomicU32 &operator=(const AtomicU32 &) = delete;
};

/// An atomic `int64_t` class
class DLL_PUBLIC Atomic64
{
  public:
	using Type = int64_t;

	Atomic64()
	    : value_(0) {}
	explicit Atomic64(int64_t value)
	    : value_(value) {}
	~Atomic64() = default;

	int64_t load(MemoryModel memModel) const;
	inline int64_t load() const { return load(MemoryModel::SEQ_CST); }
	void store(int64_t value, MemoryModel memModel);
	inline void store(int64_t value) { store(value, MemoryModel::SEQ_CST); }

	int64_t fetchAdd(int64_t value, MemoryModel memModel);
	inline int64_t fetchAdd(int64_t value) { return fetchAdd(value, MemoryModel::SEQ_CST); }
	int64_t fetchSub(int64_t value, MemoryModel memModel);
	inline int64_t fetchSub(int64_t value) { return fetchSub(value, MemoryModel::SEQ_CST); }

	int64_t fetchAnd(int64_t value, MemoryModel memModel);
	inline int64_t fetchAnd(int64_t value) { return fetchAnd(value, MemoryModel::SEQ_CST); }
	int64_t fetchOr(int64_t value, MemoryModel memModel);
	inline int64_t fetchOr(int64_t value) { return fetchOr(value, MemoryModel::SEQ_CST); }
	int64_t fetchXor(int64_t value, MemoryModel memModel);
	inline int64_t fetchXor(int64_t value) { return fetchXor(value, MemoryModel::SEQ_CST); }

	bool cmpExchange(int64_t &cmpValue, int64_t newValue, MemoryModel memModel);
	inline bool cmpExchange(int64_t &cmpValue, int64_t newValue) { return cmpExchange(cmpValue, newValue, MemoryModel::SEQ_CST); }

	inline int64_t operator=(int64_t value)
	{
		store(value);
		return value;
	}

	operator int64_t() const { return load(); }

	inline int64_t operator++() { return fetchAdd(1) + 1; }
	inline int64_t operator++(int) { return fetchAdd(1); }
	inline int64_t operator--() { return fetchSub(1) - 1; }
	inline int64_t operator--(int) { return fetchSub(1); }

	inline int64_t operator+=(int64_t value) { return fetchAdd(value) + value; }
	inline int64_t operator-=(int64_t value) { return fetchSub(value) - value; }

	inline int64_t operator&=(int64_t value) { return fetchAnd(value); }
	inline int64_t operator|=(int64_t value) { return fetchOr(value); }
	inline int64_t operator^=(int64_t value) { return fetchXor(value); }

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

/// An atomic `uint64_t` class
class DLL_PUBLIC AtomicU64
{
  public:
	using Type = uint64_t;

	AtomicU64()
	    : value_(0) {}
	explicit AtomicU64(uint64_t value)
	    : value_(value) {}
	~AtomicU64() = default;

	uint64_t load(MemoryModel memModel) const;
	inline uint64_t load() const { return load(MemoryModel::SEQ_CST); }
	void store(uint64_t value, MemoryModel memModel);
	inline void store(uint64_t value) { store(value, MemoryModel::SEQ_CST); }

	uint64_t fetchAdd(uint64_t value, MemoryModel memModel);
	inline uint64_t fetchAdd(uint64_t value) { return fetchAdd(value, MemoryModel::SEQ_CST); }
	uint64_t fetchSub(uint64_t value, MemoryModel memModel);
	inline uint64_t fetchSub(uint64_t value) { return fetchSub(value, MemoryModel::SEQ_CST); }

	uint64_t fetchAnd(uint64_t value, MemoryModel memModel);
	inline uint64_t fetchAnd(uint64_t value) { return fetchAnd(value, MemoryModel::SEQ_CST); }
	uint64_t fetchOr(uint64_t value, MemoryModel memModel);
	inline uint64_t fetchOr(uint64_t value) { return fetchOr(value, MemoryModel::SEQ_CST); }
	uint64_t fetchXor(uint64_t value, MemoryModel memModel);
	inline uint64_t fetchXor(uint64_t value) { return fetchXor(value, MemoryModel::SEQ_CST); }

	bool cmpExchange(uint64_t &cmpValue, uint64_t newValue, MemoryModel memModel);
	inline bool cmpExchange(uint64_t &cmpValue, uint64_t newValue) { return cmpExchange(cmpValue, newValue, MemoryModel::SEQ_CST); }

	inline uint64_t operator=(uint64_t value)
	{
		store(value);
		return value;
	}

	operator uint64_t() const { return load(); }

	inline uint64_t operator++() { return fetchAdd(1) + 1; }
	inline uint64_t operator++(int) { return fetchAdd(1); }
	inline uint64_t operator--() { return fetchSub(1) - 1; }
	inline uint64_t operator--(int) { return fetchSub(1); }

	inline uint64_t operator+=(uint64_t value) { return fetchAdd(value) + value; }
	inline uint64_t operator-=(uint64_t value) { return fetchSub(value) - value; }

	inline uint64_t operator&=(uint64_t value) { return fetchAnd(value); }
	inline uint64_t operator|=(uint64_t value) { return fetchOr(value); }
	inline uint64_t operator^=(uint64_t value) { return fetchXor(value); }

  private:
#ifdef __APPLE__
	std::atomic<uint64_t> value_;
#else
	uint64_t value_;
#endif

	/// Deleted copy constructor
	AtomicU64(const AtomicU64 &) = delete;
	/// Deleted move constructor
	AtomicU64(AtomicU64 &&) = delete;
	/// Deleted assignment operator
	AtomicU64 &operator=(const AtomicU64 &) = delete;
};

}

#endif
