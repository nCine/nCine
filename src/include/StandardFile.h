#ifndef CLASS_NCINE_STANDARDFILE
#define CLASS_NCINE_STANDARDFILE

#include "IFile.h"

namespace ncine {

/// The class handling opening, reading and closing a standard file
class StandardFile : public IFile
{
  public:
	/// Constructs a standard file object
	/*! \param filename File name including its path */
	explicit StandardFile(const char *filename)
	    : IFile(filename) { type_ = FileType::STANDARD; }
	~StandardFile() override;

	/// Static method to return class type
	inline static FileType sType() { return FileType::STANDARD; }

	/// Tries to open the standard file
	void open(unsigned char mode) override;
	/// Closes the standard file
	void close() override;
	long int seek(long int offset, int whence) const override;
	long int tell() const override;
	unsigned long int read(void *buffer, unsigned long int bytes) const override;
	unsigned long int write(void *buffer, unsigned long int bytes) override;

  private:
	/// Deleted copy constructor
	StandardFile(const StandardFile &) = delete;
	/// Deleted assignment operator
	StandardFile &operator=(const StandardFile &) = delete;

	/// Opens the file with `open()`
	void openFD(unsigned char mode);
	/// Opens the file with `fopen()`
	void openStream(unsigned char mode);
};

}

#endif
