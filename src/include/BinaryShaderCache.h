#ifndef CLASS_NCINE_BINARYSHADERCACHE
#define CLASS_NCINE_BINARYSHADERCACHE

#include <cstdint>
#include <nctl/String.h>
#include <nctl/StaticString.h>
#include <nctl/HashMap.h>

namespace ncine {

/// The class that manages the cache of binary OpenGL shader programs
class BinaryShaderCache
{
  public:
	/// A static string that can holds the contents of the `ShaderFilenameFormat` string
	using ShaderFilename = nctl::StaticString<47>;

	struct ShaderInfo
	{
		/// The filename of the binary shader (it can be generated from the hash and will not be written on disk)
		nctl::StaticString<47> binaryFilename;
		/// A descriptive name for the shader program
		nctl::StaticString<256> objectLabel;
		/// Maximum batch size for this platform (0 if the shader is not batched)
		unsigned int batchSize = -1;
	};

	/// The hash map connects a unique id hash to the `ShaderInfo` structure
	using ShaderInfoHashMapType = nctl::HashMap<uint64_t, ShaderInfo>;

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
	~BinaryShaderCache();

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

	/// Splits a binary shader filename in its platform hash, format, and shader hash components
	bool parseShaderFilename(const char *filename, uint64_t *platformHash, uint32_t *binaryFormat, uint64_t *shaderHash) const;
	/// Returns true if the filename matches the binary shader filename format
	inline bool isShaderFilename(const char *filename) const { return parseShaderFilename(filename, nullptr, nullptr, nullptr); }
	/// Combine the specified shader hash components to form its cache filename
	ShaderFilename formatShaderFilename(uint64_t platformHash, uint32_t binaryFormat, uint64_t shaderHash) const;
	/// Combine the specified shader hash components to form its cache filename (using current platform hash)
	inline ShaderFilename formatShaderFilename(uint32_t binaryFormat, uint64_t shaderHash) const { return formatShaderFilename(platformHash_, binaryFormat, shaderHash); }
	/// Combine the specified shader hash components to form its cache filename (using current platform hash and first available binary format)
	inline ShaderFilename formatShaderFilename(uint64_t shaderHash) const { return formatShaderFilename(platformHash_, binaryFormat_, shaderHash); }

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

	/// Returns true if the specified shader sources id hash has an entry in the shader information hashmap
	bool hasShaderInfo(uint64_t shaderHashName) const;
	/// Returns the shader information for the specified shader sources id hash
	bool retrieveShaderInfo(uint64_t shaderHashName, ShaderInfo &shaderInfo) const;
	/// Registers the information for the shader specified by its shader sources id hash
	bool registerShaderInfo(uint64_t shaderHashName, uint32_t binaryFormat, const char *name, unsigned int batchSize);
	/// Registers the information for the shader specified by its shader sources id hash (using the first available binary format)
	bool registerShaderInfo(uint64_t shaderHashName, const char *name, unsigned int batchSize);
	/// Returns the hashmap that contains the shader information entries (for statistical purposes)
	inline const ShaderInfoHashMapType &shaderInfoHashMap() const { return shaderInfos_; }

	/// Deletes all binary shaders that don't belong to this platform from the cache directory (and the corresponding shader info text file)
	void prune();
	/// Deletes all binary shaders and shader info text files from the cache directory
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

	/// The hash map containing the information for registered shaders
	ShaderInfoHashMapType shaderInfos_;

	/// Initializes the cache the first time it is enabled
	bool initialize();

	/// Scans the cache directory to collect statistics
	void collectStatistics();
	/// Resets all statistics to the initial values
	void clearStatistics();

	/// Loads the shader information file from the cache directory
	bool loadShaderInfoFromCache(uint32_t binaryFormat);
	/// Loads the shader information file from the cache directory (using the first available binary format)
	inline bool loadShaderInfoFromCache() { return loadShaderInfoFromCache(binaryFormat_); }
	/// Saves the shader information file to the cache directory
	bool saveShaderInfoToCache(uint32_t binaryFormat);
	/// Saves the shader information file to the cache directory (using the first available binary format)
	inline bool saveShaderInfoToCache() { return saveShaderInfoToCache(binaryFormat_); }

	/// Recover the platforms hash from a shader information filename
	bool parseShaderInfoFilename(const char *filename, uint64_t *platformHash) const;
	/// Returns true if the filename matches the shader information filename format
	inline bool isShaderInfoFilename(const char *filename) const { return parseShaderInfoFilename(filename, nullptr); }

	/// Parses a single text line in a shader information file
	bool parseShaderInfoLine(const char *buffer, const char **nextLine, uint64_t &shaderHashName, ShaderInfo &shaderInfo) const;

	/// Deleted copy constructor
	BinaryShaderCache(const BinaryShaderCache &) = delete;
	/// Deleted assignment operator
	BinaryShaderCache &operator=(const BinaryShaderCache &) = delete;
};

}

#endif
