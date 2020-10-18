#ifndef CLASS_NCINE_IAUDIOREADER
#define CLASS_NCINE_IAUDIOREADER

namespace ncine {

/// Audio reader interface class
class IAudioReader
{
  public:
	virtual ~IAudioReader() {}

	/// Decodes audio data in a specified buffer
	/*!
	 * \param buffer Buffer pointer
	 * \param bufferSize Buffer size in bytes
	 * \return Number of bytes read
	 */
	virtual unsigned long int read(void *buffer, unsigned long int bufferSize) const = 0;

	/// Resets the audio file seek value
	virtual void rewind() const = 0;
};

class InvalidAudioReader : IAudioReader
{
  public:
	inline unsigned long int read(void *buffer, unsigned long int bufferSize) const override { return 0; }
	inline void rewind() const override {};
};

}

#endif
