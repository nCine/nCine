#ifndef CLASS_NCIAUDIOLOADER
#define CLASS_NCIAUDIOLOADER

#include "ncIFile.h"

/// Audio loader class
class ncIAudioLoader
{
  public:
	virtual ~ncIAudioLoader();

	/// Decodes audio data in a specified buffer
	/*! \param buffer Buffer pointer
	 *  \param bufferSize Buffer size in bytes
	 *  \return Number of bytes read.
	 */
	virtual long read(char *buffer, int bufferSize) const = 0;
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
	inline int frequency() const { return frequency_; }

	/// Returns the decoded buffer size in bytes
	inline int bufferSize() const { return numSamples_ * numChannels_ * bytesPerSample_; }

	// Returns the proper audio loader according to the file extension
	static ncIAudioLoader* createFromFile(const char *filename);

  protected:
	/// Audio file handle
	ncIFile *fileHandle_;

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

	ncIAudioLoader(const char *filename);
	ncIAudioLoader(ncIFile *fileHandle);

	/// Load the audio parameters from file
	virtual void init() = 0;
};

#endif
