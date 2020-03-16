#include "FileSystem.h"

#ifdef _WIN32
	#include "common_windefines.h"
	#include <windef.h>
	#include <WinBase.h>
	#include <fileapi.h>
	#include <Shlobj.h>
	#include <Timezoneapi.h>
#else
	#include <cerrno>
	#include <cstdio>
	#include <cstring>
	#include <ctime>
	#include <climits> // for `PATH_MAX`
	#include <unistd.h>
	#include <sys/stat.h>
	#include <libgen.h>
	#include <pwd.h>
	#include <dirent.h>
	#include <fcntl.h>
#endif

#if defined(__linux__)
	#include <sys/sendfile.h>
#endif

#ifdef __ANDROID__
	#include "AndroidApplication.h"
#endif

namespace ncine {

#ifdef _WIN32
const unsigned int FileSystem::MaxPathLength = MAX_PATH;
#else
const unsigned int FileSystem::MaxPathLength = PATH_MAX;
#endif

namespace {

	char buffer[fs::MaxPathLength];

#ifndef _WIN32
	bool callStat(const char *path, struct stat &sb)
	{
		ASSERT(path);
		if (::lstat(path, &sb) == -1)
		{
			LOGD_X("lstat error: %s", strerror(errno));
			return false;
		}
		return true;
	}

	int nativeModeToEnum(unsigned int nativeMode)
	{
		int mode = 0;

		if (nativeMode & S_IRUSR)
			mode += FileSystem::Permission::READ;
		if (nativeMode & S_IWUSR)
			mode += FileSystem::Permission::WRITE;
		if (nativeMode & S_IXUSR)
			mode += FileSystem::Permission::EXECUTE;

		return mode;
	}

	unsigned int addPermissionsToCurrent(unsigned int currentMode, int mode)
	{
		if (mode & FileSystem::Permission::READ)
			currentMode |= S_IRUSR;
		if (mode & FileSystem::Permission::WRITE)
			currentMode |= S_IWUSR;
		if (mode & FileSystem::Permission::EXECUTE)
			currentMode |= S_IXUSR;

		return currentMode;
	}

	unsigned int removePermissionsFromCurrent(unsigned int currentMode, int mode)
	{
		if (mode & FileSystem::Permission::READ)
			currentMode &= ~S_IRUSR;
		if (mode & FileSystem::Permission::WRITE)
			currentMode &= ~S_IWUSR;
		if (mode & FileSystem::Permission::EXECUTE)
			currentMode &= ~S_IXUSR;

		return currentMode;
	}

	FileSystem::FileDate nativeTimeToFileDate(const time_t *timer)
	{
		FileSystem::FileDate date = {};

		struct tm *local = localtime(timer);
		date.year = local->tm_year + 1900;
		date.month = local->tm_mon + 1;
		date.day = local->tm_mday;
		date.weekDay = local->tm_wday;
		date.hour = local->tm_hour;
		date.minute = local->tm_min;
		date.second = local->tm_sec;

		return date;
	}
#else
	size_t wrappedStrnlen(const char *str, size_t maxLen)
	{
	#if defined(_WIN32) && !defined(__MINGW32__)
		return strnlen_s(str, maxLen);
	#else
		return strnlen(str, maxLen);
	#endif
	}

	char *wrappedStrncpy(char *dest, size_t elements, const char *source, size_t count)
	{
	#if defined(_WIN32) && !defined(__MINGW32__)
		strncpy_s(dest, elements, source, count);
		return dest;
	#else
		return strncpy(dest, source, count);
	#endif
	}

	char *wrappedStrncat(char *dest, size_t elements, const char *source, size_t count)
	{
	#if defined(_WIN32) && !defined(__MINGW32__)
		strncat_s(dest, elements, source, count);
		return dest;
	#else
		return strncat(dest, source, count);
	#endif
	}

	FileSystem::FileDate nativeTimeToFileDate(const SYSTEMTIME *sysTime)
	{
		FileSystem::FileDate date = {};

		date.year = sysTime->wYear;
		date.month = sysTime->wMonth;
		date.day = sysTime->wDay;
		date.weekDay = sysTime->wDayOfWeek;
		date.hour = sysTime->wHour;
		date.minute = sysTime->wMinute;
		date.second = sysTime->wSecond;

		return date;
	}
#endif

}

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

nctl::String FileSystem::dataPath_(MaxPathLength);
nctl::String FileSystem::savePath_(MaxPathLength);

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

FileSystem::Directory::Directory(const char *path)
{
	open(path);
}

FileSystem::Directory::~Directory()
{
	close();
}

bool FileSystem::Directory::open(const char *path)
{
	close();

#ifdef _WIN32
	fileName_[0] = '\0';
	if (path && isDirectory(path))
	{
		WIN32_FIND_DATA findFileData;
		firstFile_ = true;
		strncpy_s(buffer, path, MaxPathLength);
		const unsigned long pathLength = wrappedStrnlen(buffer, MaxPathLength);

		if (pathLength + 2 <= MaxPathLength)
		{
			// Adding a wildcard to list all files in the directory
			buffer[pathLength + 0] = '\\';
			buffer[pathLength + 1] = '*';
			buffer[pathLength + 2] = '\0';

			hFindFile_ = FindFirstFileA(buffer, &findFileData);
			if (hFindFile_)
				strncpy_s(fileName_, findFileData.cFileName, MaxPathLength);
		}
	}
	return (hFindFile_ != NULL && hFindFile_ != INVALID_HANDLE_VALUE);
#else
	if (path)
		dirStream_ = opendir(path);
	return (dirStream_ != nullptr);
#endif
}

void FileSystem::Directory::close()
{
#ifdef _WIN32
	if (hFindFile_ != NULL && hFindFile_ != INVALID_HANDLE_VALUE)
	{
		FindClose(hFindFile_);
		hFindFile_ = NULL;
	}
#else
	if (dirStream_)
	{
		closedir(dirStream_);
		dirStream_ = nullptr;
	}
#endif
}

const char *FileSystem::Directory::readNext()
{
#ifdef _WIN32
	if (hFindFile_ == NULL || hFindFile_ == INVALID_HANDLE_VALUE)
		return nullptr;

	if (firstFile_)
	{
		firstFile_ = false;
		return fileName_;
	}
	else
	{
		WIN32_FIND_DATA findFileData;
		const int status = FindNextFileA(hFindFile_, &findFileData);
		if (status != 0)
		{
			strncpy_s(fileName_, findFileData.cFileName, MaxPathLength);
			return fileName_;
		}
		return nullptr;
	}
#else
	if (dirStream_ == nullptr)
		return nullptr;

	struct dirent *entry;
	entry = readdir(dirStream_);
	if (entry)
		return entry->d_name;
	else
		return nullptr;
#endif
}

nctl::String FileSystem::absoluteJoinPath(const nctl::String &first, const nctl::String &second)
{
	nctl::String joinedPath = joinPath(first, second);
#ifdef _WIN32
	const char *resolvedPath = _fullpath(buffer, joinedPath.data(), MaxPathLength);
#else
	const char *resolvedPath = ::realpath(joinedPath.data(), buffer);
#endif
	if (resolvedPath == nullptr)
		buffer[0] = '\0';

	return nctl::String(buffer);
}

nctl::String FileSystem::joinPath(const nctl::String &first, const nctl::String &second)
{
	if (first.isEmpty())
		return second;
	else if (second.isEmpty())
		return first;

	const bool firstHasSeparator = first[first.length() - 1] == '/' || first[first.length() - 1] == '\\';
	const bool secondHasSeparator = second[0] == '/' || second[0] == '\\';

	// One path has a trailing or leading separator, the other has not
	if (firstHasSeparator != secondHasSeparator)
		return first + second;
	// Both paths have no clashing separators
	else if (firstHasSeparator == false && secondHasSeparator == false)
#ifdef _WIN32
		return first + "\\" + second;
#else
		return first + "/" + second;
#endif
	else
	{
		// Both paths have a clashing separator, removing the leading one from the second path
		if (second.length() > 1)
			return first + &second[1];
		else
			return first;
	}
}

nctl::String FileSystem::dirName(const char *path)
{
	if (path == nullptr)
		return nctl::String();

#ifdef _WIN32
	static char drive[_MAX_DRIVE];
	static char dir[_MAX_DIR];

	_splitpath_s(path, drive, _MAX_DRIVE, dir, _MAX_DIR, nullptr, 0, nullptr, 0);

	wrappedStrncpy(buffer, MaxPathLength, drive, _MAX_DRIVE);
	wrappedStrncat(buffer, MaxPathLength, dir, _MAX_DIR);
	const unsigned long pathLength = wrappedStrnlen(buffer, MaxPathLength);
	// If the path only contains the drive letter the trailing separator is retained
	if (pathLength > 0 && (buffer[pathLength - 1] == '/' || buffer[pathLength - 1] == '\\') &&
	    (pathLength < 3 || (pathLength == 3 && buffer[1] != ':') || pathLength > 3))
	{
		buffer[pathLength - 1] = '\0';
	}

	return nctl::String(buffer);
#else
	strncpy(buffer, path, MaxPathLength - 1);
	return nctl::String(::dirname(buffer));
#endif
}

nctl::String FileSystem::baseName(const char *path)
{
	if (path == nullptr)
		return nctl::String();

#ifdef _WIN32
	static char fname[_MAX_FNAME];
	static char ext[_MAX_EXT];

	_splitpath_s(path, nullptr, 0, nullptr, 0, fname, _MAX_FNAME, ext, _MAX_EXT);

	wrappedStrncpy(buffer, MaxPathLength, fname, _MAX_FNAME);
	wrappedStrncat(buffer, MaxPathLength, ext, _MAX_EXT);
	return nctl::String(buffer);
#else
	strncpy(buffer, path, MaxPathLength - 1);
	return nctl::String(::basename(buffer));
#endif
}

nctl::String FileSystem::absolutePath(const char *path)
{
	if (path == nullptr)
		return nctl::String();

#ifdef _WIN32
	const char *resolvedPath = _fullpath(buffer, path, MaxPathLength);
#else
	const char *resolvedPath = ::realpath(path, buffer);
#endif
	if (resolvedPath == nullptr)
		buffer[0] = '\0';

	return nctl::String(buffer);
}

const char *FileSystem::extension(const char *path)
{
	if (path == nullptr)
		return nullptr;

	const char *subStr = strrchr(path, '.');
	if (subStr != nullptr)
	{
		const unsigned long pathLength = strnlen(path, MaxPathLength);
		const long extensionLength = path + pathLength - subStr - 1;

		// The extension is valid if it is 3 or 4 characters long without any space in it
		if ((extensionLength == 3 && subStr[1] != ' ' && subStr[2] != ' ' && subStr[3] != ' ') ||
		    (extensionLength == 4 && subStr[1] != ' ' && subStr[2] != ' ' && subStr[3] != ' ' && subStr[4] != ' '))
		{
			return subStr + 1;
		}
	}

	return nullptr;
}

bool FileSystem::hasExtension(const char *path, const char *extension)
{
	if (path == nullptr || extension == nullptr)
		return false;

	const char *pathExtension = FileSystem::extension(path);
	if (pathExtension != nullptr)
		return (strncmp(pathExtension, extension, 4) == 0);

	return false;
}

unsigned long FileSystem::logicalDrives()
{
#ifdef _WIN32
	return GetLogicalDrives();
#else
	return 0U;
#endif
}

const char *FileSystem::logicalDriveStrings()
{
#ifdef _WIN32
	// maximum number of drives * length of "A:\\\0"
	static char drivesString[26 * 4];
	const unsigned int numCopiedChars = GetLogicalDriveStringsA(26 * 4, drivesString);
	return (numCopiedChars > 0) ? drivesString : nullptr;
#else
	return nullptr;
#endif
}

nctl::String FileSystem::currentDir()
{
#ifdef _WIN32
	GetCurrentDirectory(MaxPathLength, buffer);
#else
	::getcwd(buffer, MaxPathLength);
#endif

	return nctl::String(buffer);
}

bool FileSystem::setCurrentDir(const char *path)
{
#ifdef _WIN32
	const int status = SetCurrentDirectory(path);
	return (status != 0);
#else
	const int status = ::chdir(path);
	return (status == 0);
#endif
}

nctl::String FileSystem::homeDir()
{
#ifdef _WIN32
	SHGetFolderPathA(HWND_DESKTOP, CSIDL_PROFILE, NULL, SHGFP_TYPE_CURRENT, buffer);
	return nctl::String(buffer);
#else
	const char *homeEnv = getenv("HOME");
	if (homeEnv == nullptr || strnlen(homeEnv, MaxPathLength) == 0)
	{
	#ifndef __EMSCRIPTEN__
		// `getpwuid()` is not yet implemented on Emscripten
		const struct passwd *pw = ::getpwuid(getuid());
		if (pw)
			return nctl::String(pw->pw_dir);
		else
	#endif
			return currentDir();
	}
	else
		return nctl::String(homeEnv);
#endif
}

bool FileSystem::isDirectory(const char *path)
{
	if (path == nullptr)
		return false;

#ifdef _WIN32
	const DWORD attrs = GetFileAttributesA(path);
	return (attrs != INVALID_FILE_ATTRIBUTES && attrs & FILE_ATTRIBUTE_DIRECTORY);
#else
	struct stat sb;
	const bool statCalled = callStat(path, sb);
	if (statCalled)
		return (sb.st_mode & S_IFMT) == S_IFDIR;
	return false;
#endif
}

bool FileSystem::isFile(const char *path)
{
	if (path == nullptr)
		return false;

#ifdef _WIN32
	const DWORD attrs = GetFileAttributesA(path);
	return (attrs != INVALID_FILE_ATTRIBUTES && (attrs & FILE_ATTRIBUTE_DIRECTORY) == 0);
#else
	struct stat sb;
	const bool statCalled = callStat(path, sb);
	if (statCalled)
		return (sb.st_mode & S_IFMT) == S_IFREG;
	return false;
#endif
}

bool FileSystem::exists(const char *path)
{
	if (path == nullptr)
		return false;

#ifdef _WIN32
	const DWORD attrs = GetFileAttributesA(path);
	return !(attrs == INVALID_FILE_ATTRIBUTES && GetLastError() == ERROR_FILE_NOT_FOUND);
#else
	struct stat sb;
	const bool statCalled = callStat(path, sb);
	return statCalled;
#endif
}

bool FileSystem::isReadable(const char *path)
{
	if (path == nullptr)
		return false;

#ifdef _WIN32
	// Assuming that every file that exists is also readable
	const DWORD attrs = GetFileAttributesA(path);
	return !(attrs == INVALID_FILE_ATTRIBUTES && GetLastError() == ERROR_FILE_NOT_FOUND);
#else
	struct stat sb;
	const bool statCalled = callStat(path, sb);
	if (statCalled)
		return (sb.st_mode & S_IRUSR);
	return false;
#endif
}

bool FileSystem::isWritable(const char *path)
{
	if (path == nullptr)
		return false;

#ifdef _WIN32
	const DWORD attrs = GetFileAttributesA(path);
	return (attrs != INVALID_FILE_ATTRIBUTES && (attrs & FILE_ATTRIBUTE_READONLY) == 0);
#else
	struct stat sb;
	const bool statCalled = callStat(path, sb);
	if (statCalled)
		return (sb.st_mode & S_IWUSR);
	return false;
#endif
}

bool FileSystem::isExecutable(const char *path)
{
	if (path == nullptr)
		return false;

#ifdef _WIN32
	// Assuming that every file that exists is also executable
	const DWORD attrs = GetFileAttributesA(path);
	// Assuming that every existing directory is accessible
	if (attrs != INVALID_FILE_ATTRIBUTES && attrs & FILE_ATTRIBUTE_DIRECTORY)
		return true;
	// Using some of the Windows executable extensions to detect executable files
	else if (attrs != INVALID_FILE_ATTRIBUTES &&
	         (hasExtension(path, "exe") || hasExtension(path, "bat") || hasExtension(path, "com")))
		return true;
	else
		return false;
#else
	return (::access(path, X_OK) == 0);
#endif
}

bool FileSystem::isReadableFile(const char *path)
{
	if (path == nullptr)
		return false;

#ifdef _WIN32
	const DWORD attrs = GetFileAttributesA(path);
	return (attrs != INVALID_FILE_ATTRIBUTES && (attrs & FILE_ATTRIBUTE_DIRECTORY) == 0);
#else
	struct stat sb;
	const bool statCalled = callStat(path, sb);
	if (statCalled)
		return (sb.st_mode & S_IFMT) == S_IFREG && (sb.st_mode & S_IRUSR);
#endif
	return false;
}

bool FileSystem::isWritableFile(const char *path)
{
	if (path == nullptr)
		return false;

#ifdef _WIN32
	const DWORD attrs = GetFileAttributesA(path);
	return (attrs != INVALID_FILE_ATTRIBUTES &&
	        (attrs & FILE_ATTRIBUTE_READONLY) == 0 &&
	        (attrs & FILE_ATTRIBUTE_DIRECTORY) == 0);
#else
	struct stat sb;
	const bool statCalled = callStat(path, sb);
	if (statCalled)
		return (sb.st_mode & S_IFMT) == S_IFREG && (sb.st_mode & S_IWUSR);
#endif
	return false;
}

bool FileSystem::isHidden(const char *path)
{
	if (path == nullptr)
		return false;

#ifdef _WIN32
	const DWORD attrs = GetFileAttributesA(path);
	return (attrs != INVALID_FILE_ATTRIBUTES && (attrs & FILE_ATTRIBUTE_HIDDEN));
#else
	strncpy(buffer, path, MaxPathLength - 1);
	const char *baseName = ::basename(buffer);
	return (baseName && baseName[0] == '.');
#endif
}

bool FileSystem::setHidden(const char *path, bool hidden)
{
	if (path == nullptr)
		return false;

#ifdef _WIN32
	DWORD attrs = GetFileAttributesA(path);

	// Adding the hidden flag
	if (hidden && (attrs & FILE_ATTRIBUTE_HIDDEN) == 0)
	{
		attrs |= FILE_ATTRIBUTE_HIDDEN;
		const int status = SetFileAttributesA(path, attrs);
		return (status != 0);
	}
	// Removing the hidden flag
	else if (hidden == false && (attrs & FILE_ATTRIBUTE_HIDDEN))
	{
		attrs &= ~FILE_ATTRIBUTE_HIDDEN;
		const int status = SetFileAttributesA(path, attrs);
		return (status != 0);
	}
#else
	strncpy(buffer, path, MaxPathLength - 1);
	const char *baseName = ::basename(buffer);
	if (hidden && baseName && baseName[0] != '.')
	{
		nctl::String newPath(MaxPathLength);
		newPath.assign(dirName(path));
		newPath.assign(joinPath(newPath.data(), "."));
		newPath.append(baseName);
		const int status = ::rename(path, newPath.data());
		return (status == 0);
	}
	else if (hidden == false && baseName && baseName[0] == '.')
	{
		int numDots = 0;
		while (baseName[numDots] == '.')
			numDots++;

		nctl::String newPath(MaxPathLength);
		newPath.assign(dirName(path));
		newPath.assign(joinPath(newPath.data(), &buffer[numDots]));
		const int status = ::rename(path, newPath.data());
		return (status == 0);
	}
#endif
	return false;
}

bool FileSystem::createDir(const char *path)
{
	if (path == nullptr)
		return false;

#ifdef _WIN32
	const int status = CreateDirectoryA(path, NULL);
	return (status != 0);
#else
	const int status = ::mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	return (status == 0);
#endif
}

bool FileSystem::deleteEmptyDir(const char *path)
{
	if (path == nullptr)
		return false;

#ifdef _WIN32
	const int status = RemoveDirectoryA(path);
	return (status != 0);
#else
	const int status = ::rmdir(path);
	return (status == 0);
#endif
}

bool FileSystem::deleteFile(const char *path)
{
	if (path == nullptr)
		return false;

#ifdef _WIN32
	const int status = DeleteFileA(path);
	return (status != 0);
#else
	const int status = ::unlink(path);
	return (status == 0);
#endif
}

bool FileSystem::rename(const char *oldPath, const char *newPath)
{
	if (oldPath == nullptr || newPath == nullptr)
		return false;

#ifdef _WIN32
	const int status = MoveFile(oldPath, newPath);
	return (status != 0);
#else
	const int status = ::rename(oldPath, newPath);
	return (status == 0);
#endif
}

bool FileSystem::copy(const char *oldPath, const char *newPath)
{
	if (oldPath == nullptr || newPath == nullptr)
		return false;

#ifdef _WIN32
	const int status = CopyFile(oldPath, newPath, TRUE);
	return (status != 0);
#elif defined(__linux__)
	int source, dest;
	off_t bytes = 0;
	struct stat sb;

	if ((source = open(oldPath, O_RDONLY)) == -1)
		return false;

	fstat(source, &sb);
	if ((dest = creat(newPath, sb.st_mode)) == -1)
	{
		close(source);
		return false;
	}

	const int status = sendfile(dest, source, &bytes, sb.st_size);

	close(source);
	close(dest);

	return (status != -1);
#else
	const size_t BufferSize = 4096;
	char buffer[BufferSize];

	int source, dest;
	size_t size = 0;
	struct stat sb;

	if ((source = open(oldPath, O_RDONLY)) == -1)
		return false;

	fstat(source, &sb);
	if ((dest = open(newPath, O_WRONLY | O_CREAT, sb.st_mode)) == -1)
	{
		close(source);
		return false;
	}

	while ((size = read(source, buffer, BufferSize)) > 0)
		write(dest, buffer, size);

	close(source);
	close(dest);

	return true;
#endif
}

long int FileSystem::fileSize(const char *path)
{
	if (path == nullptr)
		return -1;

#ifdef _WIN32
	HANDLE hFile = CreateFile(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
	LARGE_INTEGER fileSize;
	fileSize.QuadPart = 0;
	const int status = GetFileSizeEx(hFile, &fileSize);
	const int closed = CloseHandle(hFile);
	return (status != 0 && closed != 0) ? static_cast<long int>(fileSize.QuadPart) : -1;
#else
	struct stat sb;
	const bool statCalled = callStat(path, sb);
	if (statCalled == false)
		return -1;

	return sb.st_size;
#endif
}

FileSystem::FileDate FileSystem::lastModificationTime(const char *path)
{
	FileDate date = {};
#ifdef _WIN32
	HANDLE hFile = CreateFile(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
	FILETIME fileTime;
	const int status = GetFileTime(hFile, nullptr, nullptr, &fileTime);
	if (status != 0)
	{
		FILETIME localTime;
		SYSTEMTIME sysTime;
		FileTimeToLocalFileTime(&fileTime, &localTime);
		FileTimeToSystemTime(&localTime, &sysTime);
		date = nativeTimeToFileDate(&sysTime);
	}
	const int closed = CloseHandle(hFile);
#else
	struct stat sb;
	const bool statCalled = callStat(path, sb);
	if (statCalled)
		date = nativeTimeToFileDate(&sb.st_mtime);
#endif

	return date;
}

FileSystem::FileDate FileSystem::lastAccessTime(const char *path)
{
	FileDate date = {};
#ifdef _WIN32
	HANDLE hFile = CreateFile(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
	FILETIME fileTime;
	const int status = GetFileTime(hFile, nullptr, &fileTime, nullptr);
	if (status != 0)
	{
		FILETIME localTime;
		SYSTEMTIME sysTime;
		FileTimeToLocalFileTime(&fileTime, &localTime);
		FileTimeToSystemTime(&localTime, &sysTime);
		date = nativeTimeToFileDate(&sysTime);
	}
	const int closed = CloseHandle(hFile);
#else
	struct stat sb;
	const bool statCalled = callStat(path, sb);
	if (statCalled)
		date = nativeTimeToFileDate(&sb.st_atime);
#endif

	return date;
}

int FileSystem::permissions(const char *path)
{
	if (path == nullptr)
		return 0;

#ifdef _WIN32
	int mode = Permission::READ;
	mode += isExecutable(path) ? Permission::EXECUTE : 0;
	const DWORD attrs = GetFileAttributesA(path);
	if (attrs != INVALID_FILE_ATTRIBUTES && (attrs & FILE_ATTRIBUTE_READONLY) == 0)
		mode += Permission::WRITE;
	return mode;
#else
	struct stat sb;
	const bool statCalled = callStat(path, sb);
	if (statCalled == false)
		return 0;

	return nativeModeToEnum(sb.st_mode);
#endif
}

bool FileSystem::changePermissions(const char *path, int mode)
{
	if (path == nullptr)
		return false;

#ifdef _WIN32
	DWORD attrs = GetFileAttributesA(path);
	if (attrs != INVALID_FILE_ATTRIBUTES)
	{
		// Adding the write permission
		if ((mode & Permission::WRITE) && (attrs & FILE_ATTRIBUTE_READONLY))
		{
			attrs &= ~FILE_ATTRIBUTE_READONLY;
			const int status = SetFileAttributesA(path, attrs);
			return (status != 0);
		}
		// Removing the write permission
		else if ((mode & Permission::WRITE) == 0 && (attrs & FILE_ATTRIBUTE_READONLY) == 0)
		{
			attrs |= FILE_ATTRIBUTE_READONLY;
			const int status = SetFileAttributesA(path, attrs);
			return (status != 0);
		}
		return true;
	}
	return false;
#else
	struct stat sb;
	const bool statCalled = callStat(path, sb);
	if (statCalled == false)
		return false;

	const unsigned int currentMode = sb.st_mode;
	unsigned int newMode = addPermissionsToCurrent(currentMode, mode);
	const int complementMode = EXECUTE + WRITE + READ - mode;
	newMode = removePermissionsFromCurrent(newMode, complementMode);

	const int status = chmod(path, newMode);
	return (status == 0);
#endif
}

bool FileSystem::addPermissions(const char *path, int mode)
{
	if (path == nullptr)
		return false;

#ifdef _WIN32
	DWORD attrs = GetFileAttributesA(path);
	if (attrs != INVALID_FILE_ATTRIBUTES)
	{
		// Adding the write permission
		if ((mode & Permission::WRITE) && (attrs & FILE_ATTRIBUTE_READONLY))
		{
			attrs &= ~FILE_ATTRIBUTE_READONLY;
			const int status = SetFileAttributesA(path, attrs);
			return (status != 0);
		}
		return true;
	}
	return false;
#else
	struct stat sb;
	const bool statCalled = callStat(path, sb);
	if (statCalled == false)
		return false;

	const unsigned int currentMode = sb.st_mode;
	const unsigned int newMode = addPermissionsToCurrent(currentMode, mode);
	const int status = chmod(path, newMode);
	return (status == 0);
#endif
}

bool FileSystem::removePermissions(const char *path, int mode)
{
	if (path == nullptr)
		return false;

#ifdef _WIN32
	DWORD attrs = GetFileAttributesA(path);
	if (attrs != INVALID_FILE_ATTRIBUTES)
	{
		// Removing the write permission
		if ((mode & Permission::WRITE) && (attrs & FILE_ATTRIBUTE_READONLY) == 0)
		{
			attrs |= FILE_ATTRIBUTE_READONLY;
			const int status = SetFileAttributesA(path, attrs);
			return (status != 0);
		}
		return true;
	}
	return false;
#else
	struct stat sb;
	const bool statCalled = callStat(path, sb);
	if (statCalled == false)
		return false;

	const unsigned int currentMode = sb.st_mode;
	const unsigned int newMode = removePermissionsFromCurrent(currentMode, mode);
	const int status = chmod(path, newMode);
	return (status == 0);
#endif
}

const nctl::String &FileSystem::savePath()
{
	if (savePath_.isEmpty())
		initSavePath();

	return savePath_;
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void FileSystem::initSavePath()
{
#if defined(__ANDROID__)
	AndroidApplication &application = static_cast<AndroidApplication &>(theApplication());

	// Get the internal data path from the Android application
	savePath_ = application.internalDataPath();

	// Trying to create the data directory
	if (createDir(savePath_.data()) == false)
	{
		LOGE_X("Cannot create directory: %s", savePath_.data());
		savePath_.clear();
	}
#elif defined(_WIN32)
	const char *userProfileEnv = getenv("USERPROFILE");
	if (userProfileEnv == nullptr || strlen(userProfileEnv) == 0)
	{
		const char *homeDriveEnv = getenv("HOMEDRIVE");
		const char *homePathEnv = getenv("HOMEPATH");

		if ((homeDriveEnv == nullptr || strlen(homeDriveEnv) == 0) &&
		    (homePathEnv == nullptr || strlen(homePathEnv) == 0))
		{
			const char *homeEnv = getenv("HOME");
			if (homeEnv && wrappedStrnlen(homeEnv, MaxPathLength))
				savePath_ = homeEnv;
		}
		else
		{
			savePath_ = homeDriveEnv;
			savePath_ += homePathEnv;
		}
	}
	else
		savePath_ = userProfileEnv;

	if (savePath_.isEmpty() == false)
		savePath_ += "\\";
#else
	const char *homeEnv = getenv("HOME");

	if (homeEnv == nullptr || strnlen(homeEnv, MaxPathLength) == 0)
		savePath_ = getpwuid(getuid())->pw_dir;
	else
		savePath_ = homeEnv;

	savePath_ += "/.config/";
#endif
}

}
