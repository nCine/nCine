#define QOI_IMPLEMENTATION
#include "qoi.h"

#include "return_macros.h"
#include "ImageLoaderQoi.h"
#include "IFile.h"

namespace ncine {

namespace {

	const char *colorspaceString(uint8_t colorspace)
	{
		switch (colorspace)
		{
			case 0: return "sRGB with linear alpha";
			case 1: return "all channels linear";
			default: return "unknown";
		}
	}

}

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ImageLoaderQoi::ImageLoaderQoi(nctl::UniquePtr<IFile> fileHandle)
    : IImageLoader(nctl::move(fileHandle))
{
	LOGI_X("Loading \"%s\"", fileHandle_->filename());

	const int SignatureLength = 8;
	unsigned char signature[SignatureLength];
	fileHandle_->open(IFile::OpenMode::READ | IFile::OpenMode::BINARY);
	RETURN_ASSERT_MSG_X(fileHandle_->isOpened(), "File \"%s\" cannot be opened", fileHandle_->filename());
	const long int fileSize = fileHandle_->size();
	nctl::UniquePtr<unsigned char[]> fileBuffer = nctl::makeUnique<unsigned char[]>(fileSize);
	fileHandle_->read(fileBuffer.get(), fileSize);

	qoi_desc desc;
	void *decodedPixels = qoi_decode(fileBuffer.get(), fileSize, &desc, 0);
	if (decodedPixels == nullptr)
		RETURN_MSG("Cannot decode QOI image");

	width_ = desc.width;
	height_ = desc.height;
	LOGI_X("Header found: w:%d, h:%d, channels:%d, colorspace:%s", width_, height_, desc.channels, colorspaceString(desc.colorspace));

	format_ = (desc.channels == 4) ? Format::RGBA8 : Format::RGB8;
	dataSize_ = width_ * height_ * numChannels(); // Set the format before calling `numChannels()`
	// The QOI decoded buffer allocated with `malloc()`, but the smart pointer uses a `delete[]` to free its memory
	pixels_ = nctl::makeUnique<unsigned char[]>(dataSize_);
	memcpy(pixels_.get(), decodedPixels, dataSize_);
	free(decodedPixels);

	hasLoaded_ = true;
}

}
