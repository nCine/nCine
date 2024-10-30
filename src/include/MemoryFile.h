#ifndef CLASS_NCINE_MEMORYFILE
#define CLASS_NCINE_MEMORYFILE

#include "IFile.h"
#include <nctl/UniquePtr.h>

namespace ncine {

/// The class creating a file interface around a memory buffer
class MemoryFile : public IFile
{
  public:
	MemoryFile(const char *bufferName, unsigned char *bufferPtr, unsigned long int bufferSize);
	MemoryFile(const char *bufferName, const unsigned char *bufferPtr, unsigned long int bufferSize);
	MemoryFile(unsigned char *bufferPtr, unsigned long int bufferSize);
	MemoryFile(const unsigned char *bufferPtr, unsigned long int bufferSize);

	/// Creates a memory file object with the specified name, that allocates a buffer of the specified size
	MemoryFile(const char *bufferName, unsigned long int bufferSize);
	/// Creates a memory file object that allocates a buffer of the specified size
	MemoryFile(unsigned long int bufferSize);

	/// Creates a memory file object with the specified name, that takes ownership of the specified buffer
	MemoryFile(const char *bufferName, nctl::UniquePtr<unsigned char []> buffer, unsigned long int bufferSize);
	/// Creates a memory file object that takes ownership of the specified buffer
	MemoryFile(nctl::UniquePtr<unsigned char []> buffer, unsigned long int bufferSize);

	void open(unsigned char mode) override;
	void close() override;
	long int seek(long int offset, int whence) const override;
	long int tell() const override;
	unsigned long int read(void *buffer, unsigned long int bytes) const override;
	unsigned long int write(const void *buffer, unsigned long int bytes) override;

	/*! \note The returned pointer is not offset by the value of `seekOffset_` */
	inline const void *bufferPtr() const override { return bufferPtr_; }
	/*! \note The returned pointer is not offset by the value of `seekOffset_` */
	inline void *bufferPtr() override { return bufferPtr_; }

  private:
	unsigned char *bufferPtr_;
	/// \note Modified by `seek` and `tell` constant methods
	mutable unsigned long int seekOffset_;
	bool isWritable_;

	/// Buffer used with the constructors that allocate memory
	nctl::UniquePtr<unsigned char []> ownedBuffer_;

	/// Deleted copy constructor
	MemoryFile(const MemoryFile &) = delete;
	/// Deleted assignment operator
	MemoryFile &operator=(const MemoryFile &) = delete;
};

}

#endif
