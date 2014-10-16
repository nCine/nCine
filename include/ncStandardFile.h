#ifndef CLASS_NCSTANDARDFILE
#define CLASS_NCSTANDARDFILE

#include "ncIFile.h"

/// The class dealing with file open/close, its path and extension
class ncStandardFile: public ncIFile
{
  public:
	/// Constructs a standard file object
	/*! \param pFilename File name including its path */
	ncStandardFile(const char *filename) : ncIFile(filename) { type_ = STANDARD_TYPE; }
	~ncStandardFile();

	/// Static method to return class type
	inline static FileType sType() { return STANDARD_TYPE; }

	void open(unsigned char mode);
	void close();
	long int seek(long int offset, int whence) const;
	long int tell() const;
	long int read(void *buffer, int bytes) const;

  private:
	// Opens the file with open()
	void openFd(unsigned char mode);
	// Opens the file with fopen()
	void openStream(unsigned char mode);

	// Checks if a file can be accessed with specified mode
	static bool access(const char *filename, unsigned char mode);

	friend bool ncIFile::access(const char *filename, unsigned char mode);
};

#endif
