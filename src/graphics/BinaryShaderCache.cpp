#include <cstdint>
#include <cstdlib> // for `strtoull()`
#include <nctl/HashFunctions.h>
#include <nctl/CString.h>
#include <nctl/StaticString.h>
#include "BinaryShaderCache.h"
#include "IGfxCapabilities.h"
#include "FileSystem.h"
#include "IFile.h"

namespace ncine {

namespace {
	const uint64_t HashSeed = 0x01000193811C9DC5;
	char const * const ShaderFilenameFormat = "%016llx_%08x_%016llx.bin";

	unsigned int bufferSize = 0;
	nctl::UniquePtr<uint8_t[]> bufferPtr;

	nctl::String shaderFilename(64);
	nctl::String shaderPath(256);
	char componentString[17] = "\0";
}

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

BinaryShaderCache::BinaryShaderCache(bool enable, const char *dirname)
    : isAvailable_(false), isInitialized_(false), isEnabled_(false), platformHash_(0)
{
	const IGfxCapabilities &gfxCaps = theServiceLocator().gfxCapabilities();
	const bool isSupported = gfxCaps.hasExtension(IGfxCapabilities::GLExtensions::ARB_GET_PROGRAM_BINARY) &&
	                         gfxCaps.value(IGfxCapabilities::GLIntValues::NUM_PROGRAM_BINARY_FORMATS) > 0;

	if (isSupported)
	{
		const bool cacheDirWriteable = fs::isDirectory(fs::cachePath().data()) && fs::isWritable(fs::cachePath().data());

		directory_ = fs::joinPath(fs::cachePath(), dirname);
		isAvailable_ = (isSupported && cacheDirWriteable);

		if (isAvailable_ && enable)
			initialize();
	}
	else
		LOGW_X("The get_program_binary extension is not supported, the binary shader cache is not enabled");

	isEnabled_ = enable && isAvailable_;
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void BinaryShaderCache::setEnabled(bool enabled)
{
	if (enabled && isAvailable_)
		initialize();

	isEnabled_ = enabled;
}

uint64_t BinaryShaderCache::hashSources(unsigned int count, const char **strings, const int *lengths) const
{
	uint64_t hash = 0;
	for (unsigned int i = 0; i < count; i++)
	{
		ASSERT(strings[i] != nullptr);
		ASSERT(lengths[i] > 0);
		if (strings[i] != nullptr && lengths[i] > 0)
		{
			const unsigned int length = static_cast<unsigned int>(lengths[i]);
			// Align the length to 64 bits, plus 7 bytes read after that
			const unsigned int paddedLength = length + 8 - (length % 8) + 7;
			// Recycling the loading binary buffer for zero-padded shader sources
			if (bufferSize < paddedLength)
			{
				bufferSize = paddedLength;
				bufferPtr = nctl::makeUnique<uint8_t[]>(bufferSize);
			}

			for (unsigned int j = 0; j < length; j++)
				bufferPtr[j] = strings[i][j];
			// Set padding bytes to zero for a deterministic hash
			for (unsigned int j = length; j < paddedLength; j++)
				bufferPtr[j] = '\0';

			hash += nctl::fasthash64(bufferPtr.get(), length, HashSeed);
		}
	}

	return hash;
}

unsigned int BinaryShaderCache::binarySize(uint32_t binaryFormat, uint64_t hash) const
{
	if (isEnabled_ == false || isAvailable_ == false)
		return 0;

	shaderFilename.format(ShaderFilenameFormat, platformHash_, binaryFormat, hash);
	shaderPath = fs::joinPath(directory_, shaderFilename);

	unsigned int fileSize = 0;
	if (fs::isFile(shaderPath.data()))
		fileSize = fs::fileSize(shaderPath.data());

	return fileSize;
}

const void *BinaryShaderCache::loadFromCache(uint32_t binaryFormat, uint64_t hash)
{
	if (isEnabled_ == false || isAvailable_ == false)
		return nullptr;

	bool fileRead = false;
	shaderFilename.format(ShaderFilenameFormat, platformHash_, binaryFormat, hash);
	shaderPath = fs::joinPath(directory_, shaderFilename);
	if (fs::isReadableFile(shaderPath.data()))
	{
		nctl::UniquePtr<IFile> fileHandle = IFile::createFileHandle(shaderPath.data());
		fileHandle->open(IFile::OpenMode::READ | IFile::OpenMode::BINARY);
		if (fileHandle->isOpened())
		{
			const long int fileSize = fileHandle->size();
			if (bufferSize < fileSize)
			{
				bufferSize = fileSize;
				bufferPtr = nctl::makeUnique<uint8_t[]>(bufferSize);
			}

			fileHandle->read(bufferPtr.get(), fileSize);
			fileHandle->close();

			LOGI_X("Loaded binary shader \"%s\" from cache", shaderFilename.data());
			statistics_.LoadedShaders++;
			fileRead = true;
		}
	}

	return (fileRead) ? bufferPtr.get() : nullptr;
}

bool BinaryShaderCache::saveToCache(int length, const void *buffer, uint32_t binaryFormat, uint64_t hash)
{
	if (isEnabled_ == false || isAvailable_ == false || length <= 0)
		return false;

	bool fileWritten = false;
	shaderFilename.format(ShaderFilenameFormat, platformHash_, binaryFormat, hash);
	shaderPath = fs::joinPath(directory_, shaderFilename);
	if (fs::isFile(shaderPath.data()) == false)
	{
		nctl::UniquePtr<IFile> fileHandle = IFile::createFileHandle(shaderPath.data());
		fileHandle->open(IFile::OpenMode::WRITE | IFile::OpenMode::BINARY);
		if (fileHandle->isOpened())
		{
			fileHandle->write(buffer, length);
			fileHandle->close();

			LOGI_X("Saved binary shader \"%s\" to cache", shaderFilename.data());
			statistics_.SavedShaders++;
			statistics_.PlatformFilesCount++;
			statistics_.PlatformBytesCount += length;
			statistics_.TotalFilesCount++;
			statistics_.TotalBytesCount += length;
			fileWritten = true;
		}
	}

	return fileWritten;
}

void BinaryShaderCache::prune()
{
	uint64_t platformHash = 0;

	fs::Directory dir(directory_.data());
	while (const char *entryName = dir.readNext())
	{
		if (parseShaderFilename(entryName, &platformHash, nullptr, nullptr))
		{
			// Deleting only binary shaders with different platform hashes
			if (platformHash != platformHash_)
			{
				shaderPath = fs::joinPath(directory_, entryName);
				const long int fileSize = fs::fileSize(shaderPath.data());

				ASSERT(statistics_.TotalFilesCount > 0);
				ASSERT(statistics_.TotalBytesCount >= fileSize);
				fs::deleteFile(shaderPath.data());
				statistics_.TotalFilesCount--;
				statistics_.TotalBytesCount -= fileSize;
			}
		}
	}
}

void BinaryShaderCache::clear()
{
	fs::Directory dir(directory_.data());
	while (const char *entryName = dir.readNext())
	{
		// Deleting all binary shaders
		if (isShaderFilename(entryName))
		{
			shaderPath = fs::joinPath(directory_, entryName);
			fs::deleteFile(shaderPath.data());
		}
	}

	clearStatistics();
}

/*! \return True if the path is a writable directory */
bool BinaryShaderCache::setDirectory(const char *dirPath)
{
	if (fs::isDirectory(dirPath) && fs::isWritable(dirPath))
	{
		directory_ = dirPath;
		collectStatistics();
		return true;
	}
	return false;
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

bool BinaryShaderCache::initialize()
{
	if (isAvailable_ == false || isInitialized_)
		return isInitialized_;

	// Check the cache directory existence before attempting the initialization
	const bool dirExistedAlready = fs::isDirectory(directory_.data());
	if (dirExistedAlready == false)
		fs::createDir(directory_.data());

	const bool dirExistsNow = fs::isDirectory(directory_.data());
	isAvailable_ = (isAvailable_ && dirExistsNow);

	if (isAvailable_)
	{
		const IGfxCapabilities &gfxCaps = theServiceLocator().gfxCapabilities();
		const IGfxCapabilities::GlInfoStrings &infoStrings = gfxCaps.glInfoStrings();

		// For a stable hash, the OpenGL strings need to be copied so that padding bytes can be set to zero
		nctl::StaticString<512> platformString;

		// 1) `GL_RENDERER` string
		platformString.assign(reinterpret_cast<const char *>(infoStrings.renderer), platformString.capacity());

		unsigned int paddedLength = platformString.length() + 8 - (platformString.length() % 8) + 7;
		// Set padding bytes to zero for a deterministic hash
		for (unsigned int i = platformString.length(); i < paddedLength; i++)
			platformString.data()[i] = '\0';

		platformHash_ += nctl::fasthash64(platformString.data(), platformString.length(), HashSeed);

		// 2) `GL_VERSION` string
		platformString.assign(reinterpret_cast<const char *>(infoStrings.glVersion), platformString.capacity());

		paddedLength = platformString.length() + 8 - (platformString.length() % 8) + 7;
		// Set padding bytes to zero for a deterministic hash
		for (unsigned int i = platformString.length(); i < paddedLength; i++)
			platformString.data()[i] = '\0';

		platformHash_ += nctl::fasthash64(platformString.data(), platformString.length(), HashSeed);

		// Always collect statistics after having calculated the platform hash
		if (dirExistedAlready)
			collectStatistics();

		bufferSize = 64 * 1024;
		bufferPtr = nctl::makeUnique<uint8_t[]>(bufferSize);
		isInitialized_ = true;
	}
	return isInitialized_;
}

void BinaryShaderCache::collectStatistics()
{
	ASSERT(platformHash_ != 0);

	clearStatistics();
	uint64_t platformHash = 0;

	fs::Directory dir(directory_.data());
	while (const char *entryName = dir.readNext())
	{
		if (parseShaderFilename(entryName, &platformHash, nullptr, nullptr))
		{
			shaderPath = fs::joinPath(directory_, entryName);
			const long int fileSize = fs::fileSize(shaderPath.data());

			if (platformHash == platformHash_)
			{
				statistics_.PlatformFilesCount++;
				statistics_.PlatformBytesCount += fileSize;
			}
			statistics_.TotalFilesCount++;
			statistics_.TotalBytesCount += fileSize;
		}
	}
	dir.close();
}

void BinaryShaderCache::clearStatistics()
{
	statistics_.LoadedShaders = 0;
	statistics_.SavedShaders = 0;
	statistics_.PlatformFilesCount = 0;
	statistics_.PlatformBytesCount = 0;
	statistics_.TotalFilesCount = 0;
	statistics_.TotalBytesCount = 0;
}

bool BinaryShaderCache::parseShaderFilename(const char *filename, uint64_t *platformHash, uint32_t *binaryFormat, uint64_t *shaderHash) const
{
	// The length of a binary shader filename is: 16 + "_" + 8 + "_" + 16 + ".bin"
	if (nctl::strnlen(filename, 64) != 46)
		return false;

	if (fs::hasExtension(filename, "bin") == false || filename[16] != '_' || filename[25] != '_')
		return false;

	for (unsigned int i = 0; i < 16; i++)
	{
		const char c = filename[i];
		// Check if the platform component is an hexadecimal number
		if (!(c >= '0' && c <= '9') && !(c >= 'A' && c <= 'F') && !(c >= 'a' && c <= 'f'))
			return false;
	}

	for (unsigned int i = 0; i < 8; i++)
	{
		const char c = filename[i + 17];
		// Check if the format component is an hexadecimal number
		if (!(c >= '0' && c <= '9') && !(c >= 'A' && c <= 'F') && !(c >= 'a' && c <= 'f'))
			return false;
	}

	for (unsigned int i = 0; i < 16; i++)
	{
		const char c = filename[i + 26];
		// Check if the shader hash component is an hexadecimal number
		if (!(c >= '0' && c <= '9') && !(c >= 'A' && c <= 'F') && !(c >= 'a' && c <= 'f'))
			return false;
	}

	componentString[16] = '\0';
	if (platformHash != nullptr)
	{
		for (unsigned int i = 0; i < 16; i++)
			componentString[i] = filename[i];
		*platformHash = strtoull(componentString, nullptr, 16);
	}

	componentString[8] = '\0';
	if (binaryFormat != nullptr)
	{
		for (unsigned int i = 0; i < 8; i++)
			componentString[i] = filename[i + 17];
		*binaryFormat = strtoul(componentString, nullptr, 16);
	}

	if (shaderHash != nullptr)
	{
		for (unsigned int i = 0; i < 16; i++)
			componentString[i] = filename[i + 26];
		*shaderHash = strtoull(componentString, nullptr, 16);
	}

	return true;
}

}
