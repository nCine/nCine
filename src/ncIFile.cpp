#include <stdlib.h> // for exit()
#include "ncIFile.h"
#include "ncStandardFile.h"

#ifdef __ANDROID__
#include "ncAssetFile.h"
#endif

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
		strncpy(m_vExtension, pDotChar+1, s_uMaxExtensionsLength);
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

/// Checks if a filename matches a particular extension
bool ncIFile::FileHasExtension(const char *pFilename, const char* pExtension)
{
	const char *pDotChar = strrchr(pFilename, '.');
	// A dot followed by at least three extension characters
	if (pDotChar && strlen(pDotChar) >= 4)
		return !strncmp(pDotChar+1, pExtension, s_uMaxExtensionsLength);
	else
		return false;
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
