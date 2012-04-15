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

char ncIFile::m_vDataPath[s_uMaxFilenameLength] = {'\0'};

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ncIFile::ncIFile(const char *pFilename)
	: m_eType(BASE_TYPE), m_iFileDescriptor(-1), m_pFilePointer(NULL), m_bShouldCloseOnExit(true), m_lFileSize(0)
{
	memset(m_vFilename, 0, s_uMaxFilenameLength);
	strncpy(m_vFilename, pFilename, s_uMaxFilenameLength);

	memset(m_vExtension, 0, s_uMaxExtensionsLength);
	char *pDotChar = strrchr(m_vFilename, '.');
	// A dot followed by at least three extension characters
	if (pDotChar && strlen(pDotChar) >= 4)
		strncpy(m_vExtension, pDotChar+1, s_uMaxExtensionsLength-1); // preserves '\0'
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/// Returns true if the file is already opened
bool ncIFile::IsOpened() const
{
	if (m_iFileDescriptor >= 0 || m_pFilePointer != NULL)
		return true;
	else
		return false;
}

/// Checks if file extension matches
bool ncIFile::HasExtension(const char *pExtension) const
{
	return !strncmp(m_vExtension, pExtension, s_uMaxExtensionsLength);
}

/// Returns the proper file handle according to prepended tags
ncIFile* ncIFile::CreateFileHandle(const char *pFilename)
{
#ifdef __ANDROID__
	if (strncmp(pFilename, (const char *)"asset::", 7) == 0)
		return new ncAssetFile(pFilename+7);
	else
#endif
		return new ncStandardFile(pFilename);
}

/// Returns the writable directory for data storage
char* ncIFile::DataPath()
{
	// Searching for path only on first invokation
	if (strlen(m_vDataPath))
		return m_vDataPath;

	memset(m_vDataPath, 0, s_uMaxFilenameLength);

#ifdef __ANDROID__
	int iPid = getpid();
	char vProcFileName[s_uMaxFilenameLength];
	snprintf(vProcFileName, s_uMaxFilenameLength, "/proc/%d/cmdline", iPid);

	FILE *pProcFile = fopen(vProcFileName, "r");
	if (pProcFile)
	{
		// Creating the path "/data/data/PACKAGE_NAME/files/"
		strncpy(m_vDataPath, "/data/data/", 11);
		fread((char *)m_vDataPath+11, s_uMaxFilenameLength-11, 1, pProcFile);
		strncat(m_vDataPath, "/files/", 7);
		fclose(pProcFile);

		// Trying to create the data directory
		if (mkdir(m_vDataPath, 0770) && errno != EEXIST)
		{
			ncServiceLocator::Logger().Write(ncILogger::LOG_ERROR, (const char *)"ncIFile::DataPath - Cannot create directory: %s", m_vDataPath);
			memset(m_vDataPath, 0, s_uMaxFilenameLength);
		}
	}
#elif _WIN32
	char *pUserProfileEnv = getenv("USERPROFILE");
	if (pUserProfileEnv == NULL || strlen(pUserProfileEnv) == 0)
	{
		char *pHomeDriveEnv = getenv("HOMEDRIVE");
		char *pHomePathEnv = getenv("HOMEPATH");

		if ((pHomeDriveEnv == NULL || strlen(pHomeDriveEnv) == 0) &&
			(pHomePathEnv == NULL || strlen(pHomePathEnv) == 0))
		{
			char *pHomeEnv = getenv("HOME");
			if (pHomeEnv && strlen(pHomeEnv))
				strncpy(m_vDataPath, pHomeEnv, s_uMaxFilenameLength);
		}
		else
		{
			strncpy(m_vDataPath, pHomeDriveEnv, s_uMaxFilenameLength);
			strncat(m_vDataPath, pHomePathEnv, s_uMaxFilenameLength-strlen(pHomeDriveEnv));
		}
	}
	else
		strncpy(m_vDataPath, pUserProfileEnv, s_uMaxFilenameLength);

	if (strlen(m_vDataPath))
		strncat(m_vDataPath, "\\", 1);
#else
	char *pHomeEnv = getenv("HOME");

	if (pHomeEnv == NULL || strlen(pHomeEnv) == 0)
		strncpy(m_vDataPath, getpwuid(getuid())->pw_dir, s_uMaxFilenameLength);
	else
		strncpy(m_vDataPath, pHomeEnv, s_uMaxFilenameLength);

	strncat(m_vDataPath, "/.config/", s_uMaxFilenameLength - strlen(m_vDataPath));
#endif

	return m_vDataPath;
}
