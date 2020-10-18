#include <sys/stat.h> // for open()
#include <fcntl.h> // for open()
#include <unistd.h> // for close()
#include "common_macros.h"
#include "AssetFile.h"

namespace ncine {

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

AAssetManager *AssetFile::assetManager_ = nullptr;
const char *AssetFile::Prefix = "asset::";

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

AssetFile::AssetFile(const char *filename)
    : IFile(filename), asset_(nullptr), startOffset_(0L)
{
	type_ = FileType::ASSET;
}

AssetFile::~AssetFile()
{
	if (shouldCloseOnDestruction_)
		close();
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void AssetFile::open(unsigned char mode)
{
	// Checking if the file is already opened
	if (fileDescriptor_ >= 0 || asset_ != nullptr)
		LOGW_X("File \"%s\" is already opened", filename_.data());
	else
	{
		// Opening with a file descriptor
		if (mode & OpenMode::FD)
			openFD(mode);
		// Opening as an asset only
		else
			openAsset(mode);
	}
}

/*! This method will close a file both normally opened or fopened */
void AssetFile::close()
{
	if (fileDescriptor_ >= 0)
	{
		const int retValue = ::close(fileDescriptor_);
		if (retValue < 0)
			LOGW_X("Cannot close the file \"%s\"", filename_.data());
		else
		{
			LOGI_X("File \"%s\" closed", filename_.data());
			fileDescriptor_ = -1;
		}
	}
	else if (asset_)
	{
		AAsset_close(asset_);
		asset_ = nullptr;
		LOGI_X("File \"%s\" closed", filename_.data());
	}
}

long int AssetFile::seek(long int offset, int whence) const
{
	long int seekValue = -1;

	if (fileDescriptor_ >= 0)
	{
		switch (whence)
		{
			case SEEK_SET:
				seekValue = lseek(fileDescriptor_, startOffset_ + offset, SEEK_SET);
				break;
			case SEEK_CUR:
				seekValue = lseek(fileDescriptor_, offset, SEEK_CUR);
				break;
			case SEEK_END:
				seekValue = lseek(fileDescriptor_, startOffset_ + fileSize_ + offset, SEEK_END);
				break;
		}
		seekValue -= startOffset_;
	}
	else if (asset_)
		seekValue = AAsset_seek(asset_, offset, whence);

	return seekValue;
}

long int AssetFile::tell() const
{
	long int tellValue = -1;

	if (fileDescriptor_ >= 0)
		tellValue = lseek(fileDescriptor_, 0L, SEEK_CUR) - startOffset_;
	else if (asset_)
		tellValue = AAsset_seek(asset_, 0L, SEEK_CUR);

	return tellValue;
}

unsigned long int AssetFile::read(void *buffer, unsigned long int bytes) const
{
	ASSERT(buffer);

	unsigned long int bytesRead = 0;

	if (fileDescriptor_ >= 0)
	{
		int bytesToRead = bytes;

		const long int seekValue = lseek(fileDescriptor_, 0L, SEEK_CUR);

		if (seekValue >= startOffset_ + fileSize_)
			bytesToRead = 0; // simulating EOF
		else if (seekValue + static_cast<long int>(bytes) > startOffset_ + fileSize_)
			bytesToRead = (startOffset_ + fileSize_) - seekValue;

		bytesRead = ::read(fileDescriptor_, buffer, bytesToRead);
	}
	else if (asset_)
		bytesRead = AAsset_read(asset_, buffer, bytes);

	return bytesRead;
}

bool AssetFile::isOpened() const
{
	if (fileDescriptor_ >= 0 || asset_ != nullptr)
		return true;
	else
		return false;
}

const char *AssetFile::assetPath(const char *path)
{
	ASSERT(path);
	if (strncmp(path, Prefix, strlen(Prefix)) == 0)
	{
		// Skip leading path separator character
		return (path[7] == '/') ? path + 8 : path + 7;
	}
	return nullptr;
}

bool AssetFile::tryOpen(const char *path)
{
	ASSERT(path);
	return (tryOpenFile(path) || tryOpenDirectory(path));
}

bool AssetFile::tryOpenFile(const char *path)
{
	ASSERT(path);
	const char *strippedPath = assetPath(path);
	if (strippedPath == nullptr)
		return false;

	AAsset *asset = AAssetManager_open(assetManager_, strippedPath, AASSET_MODE_UNKNOWN);
	if (asset)
	{
		AAsset_close(asset);
		return true;
	}

	return false;
}

bool AssetFile::tryOpenDirectory(const char *path)
{
	ASSERT(path);
	const char *strippedPath = assetPath(path);
	if (strippedPath == nullptr)
		return false;

	AAsset *asset = AAssetManager_open(assetManager_, strippedPath, AASSET_MODE_UNKNOWN);
	if (asset)
	{
		AAsset_close(asset);
		return false;
	}

	AAssetDir *assetDir = AAssetManager_openDir(assetManager_, strippedPath);
	if (assetDir)
	{
		AAssetDir_close(assetDir);
		return true;
	}

	return false;
}

off_t AssetFile::length(const char *path)
{
	ASSERT(path);

	off_t assetLength = 0;
	const char *strippedPath = assetPath(path);
	if (strippedPath == nullptr)
		return assetLength;

	AAsset *asset = AAssetManager_open(assetManager_, strippedPath, AASSET_MODE_UNKNOWN);
	if (asset)
	{
		assetLength = AAsset_getLength(asset);
		AAsset_close(asset);
	}

	return assetLength;
}

AAssetDir *AssetFile::openDir(const char *dirName)
{
	ASSERT(dirName);
	return AAssetManager_openDir(assetManager_, dirName);
}

void AssetFile::closeDir(AAssetDir *assetDir)
{
	ASSERT(assetDir);
	AAssetDir_close(assetDir);
}

void AssetFile::rewindDir(AAssetDir *assetDir)
{
	ASSERT(assetDir);
	AAssetDir_rewind(assetDir);
}

const char *AssetFile::nextFileName(AAssetDir *assetDir)
{
	ASSERT(assetDir);
	return AAssetDir_getNextFileName(assetDir);
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void AssetFile::openFD(unsigned char mode)
{
	// An asset file can only be read
	if (mode == (OpenMode::FD | OpenMode::READ))
	{
		asset_ = AAssetManager_open(assetManager_, filename_.data(), AASSET_MODE_UNKNOWN);
		if (asset_ == nullptr)
		{
			if (shouldExitOnFailToOpen_)
			{
				LOGF_X("Cannot open the file \"%s\"", filename_.data());
				exit(EXIT_FAILURE);
			}
			else
			{
				LOGE_X("Cannot open the file \"%s\"", filename_.data());
				return;
			}
		}

		off_t outStart = 0;
		off_t outLength = 0;
		fileDescriptor_ = AAsset_openFileDescriptor(asset_, &outStart, &outLength);
		startOffset_ = outStart;
		fileSize_ = outLength;

		lseek(fileDescriptor_, startOffset_, SEEK_SET);
		AAsset_close(asset_);
		asset_ = nullptr;

		if (fileDescriptor_ < 0)
		{
			if (shouldExitOnFailToOpen_)
			{
				LOGF_X("Cannot open the file \"%s\"", filename_.data());
				exit(EXIT_FAILURE);
			}
			else
			{
				LOGE_X("Cannot open the file \"%s\"", filename_.data());
				return;
			}
		}
		else
			LOGI_X("File \"%s\" opened", filename_.data());
	}
	else
		LOGE_X("Cannot open the file \"%s\", wrong open mode", filename_.data());
}

void AssetFile::openAsset(unsigned char mode)
{
	// An asset file can only be read
	if (mode == OpenMode::READ || mode == (OpenMode::READ | OpenMode::BINARY))
	{
		asset_ = AAssetManager_open(assetManager_, filename_.data(), AASSET_MODE_UNKNOWN);
		if (asset_ == nullptr)
		{
			if (shouldExitOnFailToOpen_)
			{
				LOGF_X("Cannot open the file \"%s\"", filename_.data());
				exit(EXIT_FAILURE);
			}
			else
			{
				LOGE_X("Cannot open the file \"%s\"", filename_.data());
				return;
			}
		}
		else
			LOGI_X("File \"%s\" opened", filename_.data());

		// Calculating file size
		fileSize_ = AAsset_getLength(asset_);
	}
	else
		LOGE_X("Cannot open the file \"%s\", wrong open mode", filename_.data());
}

}
