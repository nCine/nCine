#include <sys/stat.h> // for open()
#include <fcntl.h> // for open()
#include <unistd.h> // for close()
#include <stdlib.h> // for exit()
#include <cstring>
#include "ncFile.h"
#include "ncServiceLocator.h"

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ncFile::ncFile(const char *pFilename)
	: m_iFileDescriptor(-1), m_pFilePointer(NULL), m_bShouldCloseOnExit(true)
{
	memset(m_filename, 0, s_uMaxFilenameLength);
	strncpy(m_filename, pFilename, s_uMaxFilenameLength);

	memset(m_extension, 0, s_uMaxExtensionsLength);

	char *pDotChar = strrchr(m_filename, '.');
	// A dot followed by at least three extension characters
	if (pDotChar && strlen(pDotChar) >= 4)
		strncpy(m_extension, pDotChar+1, s_uMaxExtensionsLength);
}

ncFile::~ncFile()
{
	if (m_bShouldCloseOnExit)
		Close();
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/// Tries to open the file with open()
int ncFile::Open(int iOFlag)
{
	// Checking if the file is already opened
	if (m_iFileDescriptor >= 0)
	{
		ncServiceLocator::Logger().Write(ncILogger::LOG_WARN, (const char *)"ncFile::Open - File \"%s\" is already opened", m_filename);
		return m_iFileDescriptor;
	}
	else if (m_pFilePointer != NULL)
	{
		ncServiceLocator::Logger().Write(ncILogger::LOG_WARN, (const char *)"ncFile::Open - File \"%s\" is already fopened", m_filename);
		return -1;
	}


	m_iFileDescriptor = open(m_filename, iOFlag);

	if (m_iFileDescriptor < 0)
	{
		ncServiceLocator::Logger().Write(ncILogger::LOG_FATAL, (const char *)"ncFile::Open - Cannot open the file \"%s\"", m_filename);
		exit(-1);
	}
	else
		ncServiceLocator::Logger().Write(ncILogger::LOG_INFO, (const char *)"ncFile::Open - File \"%s\" opened", m_filename);

	return m_iFileDescriptor;
}

/// Tries to open the file with fopen()
FILE* ncFile::FOpen(const char *pMode)
{
	// Checking if the file is already opened
	if (m_iFileDescriptor >= 0)
	{
		ncServiceLocator::Logger().Write(ncILogger::LOG_WARN, (const char *)"ncFile::FOpen - File \"%s\" is already opened", m_filename);
		return NULL;
	}
	else if (m_pFilePointer != NULL)
	{
		ncServiceLocator::Logger().Write(ncILogger::LOG_WARN, (const char *)"ncFile::FOpen - File \"%s\" is already fopened", m_filename);
		return m_pFilePointer;
	}


	m_pFilePointer = fopen(m_filename, pMode);

	if (m_pFilePointer == NULL)
	{
		ncServiceLocator::Logger().Write(ncILogger::LOG_FATAL, (const char *)"ncFile::FOpen - Cannot open the file \"%s\"", m_filename);
		exit(-1);
	}
	else
		ncServiceLocator::Logger().Write(ncILogger::LOG_INFO, (const char *)"ncFile::FOpen - File \"%s\" opened", m_filename);

	return m_pFilePointer;
}

/// Closes the file (both opened or fopened)
void ncFile::Close()
{
	if (m_iFileDescriptor >= 0)
	{
		int iRetValue = close(m_iFileDescriptor);
		if (iRetValue < 0)
			ncServiceLocator::Logger().Write(ncILogger::LOG_WARN, (const char *)"ncFile::Close - Cannot close the file \"%s\"", m_filename);
		else
			ncServiceLocator::Logger().Write(ncILogger::LOG_INFO, (const char *)"ncFile::Close - File \"%s\" closed", m_filename);
	}
	else if (m_pFilePointer)
	{
		int iRetValue = fclose(m_pFilePointer);
		if (iRetValue == EOF)
			ncServiceLocator::Logger().Write(ncILogger::LOG_WARN, (const char *)"ncFile::Close - Cannot fclose the file \"%s\"", m_filename);
		else
			ncServiceLocator::Logger().Write(ncILogger::LOG_INFO, (const char *)"ncFile::Close - File \"%s\" fclosed", m_filename);
	}
}

/// Checks if file extension matches
bool ncFile::IsExtension(const char *pExtension) const
{
	return !strncmp(m_extension, pExtension, s_uMaxExtensionsLength);
}
