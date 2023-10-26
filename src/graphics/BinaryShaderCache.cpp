#include <cstdint>
#include <cstdlib> // for `strtoull()`
#include <nctl/CString.h>
#include <nctl/HashMapIterator.h>
#include "BinaryShaderCache.h"
#include "IGfxCapabilities.h"
#include "FileSystem.h"
#include "IFile.h"
#include "Hash64.h"

namespace ncine {

namespace {
	char const * const ShaderFilenameFormat = "%016llx_%08x_%016llx.bin";
	char const * const ShaderInfoFilenameFormat = "%016llx_%08x_shaderInfo.txt";

	unsigned int bufferSize = 0;
	nctl::UniquePtr<uint8_t[]> bufferPtr;

	nctl::String fileBaseName(64);
	nctl::String filePath(fs::MaxPathLength);
	char componentString[17] = "\0";
}

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

BinaryShaderCache::BinaryShaderCache(bool enable, const char *dirname)
    : isAvailable_(false), isInitialized_(false), isEnabled_(false), binaryFormat_(0), platformHash_(0), shaderInfos_(64)
{
	const IGfxCapabilities &gfxCaps = theServiceLocator().gfxCapabilities();
	const bool isSupported = gfxCaps.hasExtension(IGfxCapabilities::GLExtensions::ARB_GET_PROGRAM_BINARY) &&
	                         gfxCaps.value(IGfxCapabilities::GLIntValues::NUM_PROGRAM_BINARY_FORMATS) > 0;

	if (isSupported)
	{
		// Initialize with the first available program binary format
		binaryFormat_ = static_cast<uint32_t>(gfxCaps.arrayValue(IGfxCapabilities::GLArrayIntValues::PROGRAM_BINARY_FORMATS, 0));

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

BinaryShaderCache::~BinaryShaderCache()
{
	saveShaderInfoToCache();
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

BinaryShaderCache::ShaderFilename BinaryShaderCache::formatShaderFilename(uint64_t platformHash, uint32_t binaryFormat, uint64_t shaderHash) const
{
	ShaderFilename filename;
	filename.format(ShaderFilenameFormat, platformHash, binaryFormat, shaderHash);
	return filename;
}

unsigned int BinaryShaderCache::binarySize(uint32_t binaryFormat, uint64_t hash) const
{
	if (isEnabled_ == false || isAvailable_ == false)
		return 0;

	fileBaseName.format(ShaderFilenameFormat, platformHash_, binaryFormat, hash);
	filePath = fs::joinPath(directory_, fileBaseName);

	unsigned int fileSize = 0;
	if (fs::isFile(filePath.data()))
		fileSize = fs::fileSize(filePath.data());

	return fileSize;
}

const void *BinaryShaderCache::loadFromCache(uint32_t binaryFormat, uint64_t hash)
{
	if (isEnabled_ == false || isAvailable_ == false)
		return nullptr;

	bool fileRead = false;
	fileBaseName.format(ShaderFilenameFormat, platformHash_, binaryFormat, hash);
	filePath = fs::joinPath(directory_, fileBaseName);
	if (fs::isReadableFile(filePath.data()))
	{
		nctl::UniquePtr<IFile> fileHandle = IFile::createFileHandle(filePath.data());
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

			LOGI_X("Loaded binary shader \"%s\" from cache", fileBaseName.data());
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
	fileBaseName.format(ShaderFilenameFormat, platformHash_, binaryFormat, hash);
	filePath = fs::joinPath(directory_, fileBaseName);
	if (fs::isFile(filePath.data()) == false)
	{
		nctl::UniquePtr<IFile> fileHandle = IFile::createFileHandle(filePath.data());
		fileHandle->open(IFile::OpenMode::WRITE | IFile::OpenMode::BINARY);
		if (fileHandle->isOpened())
		{
			fileHandle->write(buffer, length);
			fileHandle->close();

			LOGI_X("Saved binary shader \"%s\" to cache", fileBaseName.data());
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

bool BinaryShaderCache::hasShaderInfo(uint64_t shaderHashName) const
{
	if (isEnabled_ == false || isAvailable_ == false)
		return false;

	const ShaderInfo *shaderInfo = shaderInfos_.find(shaderHashName);
	return (shaderInfo != nullptr);
}

bool BinaryShaderCache::retrieveShaderInfo(uint64_t shaderHashName, ShaderInfo &shaderInfo) const
{
	if (isEnabled_ == false || isAvailable_ == false)
		return false;

	const bool contains = shaderInfos_.contains(shaderHashName, shaderInfo);
	return contains;
}

bool BinaryShaderCache::registerShaderInfo(uint64_t shaderHashName, uint32_t binaryFormat, const char *name, unsigned int batchSize)
{
	if (isEnabled_ == false || isAvailable_ == false)
		return false;

	fileBaseName.format(ShaderFilenameFormat, platformHash_, binaryFormat, shaderHashName);
	filePath = fs::joinPath(directory_, fileBaseName);

	bool inserted = false;
	if (fs::isReadableFile(filePath.data()))
	{
		ShaderInfo shaderInfo;
		shaderInfo.binaryFilename = fileBaseName.data();
		shaderInfo.objectLabel = name;
		shaderInfo.batchSize = batchSize;

		if (shaderInfos_.loadFactor() >= 0.8f)
			shaderInfos_.rehash(shaderInfos_.capacity() * 2);
		inserted = shaderInfos_.insert(shaderHashName, shaderInfo);

		if (inserted)
		{
			LOGI_X("Registering shader \"%s\" (0x%016llx) as binary file \"%s\" with a batch size of %u",
			       name, shaderHashName, fileBaseName.data(), batchSize);
		}
	}

	return inserted;
}

bool BinaryShaderCache::registerShaderInfo(uint64_t shaderHashName, const char *name, unsigned int batchSize)
{
	return registerShaderInfo(shaderHashName, binaryFormat_, name, batchSize);
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
				filePath = fs::joinPath(directory_, entryName);
				const long int fileSize = fs::fileSize(filePath.data());

				ASSERT(statistics_.TotalFilesCount > 0);
				ASSERT(statistics_.TotalBytesCount >= fileSize);
				fs::deleteFile(filePath.data());
				statistics_.TotalFilesCount--;
				statistics_.TotalBytesCount -= fileSize;
			}
		}

		if (parseShaderInfoFilename(entryName, &platformHash))
		{
			// Deleting only shader information files with different platform hashes
			if (platformHash != platformHash_)
			{
				filePath = fs::joinPath(directory_, entryName);
				fs::deleteFile(filePath.data());
			}
		}
	}
}

void BinaryShaderCache::clear()
{
	fs::Directory dir(directory_.data());
	while (const char *entryName = dir.readNext())
	{
		// Deleting all binary shaders and shader information files
		if (isShaderFilename(entryName) || isShaderInfoFilename(entryName))
		{
			filePath = fs::joinPath(directory_, entryName);
			fs::deleteFile(filePath.data());
		}
	}

	clearStatistics();
	shaderInfos_.clear();
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

		// Platform hashing statistics are not counted in shaders hashes (`RenderResources::hash64()`)
		platformHash_ += hash64().hashString(platformString.data(), platformString.length());

		// 2) `GL_VERSION` string
		platformString.assign(reinterpret_cast<const char *>(infoStrings.glVersion), platformString.capacity());

		paddedLength = platformString.length() + 8 - (platformString.length() % 8) + 7;
		// Set padding bytes to zero for a deterministic hash
		for (unsigned int i = platformString.length(); i < paddedLength; i++)
			platformString.data()[i] = '\0';

		// Platform hashing statistics are not counted in shaders hashes (`RenderResources::hash64()`)
		platformHash_ += hash64().hashString(platformString.data(), platformString.length());

		// Always collect statistics after having calculated the platform hash
		if (dirExistedAlready)
			collectStatistics();

		loadShaderInfoFromCache();

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
			filePath = fs::joinPath(directory_, entryName);
			const long int fileSize = fs::fileSize(filePath.data());

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

bool BinaryShaderCache::loadShaderInfoFromCache(uint32_t binaryFormat)
{
	if (isAvailable_ == false)
		return false;

	bool fileRead = false;
	fileBaseName.format(ShaderInfoFilenameFormat, platformHash_, binaryFormat);
	filePath = fs::joinPath(directory_, fileBaseName);
	if (fs::isReadableFile(filePath.data()))
	{
		nctl::UniquePtr<IFile> fileHandle = IFile::createFileHandle(filePath.data());
		fileHandle->open(IFile::OpenMode::READ);
		if (fileHandle->isOpened())
		{
			const long int fileSize = fileHandle->size();
			nctl::UniquePtr<char[]> shaderInfoFileBuffer = nctl::makeUnique<char[]>(fileSize);

			fileHandle->read(shaderInfoFileBuffer.get(), fileSize);
			fileHandle->close();

			int parsedEntries = 0;
			int insertedEntries = 0;
			const char *start = shaderInfoFileBuffer.get();
			char const * const end = start + fileSize;

			uint64_t shaderHashName = 0;
			ShaderInfo shaderInfo;
			while (start < end)
			{
				const bool entryParsed = parseShaderInfoLine(start, &start, shaderHashName, shaderInfo);
				if (entryParsed)
				{
					parsedEntries++;

					// Generate the binary shader filename from its shader hash name
					shaderInfo.binaryFilename.format(ShaderFilenameFormat, platformHash_, binaryFormat, shaderHashName);

					// Insert in the hashmap only if the binary shader file exists
					if (fs::isReadableFile(fs::joinPath(directory_, shaderInfo.binaryFilename.data()).data()))
					{
						LOGD_X("Shader information entry (file found): \"%s\", \"%s\", %d",
						       shaderInfo.binaryFilename.data(), shaderInfo.objectLabel.data(), shaderInfo.batchSize);

						if (shaderInfos_.loadFactor() >= 0.8f)
							shaderInfos_.rehash(shaderInfos_.capacity() * 2);
						const bool inserted = shaderInfos_.insert(shaderHashName, shaderInfo);
						insertedEntries = inserted ? insertedEntries + 1 : insertedEntries;
					}
					else
						LOGW_X("Shader information entry (file not found): \"%s\", \"%s\", %d",
						       shaderInfo.binaryFilename.data(), shaderInfo.objectLabel.data(), shaderInfo.batchSize);
				}
			}
			LOGI_X("Loaded binary shader information text \"%s\" from cache (%u entries parsed and %u inserted)", fileBaseName.data(), parsedEntries, insertedEntries);
			fileRead = (parsedEntries > 0);
		}
	}

	return fileRead;
}

bool BinaryShaderCache::saveShaderInfoToCache(uint32_t binaryFormat)
{
	if (isEnabled_ == false || isAvailable_ == false || shaderInfos_.isEmpty())
		return false;

	bool fileWritten = false;
	fileBaseName.format(ShaderInfoFilenameFormat, platformHash_, binaryFormat);
	filePath = fs::joinPath(directory_, fileBaseName);

	nctl::UniquePtr<IFile> fileHandle = IFile::createFileHandle(filePath.data());
	// The file is recreated from scratch all the times, even if it existed already
	fileHandle->open(IFile::OpenMode::WRITE);
	if (fileHandle->isOpened())
	{
		nctl::String shaderInfoLines(256 * shaderInfos_.size());
		for (ShaderInfoHashMapType::ConstIterator i = shaderInfos_.begin(); i != shaderInfos_.end(); ++i)
		{
			// Only the shader hash name is saved, the binary shader filename will be generated on load
			shaderInfoLines.formatAppend("%016llx,%s,%u\n", i.key(), i.value().objectLabel.data(), i.value().batchSize);
		}

		fileHandle->write(shaderInfoLines.data(), shaderInfoLines.length());
		fileHandle->close();
		LOGI_X("Saved binary shader information text \"%s\" to cache", fileBaseName.data());
		fileWritten = true;
	}

	return fileWritten;
}

void trimSpaces(const char **start, const char **end)
{
	while (**start == ' ' || **start == '\t')
		(*start)++;

	(*end)--;
	while (**end == ' ' || **end == '\t')
		(*end)--;
	(*end)++;
}

bool BinaryShaderCache::parseShaderInfoFilename(const char *filename, uint64_t *platformHash) const
{
	// The length of a shader info filename is: 16 + "_" + 8 + "_" + "shaderInfo.txt"
	if (nctl::strnlen(filename, 64) != 40)
		return false;

	if (fs::hasExtension(filename, "txt") == false || filename[16] != '_' || filename[25] != '_')
		return false;

	if (filename[26] != 's' || filename[27] != 'h' || filename[28] != 'a' || filename[29] != 'd' ||
	    filename[30] != 'e' || filename[31] != 'r' || filename[32] != 'I' || filename[33] != 'n' ||
	    filename[34] != 'f' || filename[35] != 'o')
	{
		return false;
	}

	for (unsigned int i = 0; i < 16; i++)
	{
		const char c = filename[i];
		// Check if the platform component is an hexadecimal number
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

	return true;
}

bool BinaryShaderCache::parseShaderInfoLine(const char *buffer, const char **nextLine, uint64_t &shaderHashName, ShaderInfo &shaderInfo) const
{
	bool entryParsed = false;

	const char *lineStart = buffer;
	const char *lineEnd = lineStart;
	// Searching for the new line character
	while (*lineEnd != '\n' && *lineEnd != '\0')
		lineEnd++;

	const unsigned int numCommas = 2;
	const char *commas[numCommas];
	for (unsigned int i = 0; i < numCommas; i++)
	{
		commas[i] = (i > 0) ? commas[i - 1] + 1 : lineStart;
		while (*commas[i] != ',' && commas[i] < lineEnd)
			commas[i]++;
	}

	const unsigned int numFields = numCommas + 1;
	const char *fieldStart[numFields];
	const char *fieldEnd[numFields];
	for (unsigned int i = 0; i < numFields; i++)
	{
		fieldStart[i] = (i > 0) ? commas[i - 1] + 1 : lineStart;
		fieldEnd[i] = (i < numFields - 1) ? commas[i] - 1 : lineEnd;
		trimSpaces(&fieldStart[i], &fieldEnd[i]);
	}

	unsigned int fieldLength[numFields];
	for (unsigned int i = 0; i < numFields; i++)
		fieldLength[i] = fieldEnd[i] - fieldStart[i] + 1;

	shaderHashName = strtoull(fieldStart[0], nullptr, 16);
	shaderInfo.objectLabel.assign(fieldStart[1], fieldLength[1]);
	const int numScanned = sscanf(fieldStart[2], "%5u", &shaderInfo.batchSize);

	if (fieldLength[0] > 0 && fieldLength[1] > 0 && fieldLength[2] > 0 && numScanned == 1)
		entryParsed = true;
	*nextLine = lineEnd + 1;

	return entryParsed;
}

}
