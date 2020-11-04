#ifndef CLASS_NCINE_IAUDIOLOADER
#define CLASS_NCINE_IAUDIOLOADER

#include "IFile.h"
#include <nctl/UniquePtr.h>
#include "IAudioReader.h"

namespace ncine {

/// Audio loader interface class
class IAudioLoader
{
  public:
	virtual ~IAudioLoader() {}

	/// Returns true if the audio has been correctly loaded
	inline bool hasLoaded() const { return hasLoaded_; }

	/// Returns number of bytes per sample
	inline int bytesPerSample() const { return bytesPerSample_; }
	/// Returns number of channels
	inline int numChannels() const { return numChannels_; }
	/// Returns samples frequency
	inline int frequency() const { return frequency_; }

	/// Returns number of samples
	inline unsigned long int numSamples() const { return numSamples_; }
	/// Returns the duration in seconds
	inline float duration() const { return duration_; }

	/// Returns the decoded buffer size in bytes
	inline unsigned long int bufferSize() const { return numSamples_ * numChannels_ * bytesPerSample_; }

	/// Returns the proper audio loader according to the memory buffer name extension
	static nctl::UniquePtr<IAudioLoader> createFromMemory(const char *bufferName, const unsigned char *bufferPtr, unsigned long int bufferSize);
	/// Returns the proper audio loader according to the file extension
	static nctl::UniquePtr<IAudioLoader> createFromFile(const char *filename);

	/// Returns the proper audio reader according to the loader instance
	virtual nctl::UniquePtr<IAudioReader> createReader() = 0;

  protected:
	/// A flag indicating if the loading process has been successful
	bool hasLoaded_;
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

	explicit IAudioLoader(nctl::UniquePtr<IFile> fileHandle);

	static nctl::UniquePtr<IAudioLoader> createLoader(nctl::UniquePtr<IFile> fileHandle, const char *filename);
};

/// A class created when the audio file extension is not recognized
class InvalidAudioLoader : IAudioLoader
{
  public:
	explicit InvalidAudioLoader(nctl::UniquePtr<IFile> fileHandle)
	    : IAudioLoader(nctl::move(fileHandle)) {}

	nctl::UniquePtr<IAudioReader> createReader() override { return nctl::makeUnique<InvalidAudioReader>(); }
};

}

#endif
