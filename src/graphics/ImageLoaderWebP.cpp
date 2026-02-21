#include "return_macros.h"
#include "ImageLoaderWebP.h"
#include "IFile.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ImageLoaderWebP::ImageLoaderWebP(nctl::UniquePtr<IFile> fileHandle)
    : IImageLoader(nctl::move(fileHandle))
{
	LOGI_X("Loading \"%s\"", fileHandle_->filename());

	// Loading the whole file in memory
	fileHandle_->open(IFile::OpenMode::READ | IFile::OpenMode::BINARY);
	RETURN_ASSERT_MSG_X(fileHandle_->isOpened(), "File \"%s\" cannot be opened", fileHandle_->filename());
	const long int fileSize = fileHandle_->size();
	nctl::UniquePtr<unsigned char[]> fileBuffer = nctl::makeUnique<unsigned char[]>(fileSize);
	fileHandle_->read(fileBuffer.get(), fileSize);

	if (WebPGetInfo(fileBuffer.get(), fileSize, &width_, &height_) == 0)
	{
		fileBuffer.reset(nullptr);
		RETURN_MSG("Cannot read WebP header");
	}

	LOGI_X("Header found: w:%d h:%d", width_, height_);

	WebPBitstreamFeatures features;
	if (WebPGetFeatures(fileBuffer.get(), fileSize, &features) != VP8_STATUS_OK)
	{
		fileBuffer.reset(nullptr);
		RETURN_MSG("Cannot retrieve WebP features from headers");
	}

	LOGI_X("Bitstream features found: alpha:%d animation:%d format:%d",
	       features.has_alpha, features.has_animation, features.format);

	format_ = features.has_alpha ? Format::RGBA8 : Format::RGB8;
	dataSize_ = width_ * height_ * numChannels(); // Set the format before calling `numChannels()`
	pixels_ = nctl::makeUnique<unsigned char[]>(dataSize_);

	if (features.has_alpha)
	{
		if (WebPDecodeRGBAInto(fileBuffer.get(), fileSize, pixels_.get(), dataSize_, width_ * 4) == nullptr)
		{
			fileBuffer.reset(nullptr);
			pixels_.reset(nullptr);
			RETURN_MSG("Cannot decode RGBA WebP image");
		}
	}
	else
	{
		if (WebPDecodeRGBInto(fileBuffer.get(), fileSize, pixels_.get(), dataSize_, width_ * 3) == nullptr)
		{
			fileBuffer.reset(nullptr);
			pixels_.reset(nullptr);
			RETURN_MSG("Cannot decode RGB WebP image");
		}
	}

	hasLoaded_ = true;
}

}
