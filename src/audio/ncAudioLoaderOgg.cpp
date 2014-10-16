#include <cstdlib> // for exit()
#include <cstring>
#include "ncAudioLoaderOgg.h"
#include "ncServiceLocator.h"

#ifdef __ANDROID__
#include "ncAssetFile.h" // for ncAssetFile::sType()

size_t asset_read(void *ptr, size_t size, size_t nmemb, void *datasource)
{
	ncAssetFile *assetFile = static_cast<ncAssetFile *>(datasource);
	return assetFile->read(ptr, size * nmemb);
}

int asset_seek(void *datasource, ogg_int64_t offset, int whence)
{
	ncAssetFile *assetFile = static_cast<ncAssetFile *>(datasource);
	return assetFile->seek(offset, whence);
}

int asset_close(void *datasource)
{
	ncAssetFile *assetFile = static_cast<ncAssetFile *>(datasource);
	assetFile->close();
	return 0;
}

long asset_tell(void *datasource)
{
	ncAssetFile *assetFile = static_cast<ncAssetFile *>(datasource);
	return assetFile->tell();
}

ov_callbacks oggCallbacks = { asset_read, asset_seek, asset_close, asset_tell };
#endif

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ncAudioLoaderOgg::ncAudioLoaderOgg(const char *filename)
	: ncIAudioLoader(filename)
{
	init();
}

ncAudioLoaderOgg::ncAudioLoaderOgg(ncIFile *fileHandle)
	: ncIAudioLoader(fileHandle)
{
	init();
}

ncAudioLoaderOgg::~ncAudioLoaderOgg()
{
	ov_clear(&oggFile_);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

long ncAudioLoaderOgg::read(char *buffer, int bufferSize) const
{
	static int bitStream = 0;
	long bytes;
	long int bufferSeek = 0;

	do
	{
		// Read up to a buffer's worth of decoded sound data
#ifdef __ANDROID__
		bytes = ov_read(&oggFile_, buffer + bufferSeek, bufferSize - bufferSeek, &bitStream);
#else
		// 0 - little endian, 2 - 16bit, 1 - signed
		bytes = ov_read(&oggFile_, buffer + bufferSeek, bufferSize - bufferSeek, 0, 2, 1, &bitStream);
#endif

		if (bytes < 0)
		{
			ov_clear(&oggFile_);
			ncServiceLocator::logger().write(ncILogger::LOG_FATAL, (const char *)"ncAudioLoaderOgg::read - Error decoding at bitstream %d", bitStream);
			exit(EXIT_FAILURE);
		}

		// Reset the static variable at the end of a decoding process
		if (bytes <= 0)
		{
			bitStream = 0;
		}

		bufferSeek += bytes;
	} while (bytes > 0 && bufferSize - bufferSeek > 0);

	return bufferSeek;
}

void ncAudioLoaderOgg::rewind() const
{
	ov_raw_seek(&oggFile_, 0);
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void ncAudioLoaderOgg::init()
{
	vorbis_info *info;

	ncServiceLocator::logger().write(ncILogger::LOG_INFO, (const char *)"ncAudioLoaderOgg::init - Loading \"%s\"", fileHandle_->filename());

	// File is closed by ov_clear()
	fileHandle_->setCloseOnExit(false);

#ifdef __ANDROID__
	if (fileHandle_->type() == ncAssetFile::sType())
	{
		fileHandle_->open(ncIFile::MODE_FD | ncIFile::MODE_READ);

		if (ov_open_callbacks(fileHandle_, &oggFile_, NULL, 0, oggCallbacks) != 0)
		{
			ncServiceLocator::logger().write(ncILogger::LOG_FATAL, (const char *)"ncAudioLoaderOgg::init - Cannot open \"%s\" with ov_open_callbacks()", fileHandle_->filename());
			fileHandle_->close();
			exit(EXIT_FAILURE);
		}
	}
	else
	{
		fileHandle_->open(ncIFile::MODE_READ | ncIFile::MODE_BINARY);

		if (ov_open(fileHandle_->ptr(), &oggFile_, NULL, 0) != 0)
		{
			ncServiceLocator::logger().write(ncILogger::LOG_FATAL, (const char *)"ncAudioLoaderOgg::init - Cannot open \"%s\" with ov_open()", fileHandle_->filename());
			fileHandle_->close();
			exit(EXIT_FAILURE);
		}
	}
#else
	if (ov_fopen(fileHandle_->filename(), &oggFile_) != 0)
	{
		ncServiceLocator::logger().write(ncILogger::LOG_FATAL, (const char *)"ncAudioLoaderOgg::init - Cannot open \"%s\" with ov_fopen()", fileHandle_->filename());
		exit(EXIT_FAILURE);
	}
#endif

	// Get some information about the OGG file
	info = ov_info(&oggFile_, -1);

	bytesPerSample_ = 2; // Ogg is always 16 bits
	numChannels_ = info->channels;
	frequency_ = info->rate;

	numSamples_ = ov_pcm_total(&oggFile_, -1);
	duration_ = float(ov_time_total(&oggFile_, -1));

	ncServiceLocator::logger().write(ncILogger::LOG_INFO, (const char *)"ncAudioLoaderOgg::init - duration: %.2f, channels: %d, frequency: %d", duration_, numChannels_, frequency_);
}
