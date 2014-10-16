#include <cstdlib> // for exit()
#if !(defined(_WIN32) && !defined(__MINGW32__))
	// All but MSVC: Linux, Android and MinGW.
	#include <sys/stat.h> // for open()
	#include <fcntl.h> // for open()
	#include <unistd.h> // for close()
#else
	#include <io.h> // for _access()
#endif
#include "ncStandardFile.h"
#include "ncServiceLocator.h"

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ncStandardFile::~ncStandardFile()
{
	if (shouldCloseOnExit_)
	{
		close();
	}
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/// Tries to open the file
void ncStandardFile::open(unsigned char mode)
{
	// Checking if the file is already opened
	if (fileDescriptor_ >= 0 || filePointer_ != NULL)
	{
		ncServiceLocator::logger().write(ncILogger::LOG_WARN, (const char *)"ncStandardFile::open - File \"%s\" is already opened", filename_);
	}
	else
	{
#if !(defined(_WIN32) && !defined(__MINGW32__))
		// Opening with a file descriptor
		if (mode & MODE_FD)
		{
			openFd(mode);
		}
		// Opening with a file stream
		else
#endif
			openStream(mode);
	}
}

/// Closes the file (both opened or fopened)
void ncStandardFile::close()
{
	if (fileDescriptor_ >= 0)
	{
#if !(defined(_WIN32) && !defined(__MINGW32__))
		int retValue = ::close(fileDescriptor_);
		if (retValue < 0)
		{
			ncServiceLocator::logger().write(ncILogger::LOG_WARN, (const char *)"ncStandardFile::close - Cannot close the file \"%s\"", filename_);
		}
		else
		{
			ncServiceLocator::logger().write(ncILogger::LOG_INFO, (const char *)"ncStandardFile::close - File \"%s\" closed", filename_);
			fileDescriptor_ = -1;
		}
#endif
	}
	else if (filePointer_)
	{
		int retValue = fclose(filePointer_);
		if (retValue == EOF)
		{
			ncServiceLocator::logger().write(ncILogger::LOG_WARN, (const char *)"ncStandardFile::close - Cannot close the file \"%s\"", filename_);
		}
		else
		{
			ncServiceLocator::logger().write(ncILogger::LOG_INFO, (const char *)"ncStandardFile::close - File \"%s\" closed", filename_);
			filePointer_ = NULL;
		}
	}
}

long int ncStandardFile::seek(long int offset, int whence) const
{
	long int seekValue = -1;

	if (fileDescriptor_ >= 0)
	{
#if !(defined(_WIN32) && !defined(__MINGW32__))
		seekValue = lseek(fileDescriptor_, offset, whence);
#endif
	}
	else if (filePointer_)
	{
		seekValue = fseek(filePointer_, offset, whence);
	}

	return seekValue;
}

long int ncStandardFile::tell() const
{
	long int tellValue = -1;

	if (fileDescriptor_ >= 0)
	{
#if !(defined(_WIN32) && !defined(__MINGW32__))
		tellValue = lseek(fileDescriptor_, 0L, SEEK_CUR);
#endif
	}
	else if (filePointer_)
	{
		tellValue = ftell(filePointer_);
	}

	return tellValue;
}


long int ncStandardFile::read(void *buffer, int bytes) const
{
	long int bytesRead = -1;

	if (fileDescriptor_ >= 0)
	{
#if !(defined(_WIN32) && !defined(__MINGW32__))
		bytesRead = ::read(fileDescriptor_, buffer, bytes);
#endif
	}
	else if (filePointer_)
	{
		bytesRead = fread(buffer, 1, bytes, filePointer_);
	}

	return bytesRead;
}


///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

/// Opens the file with open()
void ncStandardFile::openFd(unsigned char mode)
{
#if !(defined(_WIN32) && !defined(__MINGW32__))
	int openFlag = -1;

	switch (mode)
	{
		case (MODE_FD|MODE_READ):
			openFlag = O_RDONLY;
			break;
		case (MODE_FD|MODE_WRITE):
			openFlag = O_WRONLY;
			break;
		case (MODE_FD|MODE_READ|MODE_WRITE):
			openFlag = O_RDWR;
			break;
		default:
			ncServiceLocator::logger().write(ncILogger::LOG_ERROR, (const char *)"ncStandardFile::openFd - Cannot open the file \"%s\", wrong open mode", filename_);
			break;
	}

	if (openFlag >= 0)
	{
		fileDescriptor_ = ::open(filename_, openFlag);

		if (fileDescriptor_ < 0)
		{
			ncServiceLocator::logger().write(ncILogger::LOG_FATAL, (const char *)"ncStandardFile::openFd - Cannot open the file \"%s\"", filename_);
			exit(EXIT_FAILURE);
		}
		else
		{
			ncServiceLocator::logger().write(ncILogger::LOG_INFO, (const char *)"ncStandardFile::openFd - File \"%s\" opened", filename_);
		}

		// Calculating file size
		fileSize_ = lseek(fileDescriptor_, 0L, SEEK_END);
		lseek(fileDescriptor_, 0L, SEEK_SET);
	}
#endif
}

/// Opens the file with fopen()
void ncStandardFile::openStream(unsigned char mode)
{
	char modeChars[3] = {'\0', '\0', '\0'};

	switch (mode)
	{
		case (MODE_READ):
			modeChars[0] = 'r';
			break;
		case (MODE_WRITE):
			modeChars[0] = 'w';
			break;
		case (MODE_READ|MODE_WRITE):
			modeChars[0] = 'r';
			modeChars[1] = '+';
			break;
		case (MODE_READ|MODE_BINARY):
			modeChars[0] = 'r';
			modeChars[1] = 'b';
			break;
		case (MODE_WRITE|MODE_BINARY):
			modeChars[0] = 'w';
			modeChars[1] = 'b';
			break;
		case (MODE_READ|MODE_WRITE|MODE_BINARY):
			modeChars[0] = 'r';
			modeChars[1] = '+';
			modeChars[2] = 'b';
			break;
		default:
			ncServiceLocator::logger().write(ncILogger::LOG_ERROR, (const char *)"ncStandardFile::openStream - Cannot open the file \"%s\", wrong open mode", filename_);
			break;
	}

	if (modeChars[0] != '\0')
	{
		filePointer_ = fopen(filename_, modeChars);

		if (filePointer_ == NULL)
		{
			ncServiceLocator::logger().write(ncILogger::LOG_FATAL, (const char *)"ncStandardFile::openStream - Cannot open the file \"%s\"", filename_);
			exit(EXIT_FAILURE);
		}
		else
		{
			ncServiceLocator::logger().write(ncILogger::LOG_INFO, (const char *)"ncStandardFile::openStream - File \"%s\" opened", filename_);
		}

		// Calculating file size
		fseek(filePointer_, 0L, SEEK_END);
		fileSize_ = ftell(filePointer_);
		fseek(filePointer_, 0L, SEEK_SET);
	}
}

/// Checks if a file can be accessed with specified mode
/*! It is called by ncIFile::access() */
bool ncStandardFile::access(const char *filename, unsigned char mode)
{
	bool isAccessible = false;
	int accessMode = -1;

#if !(defined(_WIN32) && !defined(__MINGW32__))
	switch (mode)
	{
		case (ncIFile::MODE_EXISTS):
			accessMode = F_OK;
			break;
		case (ncIFile::MODE_CAN_READ):
			accessMode = R_OK;
			break;
		case (ncIFile::MODE_CAN_WRITE):
			accessMode = W_OK;
			break;
		case (ncIFile::MODE_CAN_READ|ncIFile::MODE_CAN_WRITE):
			accessMode = R_OK | W_OK;
			break;
		default:
			ncServiceLocator::logger().write(ncILogger::LOG_ERROR, (const char *)"ncStandardFile::access - Cannot access the file \"%s\", wrong access mode", filename);
			break;
	}

	if (accessMode != -1)
	{
		isAccessible = (::access(filename, accessMode) == 0);
	}
#else
	switch (mode)
	{
		case (ncIFile::MODE_EXISTS):
			accessMode = 0;
			break;
		case (ncIFile::MODE_CAN_READ):
			accessMode = 2;
			break;
		case (ncIFile::MODE_CAN_WRITE):
			accessMode = 4;
			break;
		case (ncIFile::MODE_CAN_READ|ncIFile::MODE_CAN_WRITE):
			accessMode = 6;
			break;
		default:
			ncServiceLocator::logger().write(ncILogger::LOG_ERROR, (const char *)"ncStandardFile::access - Cannot access the file \"%s\", wrong access mode", filename);
			break;
	}

	if (accessMode != -1)
	{
		isAccessible = (::_access(filename, accessMode) == 0);
	}
#endif

	return isAccessible;
}
