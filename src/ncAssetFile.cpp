#include <sys/stat.h> // for open()
#include <fcntl.h> // for open()
#include <unistd.h> // for close()
#include <stdlib.h> // for exit()
#include "ncAssetFile.h"
#include "ncServiceLocator.h"

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

AAssetManager* ncAssetFile::assetManager_ = NULL;

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ncAssetFile::ncAssetFile(const char *filename)
	: ncIFile(filename), asset_(NULL), startOffset_(0L)
{
	type_ = ASSET_TYPE;

	// Detect fake second extension added to prevent compression
	char *firstDotChar = strchr(filename_, '.');
	char *lastDotChar = strrchr(filename_, '.');
	int extLEngth = lastDotChar - firstDotChar - 1;
	if (extLEngth >= 3 && extLEngth <= 4)
	{
		memset(extension_, 0, MaxExtensionsLength);
		strncpy(extension_, firstDotChar + 1, extLEngth);
	}
}

ncAssetFile::~ncAssetFile()
{
	if (shouldCloseOnExit_)
	{
		close();
	}
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/// Tries to open the file
void ncAssetFile::open(unsigned char mode)
{
	// Checking if the file is already opened
	if (fileDescriptor_ >= 0 || asset_ != NULL)
	{
		LOGW_X("File \"%s\" is already opened", filename_);
	}
	else
	{
		// Opening with a file descriptor
		if (mode & MODE_FD)
		{
			openFd(mode);
		}
		// Opening as an asset only
		else
		{
			openAsset(mode);
		}
	}
}


/// Closes the file (both opened or fopened)
void ncAssetFile::close()
{
	if (fileDescriptor_ >= 0)
	{
		int retValue = ::close(fileDescriptor_);
		if (retValue < 0)
		{
			LOGW_X("Cannot close the file \"%s\"", filename_);
		}
		else
		{
			LOGI_X("File \"%s\" closed", filename_);
			fileDescriptor_ = -1;
		}
	}
	else if (asset_)
	{
		AAsset_close(asset_);
		asset_ = NULL;
		LOGI_X("File \"%s\" closed", filename_);
	}
}

long int ncAssetFile::seek(long int offset, int whence) const
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
	{
		seekValue = AAsset_seek(asset_, offset, whence);
	}

	return seekValue;
}

long int ncAssetFile::tell() const
{
	long int tellValue = -1;

	if (fileDescriptor_ >= 0)
	{
		tellValue = lseek(fileDescriptor_, 0L, SEEK_CUR) - startOffset_;
	}
	else if (asset_)
	{
		tellValue = AAsset_seek(asset_, 0L, SEEK_CUR);
	}

	return tellValue;
}


long int ncAssetFile::read(void *buffer, int bytes) const
{
	long int bytesRead = -1;

	if (fileDescriptor_ >= 0)
	{
		int bytesToRead = bytes;

		long int seekValue = lseek(fileDescriptor_, 0L, SEEK_CUR);

		if (seekValue >=  startOffset_ + fileSize_)
		{
			bytesToRead = 0;    // simulating EOF
		}
		else if (seekValue + bytes > startOffset_ + fileSize_)
		{
			bytesToRead = (startOffset_ + fileSize_) - seekValue;
		}

		bytesRead = ::read(fileDescriptor_, buffer, bytesToRead);
	}
	else if (asset_)
	{
		bytesRead = AAsset_read(asset_, buffer, bytes);
	}

	return bytesRead;
}

bool ncAssetFile::isOpened() const
{
	if (fileDescriptor_ >= 0 || asset_ != NULL)
	{
		return true;
	}
	else
	{
		return false;
	}
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

/// Opens the file with AAsset_openFileDescriptor()
void ncAssetFile::openFd(unsigned char mode)
{
	// An asset file can only be read
	if (mode == (MODE_FD | MODE_READ))
	{
		asset_ = AAssetManager_open(assetManager_, filename_, AASSET_MODE_UNKNOWN);
		if (asset_ == NULL)
		{
			LOGF_X("Cannot open the file \"%s\"", filename_);
			exit(-1);
		}

		fileDescriptor_ = AAsset_openFileDescriptor(asset_, &startOffset_, &fileSize_);
		lseek(fileDescriptor_, startOffset_, SEEK_SET);
		AAsset_close(asset_);
		asset_ = NULL;

		if (fileDescriptor_ < 0)
		{
			LOGF_X("Cannot open the file \"%s\"", filename_);
			exit(-1);
		}
		else
		{
			LOGI_X("File \"%s\" opened", filename_);
		}
	}
	else
	{
		LOGE_X("Cannot open the file \"%s\", wrong open mode", filename_);
	}
}

/// Opens the file with AAssetManager_open() only
void ncAssetFile::openAsset(unsigned char mode)
{
	// An asset file can only be read
	if (mode == MODE_READ || mode == (MODE_READ | MODE_BINARY))
	{
		asset_ = AAssetManager_open(assetManager_, filename_, AASSET_MODE_UNKNOWN);
		if (asset_ == NULL)
		{
			LOGF_X("Cannot open the file \"%s\"", filename_);
			exit(-1);
		}
		else
		{
			LOGI_X("File \"%s\" opened", filename_);
		}

		// Calculating file size
		fileSize_ = AAsset_getLength(asset_);
	}
	else
	{
		LOGE_X("Cannot open the file \"%s\", wrong open mode", filename_);
	}
}

/// Checks if a file can be accessed with specified mode
/*! It is called by ncIFile::access() */
bool ncAssetFile::access(const char *filename, unsigned char mode)
{
	bool isAccessible = false;

	if (mode == ncIFile::MODE_EXISTS || mode == ncIFile::MODE_CAN_READ)
	{
		AAsset *asset = AAssetManager_open(assetManager_, filename, AASSET_MODE_UNKNOWN);
		if (asset)
		{
			isAccessible = true;
			AAsset_close(asset);
		}
	}
	else if (mode & MODE_CAN_WRITE)
	{
		LOGE_X("Cannot access the file \"%s\", an asset can only be read", filename);
	}
	else
	{
		LOGE_X("Cannot access the file \"%s\", wrong access mode", filename);
	}

	return isAccessible;
}
