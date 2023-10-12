#ifndef CLASS_NCINE_BINARYSHADERCACHE
#define CLASS_NCINE_BINARYSHADERCACHE

#include <cstdint>
#include <nctl/String.h>

namespace ncine {

/// The class that manages the cache of binary OpenGL shader programs
class BinaryShaderCache
{
  public:
	/// The statistics about the cache and its requests
	struct Statistics
	{
		unsigned int LoadedShaders = 0;
		unsigned int SavedShaders = 0;
		unsigned int PlatformFilesCount = 0;
		unsigned int PlatformBytesCount = 0;
		unsigned int TotalFilesCount = 0;
		unsigned int TotalBytesCount = 0;
	};

	BinaryShaderCache(bool enable, const char *dirname);

	/// Returns true if the binary shader cache is supported and can be enabled
	inline bool isAvailable() const { return isAvailable_; }
	/// Returns true if the binary shader cache is currently enabled
	inline bool isEnabled() const { return isEnabled_; }
	/// Enables or disables the binary shader cache (it can be enabled only if available)
	void setEnabled(bool enabled);

	/// Returns the hash for the current platform
	inline uint64_t platformHash() const { return platformHash_; }
	/// Returns the symbolic constant of the first available binary format
	inline uint32_t binaryFormat() const { return binaryFormat_; }
	/// Returns a hash number by hashing all characters of the given source strings
	uint64_t hashSources(unsigned int count, const char **strings, const int *lengths) const;

	/// Returns the size in bytes of the binary shader from the cache with the given format and hash id
	unsigned int binarySize(uint32_t binaryFormat, uint64_t hash) const;
	/// Returns the size in bytes of the binary shader from the cache with the first available format and the given hash id
	inline unsigned int binarySize(uint64_t hash) const { return binarySize(binaryFormat_, hash); }
	/// Loads a binary shader from the cache with the given format and hash id
	const void *loadFromCache(uint32_t binaryFormat, uint64_t hash);
	/// Loads a binary shader from the cache with the first available format and the given hash id
	inline const void *loadFromCache(uint64_t hash) { return loadFromCache(binaryFormat_, hash); }
	/// Saves a binary shader to the cache with the given format and hash id
	bool saveToCache(int length, const void *buffer, uint32_t binaryFormat, uint64_t hash);
	/// Saves a binary shader to the cache with the first available format and the given hash id
	inline bool saveToCache(int length, const void *buffer, uint64_t hash) { return saveToCache(length, buffer, binaryFormat_, hash); }

	/// Deletes all binary shaders that don't belong to this platform from the cache directory
	void prune();
	/// Deletes all binary shaders from the cache directory
	void clear();

	/// Returns the statistics about the files in the cache
	inline const Statistics &statistics() const { return statistics_; }

	/// Returns the current cache directory for binary shaders
	inline const nctl::String &directory() const { return directory_; }
	/// Sets a new directory as the cache for binary shaders
	bool setDirectory(const char *dirPath);

  private:
	/// A flag that indicates that the OpenGL context supports binary shaders and the cache is available
	bool isAvailable_;
	/// A flag that indicates that the binary shader cache has been already initialized
	bool isInitialized_;
	/// A flag that indicates that the binary shader cache is enabled and should be used if available
	bool isEnabled_;

	/// The first symbolic constant in the `GL_PROGRAM_BINARY_FORMATS` list
	uint32_t binaryFormat_;

	/// The hash value that identifies a specific OpenGL platform
	uint64_t platformHash_;

	/// The cache directory containing the binary shaders
	nctl::String directory_;

	/// The statistics structure about the files in the cache
	Statistics statistics_;

	/// Initializes the cache the first time it is enabled
	bool initialize();

	/// Scans the cache directory to collect statistics
	void collectStatistics();
	/// Resets all statistics to the initial values
	void clearStatistics();

	/// Returns true if the filename matches the binary shader filename format
	inline bool isShaderFilename(const char *filename) const { return parseShaderFilename(filename, nullptr, nullptr, nullptr); }

	/// Splits a binary shader filename in its platform hash, format, and shader hash components
	bool parseShaderFilename(const char *filename, uint64_t *platformHash, uint32_t *binaryFormat, uint64_t *shaderHash) const;

	/// Deleted copy constructor
	BinaryShaderCache(const BinaryShaderCache &) = delete;
	/// Deleted assignment operator
	BinaryShaderCache &operator=(const BinaryShaderCache &) = delete;
};

}

#endif
