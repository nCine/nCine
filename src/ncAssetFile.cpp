#include <sys/stat.h> // for open()
#include <fcntl.h> // for open()
#include <unistd.h> // for close()
#include <stdlib.h> // for exit()
#include "ncAssetFile.h"
#include "ncServiceLocator.h"

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

AAssetManager* ncAssetFile::m_pAssetManager = NULL;

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ncAssetFile::ncAssetFile(const char *pFilename)
	: ncIFile(pFilename), m_pAsset(NULL), m_lStartOffset(0L)
{
	m_eType = ASSET_TYPE;

	// Detect fake second extension added to prevent compression
	char *pFirstDotChar = strchr(m_vFilename, '.');
	char *pLastDotChar = strrchr(m_vFilename, '.');
	int iExtLEngth = pLastDotChar-pFirstDotChar-1;
	if (iExtLEngth >= 3 && iExtLEngth <=4)
	{
		memset(m_vExtension, 0, s_uMaxExtensionsLength);
		strncpy(m_vExtension, pFirstDotChar+1, iExtLEngth);
	}
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/// Tries to open the file
void ncAssetFile::Open(unsigned char uMode)
{
	// Checking if the file is already opened
	if (m_iFileDescriptor >= 0 || m_pAsset != NULL)
		ncServiceLocator::Logger().Write(ncILogger::LOG_WARN, (const char *)"ncAssetFile::Open - File \"%s\" is already opened", m_vFilename);
	else
	{
		// Opening with a file descriptor
		if (uMode & MODE_FD)
			OpenFD(uMode);
		// Opening as an asset only
		else
			OpenAsset(uMode);
	}
}


/// Closes the file (both opened or fopened)
void ncAssetFile::Close()
{
	if (m_iFileDescriptor >= 0)
	{
		int iRetValue = close(m_iFileDescriptor);
		if (iRetValue < 0)
			ncServiceLocator::Logger().Write(ncILogger::LOG_WARN, (const char *)"ncAssetFile::Close - Cannot close the file \"%s\"", m_vFilename);
		else
		{
			ncServiceLocator::Logger().Write(ncILogger::LOG_INFO, (const char *)"ncAssetFile::Close - File \"%s\" closed", m_vFilename);
			m_iFileDescriptor = -1;
		}
	}
	else if (m_pAsset)
	{
		AAsset_close(m_pAsset);
		m_pAsset = NULL;
		ncServiceLocator::Logger().Write(ncILogger::LOG_INFO, (const char *)"ncAssetFile::Close - File \"%s\" closed", m_vFilename);
	}
}

long int ncAssetFile::Seek(long int lOffset, int iWhence) const
{
	if (m_iFileDescriptor >= 0)
	{
		long int lSeekValue;
		switch(iWhence)
		{
			case SEEK_SET:
				lSeekValue = lseek(m_iFileDescriptor, m_lStartOffset+lOffset, SEEK_SET);
				break;
			case SEEK_CUR:
				lSeekValue = lseek(m_iFileDescriptor, lOffset, SEEK_CUR);
				break;
			case SEEK_END:
				lSeekValue = lseek(m_iFileDescriptor, m_lStartOffset+m_lFileSize+lOffset, SEEK_END);
				break;
		}
		return (lSeekValue-m_lStartOffset);
	}
	else if (m_pAsset)
		return AAsset_seek(m_pAsset, lOffset, iWhence);
}

long int ncAssetFile::Tell() const
{
	if (m_iFileDescriptor >= 0)
	{
		long int lSeekValue = lseek(m_iFileDescriptor, 0L, SEEK_CUR);
		return (lSeekValue-m_lStartOffset);
	}
	else if (m_pAsset)
		return AAsset_seek(m_pAsset, 0L, SEEK_CUR);
}


long int ncAssetFile::Read(void *pBuffer, int iBytes) const
{
	if (m_iFileDescriptor >= 0)
	{
		int iBytesToRead = iBytes;

		long int lSeekValue = lseek(m_iFileDescriptor, 0L, SEEK_CUR);

		if (lSeekValue >=  m_lStartOffset + m_lFileSize)
			iBytesToRead = 0; // simulating EOF
		else if (lSeekValue + iBytes > m_lStartOffset + m_lFileSize)
			iBytesToRead = (m_lStartOffset + m_lFileSize) - lSeekValue;

		return read(m_iFileDescriptor, pBuffer, iBytesToRead);
	}
	else if (m_pAsset)
		return AAsset_read(m_pAsset, pBuffer, iBytes);
}

bool ncAssetFile::IsOpened() const
{
	if (m_iFileDescriptor >= 0 || m_pAsset != NULL)
		return true;
	else
		return false;
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

/// Opens the file with AAsset_openFileDescriptor()
void ncAssetFile::OpenFD(unsigned char uMode)
{
	// An asset file can only be read
	if (uMode == (MODE_FD|MODE_READ))
	{
		m_pAsset = AAssetManager_open(m_pAssetManager, m_vFilename, AASSET_MODE_UNKNOWN);
		if (m_pAsset == NULL)
		{
			ncServiceLocator::Logger().Write(ncILogger::LOG_FATAL, (const char *)"ncAssetFile::OpenFD - Cannot open the file \"%s\"", m_vFilename);
			exit(-1);
		}

		m_iFileDescriptor = AAsset_openFileDescriptor(m_pAsset, &m_lStartOffset, &m_lFileSize);
		lseek(m_iFileDescriptor, m_lStartOffset, SEEK_SET);
		AAsset_close(m_pAsset);
		m_pAsset = NULL;

		if (m_iFileDescriptor < 0)
		{
			ncServiceLocator::Logger().Write(ncILogger::LOG_FATAL, (const char *)"ncAssetFile::OpenFD - Cannot open the file \"%s\"", m_vFilename);
			exit(-1);
		}
		else
			ncServiceLocator::Logger().Write(ncILogger::LOG_INFO, (const char *)"ncAssetFile::OpenFD - File \"%s\" opened", m_vFilename);
	}
	else
		ncServiceLocator::Logger().Write(ncILogger::LOG_ERROR, (const char *)"ncAssetFile::OpenFD - Cannot open the file \"%s\", wrong open mode", m_vFilename);
}

/// Opens the file with AAssetManager_open() only
void ncAssetFile::OpenAsset(unsigned char uMode)
{
	// An asset file can only be read
	if (uMode == MODE_READ || uMode == (MODE_READ|MODE_BINARY))
	{
		m_pAsset = AAssetManager_open(m_pAssetManager, m_vFilename, AASSET_MODE_UNKNOWN);
		if (m_pAsset == NULL)
		{
			ncServiceLocator::Logger().Write(ncILogger::LOG_FATAL, (const char *)"ncAssetFile::OpenAsset - Cannot open the file \"%s\"", m_vFilename);
			exit(-1);
		}
		else
			ncServiceLocator::Logger().Write(ncILogger::LOG_INFO, (const char *)"ncAssetFile::OpenAsset - File \"%s\" opened", m_vFilename);

		// Calculating file size
		m_lFileSize = AAsset_getLength(m_pAsset);
	}
	else
		ncServiceLocator::Logger().Write(ncILogger::LOG_ERROR, (const char *)"ncAssetFile::OpenAsset - Cannot open the file \"%s\", wrong open mode", m_vFilename);
}
