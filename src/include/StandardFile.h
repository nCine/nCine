#ifndef CLASS_NCINE_STANDARDFILE
#define CLASS_NCINE_STANDARDFILE

#include "IFile.h"

namespace ncine {

/// The class dealing with file open/close, its path and extension
class StandardFile: public IFile
{
  public:
	/// Constructs a standard file object
	/*! \param pFilename File name including its path */
	StandardFile(const char *filename) : IFile(filename) { type_ = STANDARD_TYPE; }
	~StandardFile();

	/// Static method to return class type
	inline static FileType sType() { return STANDARD_TYPE; }

	void open(unsigned char mode);
	void close();
	long int seek(long int offset, int whence) const;
	long int tell() const;
	long int read(void *buffer, int bytes) const;

  private:
	/// Private copy constructor
	StandardFile(const StandardFile&);
	/// Private assignment operator
	StandardFile& operator=(const StandardFile&);

	// Opens the file with open()
	void openFD(unsigned char mode);
	// Opens the file with fopen()
	void openStream(unsigned char mode);

	// Checks if a file can be accessed with specified mode
	static bool access(const char *filename, unsigned char mode);

	friend bool IFile::access(const char *filename, unsigned char mode);
};

}

#endif
