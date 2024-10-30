#include "MemoryFile.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

MemoryFile::MemoryFile(const char *bufferName, unsigned char *bufferPtr, unsigned long int bufferSize)
    : IFile(bufferName), bufferPtr_(bufferPtr), seekOffset_(0), isWritable_(true)
{
	ASSERT(bufferPtr != nullptr);
	ASSERT(bufferSize > 0);
	type_ = FileType::MEMORY;
	fileSize_ = bufferSize;

	// The memory file appears to be already opened when first created
	fileDescriptor_ = (bufferSize > 0) ? 0 : -1;
}

MemoryFile::MemoryFile(const char *bufferName, const unsigned char *bufferPtr, unsigned long int bufferSize)
    : IFile(bufferName), bufferPtr_(const_cast<unsigned char *>(bufferPtr)), seekOffset_(0), isWritable_(false)
{
	ASSERT(bufferPtr != nullptr);
	ASSERT(bufferSize > 0);
	type_ = FileType::MEMORY;
	fileSize_ = bufferSize;

	// The memory file appears to be already opened as read-only when first created
	fileDescriptor_ = (bufferSize > 0) ? 0 : -1;
}

MemoryFile::MemoryFile(unsigned char *bufferPtr, unsigned long int bufferSize)
    : MemoryFile("MemoryFile", bufferPtr, bufferSize)
{
}

MemoryFile::MemoryFile(const unsigned char *bufferPtr, unsigned long int bufferSize)
    : MemoryFile("MemoryFile", bufferPtr, bufferSize)
{
}

MemoryFile::MemoryFile(const char *bufferName, unsigned long int bufferSize)
    : IFile(bufferName), bufferPtr_(nullptr), seekOffset_(0), isWritable_(true)
{
	ASSERT(bufferSize > 0);
	type_ = FileType::MEMORY;
	fileSize_ = bufferSize;

	ownedBuffer_ = nctl::makeUnique<uint8_t[]>(bufferSize);
	bufferPtr_ = ownedBuffer_.get();
}

MemoryFile::MemoryFile(unsigned long int bufferSize)
    : MemoryFile("MemoryFile", bufferSize)
{
}

MemoryFile::MemoryFile(const char *bufferName, nctl::UniquePtr<unsigned char []> buffer, unsigned long int bufferSize)
    : IFile(bufferName), bufferPtr_(nullptr), seekOffset_(0), isWritable_(true)
{
	ASSERT(bufferSize > 0);
	type_ = FileType::MEMORY;
	fileSize_ = bufferSize;

	ownedBuffer_ = nctl::move(buffer);
	bufferPtr_ = ownedBuffer_.get();
}

MemoryFile::MemoryFile(nctl::UniquePtr<unsigned char []> buffer, unsigned long int bufferSize)
    : MemoryFile("MemoryFile", nctl::move(buffer), bufferSize)
{
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void MemoryFile::open(unsigned char mode)
{
	// Checking if the file is already opened
	if (fileDescriptor_ >= 0)
		LOGW_X("Memory file \"%s\" at address 0x%lx is already opened", filename_.data(), bufferPtr_);
	else
		isWritable_ = (mode & OpenMode::WRITE);
}

void MemoryFile::close()
{
	fileDescriptor_ = -1;
	seekOffset_ = 0;
	isWritable_ = false;
}

long int MemoryFile::seek(long int offset, int whence) const
{
	long int seekValue = -1;

	if (fileDescriptor_ >= 0)
	{
		switch (whence)
		{
			case SEEK_SET:
				seekValue = offset;
				break;
			case SEEK_CUR:
				seekValue = seekOffset_ + offset;
				break;
			case SEEK_END:
				seekValue = fileSize_ + offset;
				break;
		}
	}

	if (seekValue < 0 || seekValue > static_cast<long int>(fileSize_))
		seekValue = -1;
	else
		seekOffset_ = seekValue;

	return seekValue;
}

long int MemoryFile::tell() const
{
	long int tellValue = -1;

	if (fileDescriptor_ >= 0)
		tellValue = seekOffset_;

	return tellValue;
}

unsigned long int MemoryFile::read(void *buffer, unsigned long int bytes) const
{
	ASSERT(buffer);

	unsigned long int bytesRead = 0;

	if (fileDescriptor_ >= 0)
	{
		bytesRead = (seekOffset_ + bytes > fileSize_) ? fileSize_ - seekOffset_ : bytes;
		memcpy(buffer, bufferPtr_ + seekOffset_, bytesRead);
		seekOffset_ += bytesRead;
	}

	return bytesRead;
}

unsigned long int MemoryFile::write(const void *buffer, unsigned long int bytes)
{
	ASSERT(buffer);

	unsigned long int bytesWritten = 0;

	if (fileDescriptor_ >= 0 && isWritable_)
	{
		bytesWritten = (seekOffset_ + bytes > fileSize_) ? fileSize_ - seekOffset_ : bytes;
		memcpy(bufferPtr_ + seekOffset_, buffer, bytesWritten);
		seekOffset_ += bytesWritten;
	}

	return bytesWritten;
}

}
