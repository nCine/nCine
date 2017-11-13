#include <cstdlib> // for exit() and getenv()
#include <cstring> // for strlen()
#include "IFile.h"
#include "StandardFile.h"

#ifdef __ANDROID__
	#include <sys/stat.h> // for mkdir()
	#include <errno.h> // for EEXIST
	#include "AndroidApplication.h"
	#include "AssetFile.h"
	#include "ServiceLocator.h"
#endif

#ifndef _WIN32
	#include <unistd.h> // for getpid()
	#include <pwd.h> // for getpwuid()
#endif

namespace ncine {

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

String IFile::dataPath_(MaxFilenameLength);
String IFile::savePath_(MaxFilenameLength);

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

IFile::IFile(const char *filename)
	: type_(BASE_TYPE), filename_(filename), extension_(MaxExtensionLength),
	  fileDescriptor_(-1), filePointer_(NULL), shouldCloseOnDestruction_(true),
	  shouldExitOnFailToOpen_(true), fileSize_(0)
{
	int dotChar = filename_.findLastChar('.');
	// A dot followed by at least three extension characters
	if (dotChar && (filename_.length() - dotChar) >= 4)
	{
		extension_.copy(filename_, dotChar + 1, filename_.length() - dotChar - 1, 0);
	}
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

bool IFile::isOpened() const
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

bool IFile::hasExtension(const char *extension) const
{
	return extension_ == extension;
}

IFile *IFile::createFileHandle(const char *filename)
{
#ifdef __ANDROID__
	if (strncmp(filename, static_cast<const char *>("asset::"), 7) == 0)
	{
		return new AssetFile(filename + 7);
	}
	else
#endif
		return new StandardFile(filename);
}

bool IFile::access(const char *filename, unsigned char mode)
{
#ifdef __ANDROID__
	if (strncmp(filename, static_cast<const char *>("asset::"), 7) == 0)
	{
		return AssetFile::access(filename + 7, mode);
	}
	else
#endif
		return StandardFile::access(filename, mode);
}

const String &IFile::savePath()
{
	if (savePath_.isEmpty())
	{
		initSavePath();
	}

	return savePath_;
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void IFile::initSavePath()
{
#ifdef __ANDROID__
	nc::AndroidApplication &application = static_cast<nc::AndroidApplication &>(nc::theApplication());

	// Get the internal data path from the Android application
	savePath_ = application.internalDataPath();

	// Trying to create the data directory
	if (mkdir(savePath_.data(), 0770) && errno != EEXIST)
	{
		LOGE_X("Cannot create directory: %s", savePath_.data());
		savePath_.clear();
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
				savePath_ = homeEnv;
			}
		}
		else
		{
			savePath_ = homeDriveEnv;
			savePath_ += homePathEnv;
		}
	}
	else
	{
		savePath_ = userProfileEnv;
	}

	if (savePath_.isEmpty() == false)
	{
		savePath_ += "\\";
	}
#else
	char *homeEnv = getenv("HOME");

	if (homeEnv == NULL || strlen(homeEnv) == 0)
	{
		savePath_ = getpwuid(getuid())->pw_dir;
	}
	else
	{
		savePath_ = homeEnv;
	}

	savePath_ += "/.config/";
#endif
}

}
