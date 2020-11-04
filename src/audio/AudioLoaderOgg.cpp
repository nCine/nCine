#include "return_macros.h"
#include "AudioLoaderOgg.h"
#include "AudioReaderOgg.h"

namespace ncine {

namespace {
	size_t fileRead(void *ptr, size_t size, size_t nmemb, void *datasource)
	{
		IFile *file = static_cast<IFile *>(datasource);
		return file->read(ptr, size * nmemb);
	}

	int fileSeek(void *datasource, ogg_int64_t offset, int whence)
	{
		IFile *file = static_cast<IFile *>(datasource);
		return file->seek(offset, whence);
	}

	int fileClose(void *datasource)
	{
		IFile *file = static_cast<IFile *>(datasource);
		file->close();
		return 0;
	}

	long fileTell(void *datasource)
	{
		IFile *file = static_cast<IFile *>(datasource);
		return file->tell();
	}

	const ov_callbacks fileCallbacks = { fileRead, fileSeek, fileClose, fileTell };
}

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

AudioLoaderOgg::AudioLoaderOgg(nctl::UniquePtr<IFile> fileHandle)
    : IAudioLoader(nctl::move(fileHandle))
{
	LOGI_X("Loading \"%s\"", fileHandle_->filename());

	// File is closed by `ov_clear()`
	fileHandle_->setCloseOnDestruction(false);

#ifdef __ANDROID__
	if (fileHandle_->type() == IFile::FileType::ASSET)
		fileHandle_->open(IFile::OpenMode::FD | IFile::OpenMode::READ);
	else
#endif
		fileHandle_->open(IFile::OpenMode::READ | IFile::OpenMode::BINARY);

	if (ov_open_callbacks(fileHandle_.get(), &oggFile_, nullptr, 0, fileCallbacks) != 0)
	{
		LOGF_X("Cannot open \"%s\" with ov_open_callbacks()", fileHandle_->filename());
		fileHandle_->close();
		return;
	}

	// Get some information about the Ogg file
	const vorbis_info *info = ov_info(&oggFile_, -1);

	bytesPerSample_ = 2; // Ogg is always 16 bits
	numChannels_ = info->channels;
	frequency_ = info->rate;

	numSamples_ = static_cast<unsigned long int>(ov_pcm_total(&oggFile_, -1));
	duration_ = float(ov_time_total(&oggFile_, -1));

	RETURN_ASSERT_MSG_X(numChannels_ == 1 || numChannels_ == 2, "Unsupported number of channels: %d", numChannels_);
	LOGI_X("duration: %.2fs, channels: %d, frequency: %dHz", duration_, numChannels_, frequency_);

	hasLoaded_ = true;
}

AudioLoaderOgg::~AudioLoaderOgg()
{
	// Checking if the ownership of the `IFile` pointer has been transferred to a reader
	if (fileHandle_)
		ov_clear(&oggFile_);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

nctl::UniquePtr<IAudioReader> AudioLoaderOgg::createReader()
{
	return nctl::makeUnique<AudioReaderOgg>(nctl::move(fileHandle_), oggFile_);
}

}
