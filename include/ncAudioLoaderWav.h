#ifndef CLASS_NCAUDIOLOADERWAV
#define CLASS_NCAUDIOLOADERWAV

#include "ncIAudioLoader.h"

/// WAVE audio loader
class ncAudioLoaderWav : public ncIAudioLoader
{
 public:
	ncAudioLoaderWav(const char *pFilename);
	ncAudioLoaderWav(ncIFile *pFileHandle);
	~ncAudioLoaderWav() { }

	long Read(char *pBuffer, int iBufSize) const;
	void Rewind() const;

 private:
	/// Header for the RIFF WAVE format
	typedef struct WAV_header {
		char		cChunkID[4];
		uint32_t	uChunkSize;
		char		cFormat[4];

		char		cSubchunk1Id[4];
		uint32_t	uSubchunk1Size;
		uint16_t	uAudioFormat;
		uint16_t	uNumChannels;
		uint32_t	uSampleRate;
		uint32_t	uByteRate;
		uint16_t	uBlockAlign;
		uint16_t	uBitsPerSample;

		char		cSubchunk2Id[4];
		uint32_t	uSubchunk2Size;
	} WAV_header;

	void Init();
};

#endif
