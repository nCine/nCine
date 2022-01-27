#ifndef CLASS_NCTL_BITSET
#define CLASS_NCTL_BITSET

#include <cstdint>
#include "type_traits.h"

namespace nctl {

/// A sequence of bits to be manipulated with logical operators
template <class T>
class BitSet
{
  public:
	BitSet();
	explicit BitSet(T value);

	bool operator==(const BitSet &other) const;
	bool operator!=(const BitSet &other) const;

	/// \returns True if the bit at the specified position is set
	bool test(unsigned int pos) const;

	/// \returns True if all bits are set
	bool all() const;
	/// \returns True if at least one bit is set
	bool any() const;
	/// \returns True if all bits are not set
	bool none() const;

	/// \returns The number of bits that are set
	unsigned int count() const;

	/// \returns The total number of bits in the bitset
	unsigned int size() const;

	BitSet &operator&=(const BitSet &other);
	BitSet &operator|=(const BitSet &other);
	BitSet &operator^=(const BitSet &other);

	BitSet operator~() const;

	BitSet &operator<<=(unsigned int pos);
	BitSet &operator>>=(unsigned int pos);

	BitSet operator<<(unsigned int pos) const;
	BitSet operator>>(unsigned int pos) const;

	/// Sets all bits in the bitset
	void set();
	/// Sets the bit at the specified position
	void set(unsigned int pos);
	/// Sets the bit at the specified position with the specified value
	void set(unsigned int pos, bool value);

	/// Resets all bits in the bitset
	void reset();
	/// Resets the bit at the specified position
	void reset(unsigned int pos);

	/// Flips the bit at the specified position
	void flip(unsigned int pos);

	friend BitSet operator&(const BitSet &lhs, const BitSet &rhs) { return BitSet(lhs.bits_ & rhs.bits_); }
	friend BitSet operator|(const BitSet &lhs, const BitSet &rhs) { return BitSet(lhs.bits_ | rhs.bits_); }
	friend BitSet operator^(const BitSet &lhs, const BitSet &rhs) { return BitSet(lhs.bits_ ^ rhs.bits_); }

  private:
	T bits_;
};

template <class T>
BitSet<T>::BitSet()
    : bits_(T(0))
{
	static_assert(nctl::isIntegral<T>::value, "Integral type is required");
	static_assert(T(0) < T(-1), "Unsigned type is required");
}

template <class T>
BitSet<T>::BitSet(T value)
    : bits_(value)
{
	static_assert(nctl::isIntegral<T>::value, "Integral type is required");
	static_assert(T(0) < T(-1), "Unsigned type is required");
}

template <class T>
inline bool BitSet<T>::operator==(const BitSet &other) const
{
	return other.bits_ == bits_;
}

template <class T>
inline bool BitSet<T>::operator!=(const BitSet &other) const
{
	return other.bits_ != bits_;
}

template <class T>
inline bool BitSet<T>::test(unsigned int pos) const
{
	return ((bits_ >> pos) & T(1)) != T(0);
}

template <class T>
inline bool BitSet<T>::all() const
{
	return ~bits_ == T(0);
}

template <class T>
inline bool BitSet<T>::any() const
{
	return bits_ != T(0);
}

template <class T>
inline bool BitSet<T>::none() const
{
	return bits_ == T(0);
}

template <>
inline unsigned int BitSet<uint8_t>::count() const
{
	static const uint8_t splitLookup[] = { 0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4 };
	return splitLookup[bits_ & 0xF] + splitLookup[bits_ >> 4];
}

template <>
inline unsigned int BitSet<uint16_t>::count() const
{
	return BitSet<uint8_t>(bits_ & 0xFF).count() + BitSet<uint8_t>(bits_ >> 8).count();
}

template <>
inline unsigned int BitSet<uint32_t>::count() const
{
	uint32_t bits = bits_;
	bits = bits - ((bits >> 1) & 0x55555555);
	bits = (bits & 0x33333333) + ((bits >> 2) & 0x33333333);
	return (((bits + (bits >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
}

template <>
inline unsigned int BitSet<uint64_t>::count() const
{
	uint64_t bits = bits_;
	bits = bits - ((bits >> 1) & 0x5555555555555555);
	bits = (bits & 0x3333333333333333) + ((bits >> 2) & 0x3333333333333333);
	return (((bits + (bits >> 4)) & 0x0F0F0F0F0F0F0F0F) * 0x0101010101010101) >> 56;
}

template <class T>
inline unsigned int BitSet<T>::size() const
{
	return sizeof(T) * 8;
}

template <class T>
inline BitSet<T> &BitSet<T>::operator&=(const BitSet &other)
{
	bits_ &= other.bits_;
	return *this;
}

template <class T>
inline BitSet<T> &BitSet<T>::operator|=(const BitSet &other)
{
	bits_ |= other.bits_;
	return *this;
}

template <class T>
inline BitSet<T> &BitSet<T>::operator^=(const BitSet &other)
{
	bits_ ^= other.bits_;
	return *this;
}

template <class T>
inline BitSet<T> BitSet<T>::operator~() const
{
	return BitSet(~bits_);
}

template <class T>
inline BitSet<T> &BitSet<T>::operator<<=(unsigned int pos)
{
	bits_ <<= pos;
	return *this;
}

template <class T>
inline BitSet<T> &BitSet<T>::operator>>=(unsigned int pos)
{
	bits_ >>= pos;
	return *this;
}

template <class T>
inline BitSet<T> BitSet<T>::operator<<(unsigned int pos) const
{
	return BitSet(bits_ << pos);
}

template <class T>
inline BitSet<T> BitSet<T>::operator>>(unsigned int pos) const
{
	return BitSet(bits_ >> pos);
}

template <class T>
inline void BitSet<T>::set()
{
	bits_ = ~T(0);
}

template <class T>
inline void BitSet<T>::set(unsigned int pos)
{
	bits_ |= T(1) << pos;
}

template <class T>
inline void BitSet<T>::set(unsigned int pos, bool value)
{
	value ? set(pos) : reset(pos);
}

template <class T>
inline void BitSet<T>::reset()
{
	bits_ = T(0);
}

template <class T>
inline void BitSet<T>::reset(unsigned int pos)
{
	bits_ &= ~(T(1) << pos);
}

template <class T>
inline void BitSet<T>::flip(unsigned int pos)
{
	bits_ ^= (T(1) << pos);
}

}

#endif
