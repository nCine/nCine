#ifndef CLASS_NCINE_Handle
#define CLASS_NCINE_Handle

#include <cstdint>

namespace ncine {
namespace grail {

template <typename Tag>
struct HandleHash;

template <typename Tag>
class Handle
{
  public:
	Handle()
	    : value_(InvalidValue) {}

	Handle(uint32_t index, uint32_t generation)
	    : value_((generation << IndexBits) | (index & IndexMask)) {}

	static Handle Invalid() { return Handle(); }

	uint32_t index() const
	{
		return value_ & IndexMask;
	}

	uint32_t generation() const
	{
		return (value_ >> IndexBits) & GenMask;
	}

	bool isValid() const { return value_ != InvalidValue; }

	bool operator==(const Handle &other) const { return value_ == other.value_; }
	bool operator!=(const Handle &other) const { return value_ != other.value_; }

  private:
	static constexpr uint32_t InvalidValue = 0;

	static constexpr uint32_t IndexBits = 18;
	static constexpr uint32_t GenBits = 14;
	static_assert(IndexBits + GenBits == sizeof(uint32_t) * 8, "The handle should be 32bit long");

	static constexpr uint32_t IndexMask = (1u << IndexBits) - 1u;
	static constexpr uint32_t GenMask = (1u << GenBits) - 1u;

	uint32_t value_ = InvalidValue;

	friend struct HandleHash<Tag>;
};

template <typename Tag>
struct HandleHash
{
	uint32_t operator()(const Handle<Tag> &handle) const
	{
		return static_cast<uint32_t>(handle.value_);
	}
};

} // namespace grail
} // namespace ncine

#endif
