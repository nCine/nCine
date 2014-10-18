#include <cstdlib> // for exit() and getenv()
#include "ncIFile.h"
#include "ncStandardFile.h"

#ifdef __ANDROID__
#include "ncAssetFile.h"
#include <unistd.h> // for getpid()
#include <sys/stat.h> // for mkdir()
#include <errno.h> // for EEXIST
#include "ncServiceLocator.h"
#endif

#ifndef _WIN32
#include <unistd.h> // for getpid()
#include <sys/types.h> // for getpwuid()
#include <pwd.h> // for getpwuid()
#endif

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

char ncIFile::dataPath_[MaxFilenameLength] = {'\0'};

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ncIFile::ncIFile(const char *filename)
	: type_(BASE_TYPE), fileDescriptor_(-1), filePointer_(NULL), shouldCloseOnExit_(true), fileSize_(0)
{
	memset(filename_, 0, MaxFilenameLength);
	strncpy(filename_, filename, MaxFilenameLength);

	memset(extension_, 0, MaxExtensionsLength);
	char *dotChar = strrchr(filename_, '.');
	// A dot followed by at least three extension characters
	if (dotChar && strlen(dotChar) >= 4)
	{
		strncpy(extension_, dotChar + 1, MaxExtensionsLength - 1);    // preserves '\0'
	}
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/// Returns true if the file is already opened
bool ncIFile::isOpened() const
{
	if (fileDescriptor_ >= 0 || filePointer_ != NULL)
	{
		return true;
	}
	else
	{
		return false;
	}
}

/// Checks if file extension matches
bool ncIFile::hasExtension(const char *extension) const
{
	return !strncmp(extension_, extension, MaxExtensionsLength);
}

/// Returns the proper file handle according to prepended tags
ncIFile* ncIFile::createFileHandle(const char *filename)
{
#ifdef __ANDROID__
	if (strncmp(filename, (const char *)"asset::", 7) == 0)
	{
		return new ncAssetFile(filename + 7);
	}
	else
#endif
		return new ncStandardFile(filename);
}

/// Checks if a file can be accessed with specified mode
bool ncIFile::access(const char *filename, unsigned char mode)
{
#ifdef __ANDROID__
	if (strncmp(filename, (const char *)"asset::", 7) == 0)
	{
		return ncAssetFile::access(filename + 7, mode);
	}
	else
#endif
		return ncStandardFile::access(filename, mode);
}

/// Returns the writable directory for data storage
char* ncIFile::dataPath()
{
	// Searching for path only on first invokation
	if (strlen(dataPath_))
	{
		return dataPath_;
	}

	memset(dataPath_, 0, MaxFilenameLength);

#ifdef __ANDROID__
	int pid = getpid();
	char procFileName[MaxFilenameLength];
	snprintf(procFileName, MaxFilenameLength, "/proc/%d/cmdline", pid);

	FILE *procFile = fopen(procFileName, "r");
	if (procFile)
	{
		// Creating the path "/data/data/PACKAGE_NAME/files/"
		strncpy(dataPath_, "/data/data/", 11);
		fread((char *)dataPath_ + 11, MaxFilenameLength - 11, 1, procFile);
		strncat(dataPath_, "/files/", 7);
		fclose(procFile);

		// Trying to create the data directory
		if (mkdir(dataPath_, 0770) && errno != EEXIST)
		{
			LOGE_X("Cannot create directory: %s", dataPath_);
			memset(dataPath_, 0, MaxFilenameLength);
		}
	}
#elif _WIN32
	char *userProfileEnv = getenv("USERPROFILE");
	if (userProfileEnv == NULL || strlen(userProfileEnv) == 0)
	{
		char *homeDriveEnv = getenv("HOMEDRIVE");
		char *homePathEnv = getenv("HOMEPATH");

		if ((homeDriveEnv == NULL || strlen(homeDriveEnv) == 0) &&
			(homePathEnv == NULL || strlen(homePathEnv) == 0))
		{
			char *homeEnv = getenv("HOME");
			if (homeEnv && strlen(homeEnv))
			{
				strncpy(dataPath_, homeEnv, MaxFilenameLength);
			}
		}
		else
		{
			strncpy(dataPath_, homeDriveEnv, MaxFilenameLength);
			strncat(dataPath_, homePathEnv, MaxFilenameLength - strlen(homeDriveEnv));
		}
	}
	else
	{
		strncpy(dataPath_, userProfileEnv, MaxFilenameLength);
	}

	if (strlen(dataPath_))
	{
		strncat(dataPath_, "\\", 1);
	}
#else
	char *homeEnv = getenv("HOME");

	if (homeEnv == NULL || strlen(homeEnv) == 0)
	{
		strncpy(dataPath_, getpwuid(getuid())->pw_dir, MaxFilenameLength);
	}
	else
	{
		strncpy(dataPath_, homeEnv, MaxFilenameLength);
	}

	strncat(dataPath_, "/.config/", MaxFilenameLength - strlen(dataPath_));
#endif

	return dataPath_;
}
