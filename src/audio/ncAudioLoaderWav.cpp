#include <cstdlib> // for exit()
#include <cstring>
#include <stdint.h> // for WAV header
#include "ncAudioLoaderWav.h"
#include "ncServiceLocator.h"

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ncAudioLoaderWav::ncAudioLoaderWav(const char *filename)
	: ncIAudioLoader(filename)
{
	init();
}

ncAudioLoaderWav::ncAudioLoaderWav(ncIFile *fileHandle)
	: ncIAudioLoader(fileHandle)
{
	init();
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

long ncAudioLoaderWav::read(char *buffer, int bufferSize) const
{
	long bytes_;
	long int bufferSeek = 0;

	do
	{
		// Read up to a buffer's worth of decoded sound data
		bytes_ = fileHandle_->read(buffer, bufferSize);

		if (bytes_ == 0 && ferror(fileHandle_->ptr()))
		{
			ncServiceLocator::logger().write(ncILogger::LOG_FATAL, (const char *)"ncAudioLoaderWav::read - Error reading the file");
			exit(EXIT_FAILURE);
		}

		bufferSeek += bytes_;
	}
	while (bytes_ > 0 && bufferSize - bufferSeek > 0);

	return bufferSeek;
}

void ncAudioLoaderWav::rewind() const
{
	clearerr(fileHandle_->ptr());
	fileHandle_->seek(sizeof(WavHeader), SEEK_SET);
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void ncAudioLoaderWav::init()
{
	ncServiceLocator::logger().write(ncILogger::LOG_INFO, (const char *)"ncAudioLoaderWav::init - Loading \"%s\"", fileHandle_->filename());
	fileHandle_->open(ncIFile::MODE_READ | ncIFile::MODE_BINARY);

	WavHeader header;
	fileHandle_->read(&header, sizeof(WavHeader));

	if (strncmp(header.chunkId, "RIFF", 4) != 0)
	{
		ncServiceLocator::logger().write(ncILogger::LOG_FATAL, (const char *)"ncAudioLoaderWav::init - \"%s\" is not a WAV file", fileHandle_->filename());
		exit(EXIT_FAILURE);
	}
	if (strncmp(header.format, "WAVE", 4) != 0)
	{
		ncServiceLocator::logger().write(ncILogger::LOG_FATAL, (const char *)"ncAudioLoaderWav::init - \"%s\" is not a WAV file", fileHandle_->filename());
		exit(EXIT_FAILURE);
	}
	if (strncmp(header.subchunk1Id, "fmt ", 4) != 0)
	{
		ncServiceLocator::logger().write(ncILogger::LOG_FATAL, (const char *)"ncAudioLoaderWav::init - \"%s\" is an invalid WAV file", fileHandle_->filename());
		exit(EXIT_FAILURE);
	}

	if (ncIFile::int16FromLE(header.audioFormat) != 1)
	{
		ncServiceLocator::logger().write(ncILogger::LOG_FATAL, (const char *)"ncAudioLoaderWav::init - Data in \"%s\" is not in PCM format", fileHandle_->filename());
		exit(EXIT_FAILURE);
	}

	bytesPerSample_ = ncIFile::int16FromLE(header.bitsPerSample) / 8;
	numChannels_ = ncIFile::int16FromLE(header.numChannels);
	frequency_ = ncIFile::int32FromLE(header.sampleRate);

	numSamples_ = ncIFile::int32FromLE(header.subchunk2Size) / (numChannels_ * bytesPerSample_);
	duration_ = float(numSamples_) / frequency_;

	ncServiceLocator::logger().write(ncILogger::LOG_INFO, (const char *)"ncAudioLoaderWav::init - duration: %.2f, channels: %d, frequency: %d", duration_, numChannels_, frequency_);
}
