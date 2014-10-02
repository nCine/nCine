#ifndef CLASS_NCSTANDARDFILE
#define CLASS_NCSTANDARDFILE

#include "ncIFile.h"

/// The class dealing with file open/close, its path and extension
class ncStandardFile: public ncIFile
{
  public:
	/// Constructs a standard file object
	/*! \param pFilename File name including its path */
	ncStandardFile(const char *pFilename) : ncIFile(pFilename) { m_eType = STANDARD_TYPE; }

	~ncStandardFile()
	{
		if (m_bShouldCloseOnExit)
		{
			Close();
		}
	}

	/// Static method to return class type
	inline static eFileType sType() { return STANDARD_TYPE; }

	void Open(unsigned char uMode);
	void Close();
	long int Seek(long int lOffset, int iWhence) const;
	long int Tell() const;
	long int Read(void *pBuffer, int iBytes) const;

  private:
	// Opens the file with open()
	void OpenFD(unsigned char uMode);
	// Opens the file with fopen()
	void OpenStream(unsigned char uMode);

	// Checks if a file can be accessed with specified mode
	static bool Access(const char *pFilename, unsigned char uMode);

	friend bool ncIFile::Access(const char *pFilename, unsigned char uMode);
};

#endif
