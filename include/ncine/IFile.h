#ifndef CLASS_NCINE_IFILE
#define CLASS_NCINE_IFILE

#include <cstdio> // for FILE
#include <cstdint> // for endianness conversions
#include <nctl/String.h>
#include <nctl/UniquePtr.h>

namespace ncine {

/// The interface class dealing with file operations
class DLL_PUBLIC IFile
{
  public:
	/// File types
	enum class FileType
	{
		BASE = 0,
		STANDARD,
		ASSET
	};

	/// Open mode bitmask
	struct OpenMode
	{
		enum
		{
#if !(defined(_WIN32) && !defined(__MINGW32__))
			FD = 1,
#endif
			READ = 2,
			WRITE = 4,
			BINARY = 8
		};
	};

	/// Access mode bitmask
	struct AccessMode
	{
		enum
		{
			EXISTS = 0,
			READABLE = 2,
			WRITABLE = 4
		};
	};

	/// Constructs a base file object
	/*! \param filename File name including its path */
	explicit IFile(const char *filename);
	virtual ~IFile() {}

	/// Returns the file type (RTTI)
	FileType type() const { return type_; }

	/// Tries to open the file
	virtual void open(unsigned char mode) = 0;
	/// Closes the file
	virtual void close() = 0;
	/// Seeks in an opened file
	virtual long int seek(long int offset, int whence) const = 0;
	/// Tells the seek position of an opened file
	virtual long int tell() const = 0;
	/// Reads a certain amount of bytes from the file to a buffer
	/*! \return Number of bytes read */
	virtual unsigned long int read(void *buffer, unsigned long int bytes) const = 0;
	/// Writes a certain amount of bytes from a buffer to the file
	/*! \return Number of bytes written */
	virtual unsigned long int write(void *buffer, unsigned long int bytes) = 0;

	/// Sets the close on destruction flag
	/*! If the flag is true the file is closed upon object destruction. */
	inline void setCloseOnDestruction(bool shouldCloseOnDestruction) { shouldCloseOnDestruction_ = shouldCloseOnDestruction; }
	/// Sets the exit on fail to open flag
	/*! If the flag is true the application exits if the file cannot be opened. */
	inline void setExitOnFailToOpen(bool shouldExitOnFailToOpen) { shouldExitOnFailToOpen_ = shouldExitOnFailToOpen; }
	/// Returns true if the file is already opened
	virtual bool isOpened() const;

	/// Returns file name with path
	const char *filename() const { return filename_.data(); }
	/// Returns file extension
	const char *extension() const { return extension_.data(); }
	/// Checks if file extension matches
	bool hasExtension(const char *extension) const;

	/// Returns file descriptor
	inline int fd() const { return fileDescriptor_; }
	/// Returns file stream pointer
	inline FILE *ptr() const { return filePointer_; }
	/// Returns file size in bytes
	inline long int size() const { return fileSize_; }

	/// Reads a little endian 16 bit unsigned integer
	inline static uint16_t int16FromLE(uint16_t number) { return number; }
	/// Reads a little endian 32 bit unsigned integer
	inline static uint32_t int32FromLE(uint32_t number) { return number; }
	/// Reads a little endian 64 bit unsigned integer
	inline static uint64_t int64FromLE(uint64_t number) { return number; }
	/// Reads a big endian 16 bit unsigned integer
	inline static uint16_t int16FromBE(uint16_t number)
	{
		return (number >> 8) | (number << 8);
	}
	/// Reads a big endian 32 bit unsigned integer
	inline static uint32_t int32FromBE(uint32_t number)
	{
		return (number >> 24) | ((number << 8) & 0x00FF0000) | ((number >> 8) & 0x0000FF00) | (number << 24);
	}
	/// Reads a big endian 64 bit unsigned integer
	inline static uint64_t int64FromBE(uint64_t number)
	{
		return (number >> 56) | ((number << 40) & 0x00FF000000000000ULL) | ((number << 24) & 0x0000FF0000000000ULL) |
		       ((number << 8) & 0x000000FF00000000ULL) | ((number >> 8) & 0x00000000FF000000ULL) |
		       ((number >> 24) & 0x0000000000FF0000ULL) | ((number >> 40) & 0x000000000000FF00ULL) | (number << 56);
	}

	/// Returns the proper file handle according to prepended tags
	static nctl::UniquePtr<IFile> createFileHandle(const char *filename);
	/// Checks if a file can be accessed with the specified mode
	static bool access(const char *filename, unsigned char mode);

	/// Returns the base directory for data loading
	static const nctl::String &dataPath() { return dataPath_; }
	/// Returns the writable directory for saving data
	static const nctl::String &savePath();

  protected:
	/// File type
	FileType type_;

	/// Maximum number of characters for a file name (path included)
	static const unsigned int MaxFilenameLength = 256;
	/// File name with path
	nctl::String filename_;

	/// Maximum number of characters for a file extension, plus '\0'
	static const unsigned int MaxExtensionLength = 5;
	/// File extension
	nctl::String extension_;

	/// File descriptor for `open()` and `close()`
	int fileDescriptor_;
	/// File pointer for `fopen()` and `fclose()`
	FILE *filePointer_;
	/// A flag indicating whether the destructor should also close the file
	/*! Useful for `ov_open()`/`ov_fopen()` and `ov_clear()` functions of the <em>Vorbisfile</em> library. */
	bool shouldCloseOnDestruction_;
	/// A flag indicating whether the application should exit if the file cannot be opened
	/*! Useful for the log file creation. */
	bool shouldExitOnFailToOpen_;

	/// File size in bytes
	long int fileSize_;

  private:
	/// The path for the application to load files from
	static nctl::String dataPath_;
	/// The path for the application to write files into
	static nctl::String savePath_;

	/// Determines the correct save path based on the platform
	static void initSavePath();

	/// The `AppConfiguration` class needs to call `IFile::setDataPath()`
	friend class AppConfiguration;
	/// The `TextureSaverPng` class needs to access the `filePointer_`
	friend class TextureSaverPng;
};

}

#endif
