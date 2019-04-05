#ifndef CLASS_NCINE_IAUDIOLOADER
#define CLASS_NCINE_IAUDIOLOADER

#include "IFile.h"
#include <nctl/UniquePtr.h>

namespace ncine {

/// Audio loader interface class
class DLL_PUBLIC IAudioLoader
{
  public:
	virtual ~IAudioLoader() { }

	/// Decodes audio data in a specified buffer
	/*!
	 * \param buffer Buffer pointer
	 * \param bufferSize Buffer size in bytes
	 * \return Number of bytes read
	 */
	virtual unsigned long int read(char *buffer, unsigned long int bufferSize) const = 0;
	/// Resets the audio file seek value
	virtual void rewind() const = 0;

	/// Returns the duration in seconds
	inline float duration() const { return duration_; }
	/// Returns number of samples
	inline unsigned long int numSamples() const { return numSamples_; }
	/// Returns number of bytes per sample
	inline int bytesPerSample() const { return bytesPerSample_; }
	/// Returns number of channels
	inline int numChannels() const { return numChannels_; }
	/// Returns samples frequency
	inline unsigned long int frequency() const { return frequency_; }

	/// Returns the decoded buffer size in bytes
	inline unsigned long int bufferSize() const { return numSamples_ * numChannels_ * bytesPerSample_; }

	/// Returns the proper audio loader according to the file extension
	static nctl::UniquePtr<IAudioLoader> createFromFile(const char *filename);

  protected:
	/// Audio file handle
	nctl::UniquePtr<IFile> fileHandle_;

	/// Number of bytes per sample
	int bytesPerSample_;
	/// Number of channels
	int numChannels_;
	/// Samples frequency
	int frequency_;

	/// Number of samples
	unsigned long int numSamples_;
	/// Duration in seconds
	float duration_;

	explicit IAudioLoader(const char *filename);
	explicit IAudioLoader(nctl::UniquePtr<IFile> fileHandle);
};

}

#endif
