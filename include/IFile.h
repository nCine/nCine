#ifndef CLASS_NCINE_IFILE
#define CLASS_NCINE_IFILE

#include <cstdio> // for FILE
#include <stdint.h> // for endianness conversions
#include "ncString.h"

namespace ncine {

/// The interface dealing with file operations
class DLL_PUBLIC IFile
{
  public:
	/// The enumeration of file types
	enum FileType
	{
		BASE_TYPE = 0,
		STANDARD_TYPE,
		ASSET_TYPE
	};

	/// The enumeration for the open mode bitmask
	enum OpenMode
	{
#if !(defined(_WIN32) && !defined(__MINGW32__))
		MODE_FD = 1,
#endif
		MODE_READ = 2,
		MODE_WRITE = 4,
		MODE_BINARY = 8
	};

	/// The enumeration for the access mode bitmask
	enum AccessMode
	{
		MODE_EXISTS = 0,
		MODE_CAN_READ = 2,
		MODE_CAN_WRITE = 4
	};

	/// Constructs a base file object
	/*! \param filename File name including its path */
	explicit IFile(const char *filename);
	virtual ~IFile() { }

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
	/// Read a certain amount of bytes from the file to a buffer
	/*! \return Number of bytes read */
	virtual unsigned long int read(void *buffer, unsigned long int bytes) const = 0;

	/// Sets the close on destruction flag
	inline void setCloseOnDestruction(bool shouldCloseOnDestruction) { shouldCloseOnDestruction_ = shouldCloseOnDestruction; }
	/// Sets the exit on open fail
	inline void setExitOnFailToOpen(bool shouldExitOnFailToOpen) { shouldExitOnFailToOpen_ = shouldExitOnFailToOpen; }
	// Returns true if the file is already opened
	virtual bool isOpened() const;

	/// Returns file name with path
	const char* filename() const { return filename_.data(); }
	/// Returns file extension
	const char* extension() const { return extension_.data(); }
	// Checks if file extension matches
	bool hasExtension(const char *extension) const;

	/// Returns file descriptor
	inline int fd() const { return fileDescriptor_; }
	/// Returns file stream pointer
	inline FILE* ptr() const { return filePointer_; }
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

	// Returns the proper file handle according to prepended tags
	static IFile* createFileHandle(const char *filename);
	// Checks if a file can be accessed with specified mode
	static bool access(const char *filename, unsigned char mode);

	/// Returns the base directory for data loading
	static const String& dataPath() { return dataPath_; }
	// Returns the writable directory for saving data
	static const String& savePath();

  protected:
	/// File type
	FileType type_;

	/// Maximum number of characters for a file name (path included)
	static const unsigned int MaxFilenameLength = 256;
	/// File name with path
	String filename_;

	/// Maximum number of characters for a file extension, plus '\0'
	static const unsigned int MaxExtensionLength = 5;
	/// File extension
	String extension_;

	/// File descriptor for `open()` and `close()`
	int fileDescriptor_;
	/// File pointer for `fopen()` and `fclose()`
	FILE *filePointer_;
	/// Should the destructor close the file?
	/*! Useful for `ov_open()`/`ov_fopen()` and `ov_clear()` */
	bool shouldCloseOnDestruction_;
	/// Should the application exit if the file cannot be opened?
	/*! Useful for the log file creation */
	bool shouldExitOnFailToOpen_;

	/// File size in bytes
	long int fileSize_;

  private:
	/// The path for the application to load files from
	static String dataPath_;
	/// The path for the application to write files into
	static String savePath_;

	static void initSavePath();

	friend class AppConfiguration; // needs to call IFile::setDataPath()
};

}

#endif
