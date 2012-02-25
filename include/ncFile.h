#ifndef CLASS_NCFILE
#define CLASS_NCFILE

#include <stdio.h>
#include <stdint.h> // for endianness conversions

/// The class dealing with file open/close, its path and extension
class ncFile
{
private:
	/// Max number of characters for a file name (path included)
	static const unsigned int s_uMaxFilenameLength = 256;
	/// File name with path
	char m_filename[s_uMaxFilenameLength];
	/// Max number of characters for a file extension
	static const unsigned int s_uMaxExtensionsLength = 4;
	/// File extension
	char m_extension[s_uMaxExtensionsLength];

	/// File descriptor for open() and close()
	int m_iFileDescriptor;
	/// File pointer for fopen() and fclose()
	FILE *m_pFilePointer;
	/// Should destructor close the file on exit?
	/*! Useful for ov_open()/ov_fopen() and ov_clear() */
	bool m_bShouldCloseOnExit;
public:
	ncFile(const char *pFilename);
	~ncFile();

	// Tries to open the file with open()
	int Open(int iOFlag);
	// Tries to open the file with fopen()
	FILE* FOpen(const char *pMode);
	// Closes the file (both opened or fopened)
	void Close();

	/// Sets the close on exit flag
	inline void SetCloseOnExit(bool bShouldCloseOnExit) { m_bShouldCloseOnExit = bShouldCloseOnExit; }

	/// Returns file name with path
	const char* Filename() const { return m_filename; }
	/// Returns file extension
	const char* Extension() const { return m_extension; }
	// Checks if file extension matches
	bool IsExtension(const char * pExtension) const;

	/// Returns file descriptor
	inline int Fd() const { return m_iFileDescriptor; }
	/// Returns file pointer
	inline FILE* Ptr() const { return m_pFilePointer; }

	/// Reads a little endian 16 bit unsigned integer
	static inline uint16_t Int16FromLE(uint16_t uNumber) { return uNumber; }
	/// Reads a little endian 32 bit unsigned integer
	static inline uint32_t Int32FromLE(uint32_t uNumber) { return uNumber; }
	/// Reads a big endian 16 bit unsigned integer
	static inline uint16_t Int16FromBE(uint16_t uNumber)
	{
		return (uNumber>>8) | (uNumber<<8);
	}
	/// Reads a big endian 32 bit unsigned integer
	static inline uint32_t Int32FromBE(uint32_t uNumber)
	{
		return (uNumber>>24) | ((uNumber<<8) & 0x00FF0000) | ((uNumber>>8) & 0x0000FF00) | (uNumber<<24);
	}
};

#endif
