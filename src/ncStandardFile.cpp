#include <sys/stat.h> // for open()
#include <fcntl.h> // for open()
#include <unistd.h> // for close()
#include <cstdlib> // for exit()
#include "ncStandardFile.h"
#include "ncServiceLocator.h"

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/// Tries to open the file
void ncStandardFile::Open(unsigned char uMode)
{
	// Checking if the file is already opened
	if (m_iFileDescriptor >= 0 || m_pFilePointer != NULL)
		ncServiceLocator::Logger().Write(ncILogger::LOG_WARN, (const char *)"ncStandardFile::Open - File \"%s\" is already opened", m_vFilename);
	else
	{
		// Opening with a file descriptor
		if (uMode & MODE_FD)
			OpenFD(uMode);
		// Opening with a file stream
		else
			OpenStream(uMode);
	}
}

/// Closes the file (both opened or fopened)
void ncStandardFile::Close()
{
	if (m_iFileDescriptor >= 0)
	{
		int iRetValue = close(m_iFileDescriptor);
		if (iRetValue < 0)
			ncServiceLocator::Logger().Write(ncILogger::LOG_WARN, (const char *)"ncStandardFile::Close - Cannot close the file \"%s\"", m_vFilename);
		else
		{
			ncServiceLocator::Logger().Write(ncILogger::LOG_INFO, (const char *)"ncStandardFile::Close - File \"%s\" closed", m_vFilename);
			m_iFileDescriptor = -1;
		}
	}
	else if (m_pFilePointer)
	{
		int iRetValue = fclose(m_pFilePointer);
		if (iRetValue == EOF)
			ncServiceLocator::Logger().Write(ncILogger::LOG_WARN, (const char *)"ncStandardFile::Close - Cannot close the file \"%s\"", m_vFilename);
		else
		{
			ncServiceLocator::Logger().Write(ncILogger::LOG_INFO, (const char *)"ncStandardFile::Close - File \"%s\" closed", m_vFilename);
			m_pFilePointer = NULL;
		}
	}
}

long int ncStandardFile::Seek(long int lOffset, int iWhence) const
{
	long int lSeekValue = -1;

	if (m_iFileDescriptor >= 0)
	{
		lSeekValue = lseek(m_iFileDescriptor, lOffset, iWhence);
	}
	else if (m_pFilePointer)
	{
		lSeekValue = fseek(m_pFilePointer, lOffset, iWhence);
	}

	return lSeekValue;
}

long int ncStandardFile::Tell() const
{
	long int lTellValue = -1;

	if (m_iFileDescriptor >= 0)
	{
		lTellValue = lseek(m_iFileDescriptor, 0L, SEEK_CUR);
	}
	else if (m_pFilePointer)
	{
		lTellValue = ftell(m_pFilePointer);
	}

	return lTellValue;
}


long int ncStandardFile::Read(void *pBuffer, int iBytes) const
{
	long int lBytesRead = -1;

	if (m_iFileDescriptor >= 0)
	{
		lBytesRead = read(m_iFileDescriptor, pBuffer, iBytes);
	}
	else if (m_pFilePointer)
	{
		lBytesRead = fread(pBuffer, 1, iBytes, m_pFilePointer);
	}

//	printf("bytes: %ld\n", lBytesRead);
	return lBytesRead;
}


///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

/// Opens the file with open()
void ncStandardFile::OpenFD(unsigned char uMode)
{
	int iOFlag = -1;

	switch(uMode)
	{
		case (MODE_FD|MODE_READ):
			iOFlag = O_RDONLY;
			break;
		case (MODE_FD|MODE_WRITE):
			iOFlag = O_WRONLY;
			break;
		case (MODE_FD|MODE_READ|MODE_WRITE):
			iOFlag = O_RDWR;
			break;
		default:
			ncServiceLocator::Logger().Write(ncILogger::LOG_ERROR, (const char *)"ncStandardFile::OpenFD - Cannot open the file \"%s\", wrong open mode", m_vFilename);
			break;
	}

	if (iOFlag >= 0)
	{
		m_iFileDescriptor = open(m_vFilename, iOFlag);

		if (m_iFileDescriptor < 0)
		{
			ncServiceLocator::Logger().Write(ncILogger::LOG_FATAL, (const char *)"ncStandardFile::OpenFD - Cannot open the file \"%s\"", m_vFilename);
			exit(-1);
		}
		else
			ncServiceLocator::Logger().Write(ncILogger::LOG_INFO, (const char *)"ncStandardFile::OpenFD - File \"%s\" opened", m_vFilename);

		// Calculating file size
		m_lFileSize = lseek(m_iFileDescriptor, 0L, SEEK_END);
		lseek(m_iFileDescriptor, 0L, SEEK_SET);
	}
}

/// Opens the file with fopen()
void ncStandardFile::OpenStream(unsigned char uMode)
{
	char pMode[3] = {'\0', '\0', '\0'};

	switch(uMode)
	{
		case (MODE_READ):
			pMode[0] = 'r';
			break;
		case (MODE_WRITE):
			pMode[0] = 'w';
			break;
		case (MODE_READ|MODE_WRITE):
			pMode[0] = 'r';
			pMode[1] = '+';
			break;
		case (MODE_READ|MODE_BINARY):
			pMode[0] = 'r';
			pMode[1] = 'b';
			break;
		case (MODE_WRITE|MODE_BINARY):
			pMode[0] = 'w';
			pMode[1] = 'b';
			break;
		case (MODE_READ|MODE_WRITE|MODE_BINARY):
			pMode[0] = 'r';
			pMode[1] = '+';
			pMode[2] = 'b';
		default:
			ncServiceLocator::Logger().Write(ncILogger::LOG_ERROR, (const char *)"ncStandardFile::OpenStream - Cannot open the file \"%s\", wrong open mode", m_vFilename);
			break;
	}

	if (pMode[0] != '\0')
	{
		m_pFilePointer = fopen(m_vFilename, pMode);

		if (m_pFilePointer == NULL)
		{
			ncServiceLocator::Logger().Write(ncILogger::LOG_FATAL, (const char *)"ncStandardFile::OpenStream - Cannot open the file \"%s\"", m_vFilename);
			exit(-1);
		}
		else
			ncServiceLocator::Logger().Write(ncILogger::LOG_INFO, (const char *)"ncStandardFile::OpenStream - File \"%s\" opened", m_vFilename);

		// Calculating file size
		fseek(m_pFilePointer, 0L, SEEK_END);
		m_lFileSize = ftell(m_pFilePointer);
		fseek(m_pFilePointer, 0L, SEEK_SET);
	}
}
