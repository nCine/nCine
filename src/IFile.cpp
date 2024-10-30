#include "common_macros.h"
#include "IFile.h"
#include "MemoryFile.h"
#include "StandardFile.h"

#ifdef __ANDROID__
	#include <cstring>
	#include "AssetFile.h"
#endif

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

IFile::IFile(const char *filename)
    : type_(FileType::BASE), filename_(filename), fileDescriptor_(-1),
      filePointer_(nullptr), shouldCloseOnDestruction_(true), fileSize_(0)
{
	ASSERT(filename);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

bool IFile::isOpened() const
{
	if (fileDescriptor_ >= 0 || filePointer_ != nullptr)
		return true;
	else
		return false;
}

nctl::UniquePtr<IFile> IFile::createFromMemory(const char *bufferName, unsigned char *bufferPtr, unsigned long int bufferSize)
{
	ASSERT(bufferName);
	ASSERT(bufferPtr);
	ASSERT(bufferSize > 0);
	return nctl::makeUnique<MemoryFile>(bufferName, bufferPtr, bufferSize);
}

nctl::UniquePtr<IFile> IFile::createFromMemory(const char *bufferName, const unsigned char *bufferPtr, unsigned long int bufferSize)
{
	ASSERT(bufferName);
	ASSERT(bufferPtr);
	ASSERT(bufferSize > 0);
	return nctl::makeUnique<MemoryFile>(bufferName, bufferPtr, bufferSize);
}

nctl::UniquePtr<IFile> IFile::createFromMemory(unsigned char *bufferPtr, unsigned long int bufferSize)
{
	ASSERT(bufferPtr);
	ASSERT(bufferSize > 0);
	return nctl::makeUnique<MemoryFile>(bufferPtr, bufferSize);
}

nctl::UniquePtr<IFile> IFile::createFromMemory(const unsigned char *bufferPtr, unsigned long int bufferSize)
{
	ASSERT(bufferPtr);
	ASSERT(bufferSize > 0);
	return nctl::makeUnique<MemoryFile>(bufferPtr, bufferSize);
}

nctl::UniquePtr<IFile> IFile::createFromMemory(const char *bufferName, unsigned long int bufferSize)
{
	ASSERT(bufferSize > 0);
	return nctl::makeUnique<MemoryFile>(bufferName, bufferSize);
}

nctl::UniquePtr<IFile> IFile::createFromMemory(unsigned long int bufferSize)
{
	ASSERT(bufferSize > 0);
	return nctl::makeUnique<MemoryFile>(bufferSize);
}

nctl::UniquePtr<IFile> IFile::createFromMemory(const char *bufferName, nctl::UniquePtr<unsigned char []> buffer, unsigned long int bufferSize)
{
	ASSERT(bufferSize > 0);
	return nctl::makeUnique<MemoryFile>(bufferName, nctl::move(buffer), bufferSize);
}

nctl::UniquePtr<IFile> IFile::createFromMemory(nctl::UniquePtr<unsigned char []> buffer, unsigned long int bufferSize)
{
	ASSERT(bufferSize > 0);
	return nctl::makeUnique<MemoryFile>(nctl::move(buffer), bufferSize);
}

nctl::UniquePtr<IFile> IFile::createFileHandle(const char *filename)
{
	ASSERT(filename);
#ifdef __ANDROID__
	const char *assetFilename = AssetFile::assetPath(filename);
	if (assetFilename)
		return nctl::makeUnique<AssetFile>(assetFilename);
	else
#endif
		return nctl::makeUnique<StandardFile>(filename);
}

}
