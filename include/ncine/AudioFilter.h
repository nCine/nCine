#ifndef CLASS_NCINE_AUDIOFILTER
#define CLASS_NCINE_AUDIOFILTER

#include "common_defines.h"

namespace ncine {

/// A class representing an OpenAL filter
class DLL_PUBLIC AudioFilter
{
  public:
	enum class Type
	{
		NULL_FILTER,
		LOWPASS,
		HIGHPASS,
		BANDPASS
	};

	struct Properties
	{
		Type type = Type::NULL_FILTER;
		float gain = 1.0f;
		float gainLF = 1.0f;
		float gainHF = 1.0f;
	};

	/// Creates an OpenAL filter name
	AudioFilter();
	/// Creates an OpenAL filter name with the specified type
	explicit AudioFilter(Type type);
	~AudioFilter();

	/// Move constructor
	AudioFilter(AudioFilter &&other);
	/// Move assignment operator
	AudioFilter &operator=(AudioFilter &&other);

	/// Returns the of the filter
	inline Type type() const { return type_; }

	/// Returns the OpenAL filter id
	inline unsigned int filterId() const { return filterId_; }

	void applyProperties(const Properties &properties);

  private:
	/// The filter type
	Type type_;

	/// The OpenAL filter id
	unsigned int filterId_;

	/// Deleted copy constructor
	AudioFilter(const AudioFilter &) = delete;
	/// Deleted assignment operator
	AudioFilter &operator=(const AudioFilter &) = delete;

	/// Sets the type of the filter
	void setType(Type type);
};

}

#endif
