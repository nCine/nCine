#ifndef CLASS_NCINE_MEMORYFILE
#define CLASS_NCINE_MEMORYFILE

#include "IFile.h"

namespace ncine {

/// The class creating a file interface around a memory buffer
class MemoryFile : public IFile
{
  public:
	MemoryFile(const char *bufferName, unsigned char *bufferPtr, unsigned long int bufferSize);
	MemoryFile(const char *bufferName, const unsigned char *bufferPtr, unsigned long int bufferSize);
	MemoryFile(unsigned char *bufferPtr, unsigned long int bufferSize);
	MemoryFile(const unsigned char *bufferPtr, unsigned long int bufferSize);

	void open(unsigned char mode) override;
	void close() override;
	long int seek(long int offset, int whence) const override;
	long int tell() const override;
	unsigned long int read(void *buffer, unsigned long int bytes) const override;
	unsigned long int write(void *buffer, unsigned long int bytes) override;

  private:
	unsigned char *bufferPtr_;
	/// \note Modified by `seek` and `tell` constant methods
	mutable unsigned long int seekOffset_;
	bool isWritable_;

	/// Deleted copy constructor
	MemoryFile(const MemoryFile &) = delete;
	/// Deleted assignment operator
	MemoryFile &operator=(const MemoryFile &) = delete;
};

}

#endif
