#ifndef CLASS_NCAUDIOLOADERWAV
#define CLASS_NCAUDIOLOADERWAV

#include "ncIAudioLoader.h"

/// WAVE audio loader
class ncAudioLoaderWav : public ncIAudioLoader
{
  public:
	ncAudioLoaderWav(const char *filename);
	ncAudioLoaderWav(ncIFile *fileHandle);
	~ncAudioLoaderWav() { }

	long read(char *buffer, int bufferSize) const;
	void rewind() const;

  private:
	/// Header for the RIFF WAVE format
	typedef struct WavHeader
	{
		char		chunkId[4];
		uint32_t	chunkSize;
		char		format[4];

		char		subchunk1Id[4];
		uint32_t	subchunk1Size;
		uint16_t	audioFormat;
		uint16_t	numChannels;
		uint32_t	sampleRate;
		uint32_t	byteRate;
		uint16_t	blockAlign;
		uint16_t	bitsPerSample;

		char		subchunk2Id[4];
		uint32_t	subchunk2Size;
	} WavHeader;

	void init();
};

#endif
