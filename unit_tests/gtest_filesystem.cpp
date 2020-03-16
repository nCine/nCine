#include "gtest_filesystem.h"

namespace {

class FileSystemTest : public ::testing::Test
{};

TEST_F(FileSystemTest, JoinEmptyPaths)
{
	printf("Joining two empty paths\n");
	nctl::String firstPath;
	nctl::String secondPath;
	nctl::String joinedPath = nc::fs::joinPath(firstPath, secondPath);
	printf("\"%s\" + \"%s\": \"%s\"\n", firstPath.data(), secondPath.data(), joinedPath.data());

	ASSERT_TRUE(firstPath.isEmpty());
	ASSERT_TRUE(secondPath.isEmpty());
	ASSERT_TRUE(joinedPath.isEmpty());
}

TEST_F(FileSystemTest, JoinPathFirstEmpty)
{
	printf("Joining two paths, the first is empty\n");
	nctl::String firstPath;
	nctl::String secondPath = nc::fs::currentDir();
	nctl::String joinedPath = nc::fs::joinPath(firstPath, secondPath);
	printf("\"%s\" + \"%s\": \"%s\"\n", firstPath.data(), secondPath.data(), joinedPath.data());

	ASSERT_TRUE(firstPath.isEmpty());
	ASSERT_FALSE(secondPath.isEmpty());
	ASSERT_FALSE(joinedPath.isEmpty());

	ASSERT_TRUE(nc::fs::isDirectory(secondPath.data()));
	ASSERT_TRUE(nc::fs::isDirectory(joinedPath.data()));
}

TEST_F(FileSystemTest, JoinPathSecondEmpty)
{
	printf("Joining two paths, the second is empty\n");
	nctl::String firstPath = nc::fs::currentDir();
	nctl::String secondPath;
	nctl::String joinedPath = nc::fs::joinPath(firstPath, secondPath);
	printf("\"%s\" + \"%s\": \"%s\"\n", firstPath.data(), secondPath.data(), joinedPath.data());

	ASSERT_FALSE(firstPath.isEmpty());
	ASSERT_TRUE(secondPath.isEmpty());
	ASSERT_FALSE(joinedPath.isEmpty());

	ASSERT_TRUE(nc::fs::isDirectory(firstPath.data()));
	ASSERT_TRUE(nc::fs::isDirectory(joinedPath.data()));
}

TEST_F(FileSystemTest, JoinPathSeparators)
{
	printf("Joining two paths made of just the separator\n");
	nctl::String firstPath = "/";
	nctl::String secondPath = "/";
	nctl::String joinedPath = nc::fs::joinPath(firstPath, secondPath);
	printf("\"%s\" + \"%s\": \"%s\"\n", firstPath.data(), secondPath.data(), joinedPath.data());

	ASSERT_STREQ(joinedPath.data(), "/");
}

TEST_F(FileSystemTest, JoinPathFirstHasSeparator)
{
	printf("Joining two paths where the first one has a trailing separator\n");
	nctl::String firstPath = "first/path/";
	nctl::String secondPath = "second/path";
	nctl::String joinedPath = nc::fs::joinPath(firstPath, secondPath);
	printf("\"%s\" + \"%s\": \"%s\"\n", firstPath.data(), secondPath.data(), joinedPath.data());

	ASSERT_STREQ(joinedPath.data(), "first/path/second/path");
}

TEST_F(FileSystemTest, JoinPathSecondHasSeparator)
{
	printf("Joining two paths where the second one has a leading separator\n");
	nctl::String firstPath = "first/path";
	nctl::String secondPath = "/second/path";
	nctl::String joinedPath = nc::fs::joinPath(firstPath, secondPath);
	printf("\"%s\" + \"%s\": \"%s\"\n", firstPath.data(), secondPath.data(), joinedPath.data());

	ASSERT_STREQ(joinedPath.data(), "first/path/second/path");
}

TEST_F(FileSystemTest, JoinPathBothHaveSeparator)
{
	printf("Joining two paths where the first has a trailing and the second has a leading separator\n");
	nctl::String firstPath = "first/path/";
	nctl::String secondPath = "/second/path";
	nctl::String joinedPath = nc::fs::joinPath(firstPath, secondPath);
	printf("\"%s\" + \"%s\": \"%s\"\n", firstPath.data(), secondPath.data(), joinedPath.data());

	ASSERT_STREQ(joinedPath.data(), "first/path/second/path");
}

#ifndef __EMSCRIPTEN__
// `realpath()` does not seem to work on Emscripten
TEST_F(FileSystemTest, JoinPathDotFiles)
{
	printf("Joining two paths with dot files\n");
	nctl::String firstPath = nc::fs::joinPath(nc::fs::currentDir(), ".");
	nctl::String secondPath = ".";
	nctl::String joinedPath = nc::fs::joinPath(firstPath, secondPath);
	printf("\"%s\" + \"%s\": \"%s\"\n", firstPath.data(), secondPath.data(), joinedPath.data());
	nctl::String absJoinedPath = nc::fs::absoluteJoinPath(firstPath, secondPath);

	ASSERT_TRUE(nc::fs::isDirectory(joinedPath.data()));
	ASSERT_TRUE(nc::fs::isDirectory(absJoinedPath.data()));
	ASSERT_STRNE(joinedPath.data(), absJoinedPath.data());
	ASSERT_STREQ(nc::fs::currentDir().data(), absJoinedPath.data());
}

TEST_F(FileSystemTest, BaseAndDirName)
{
	nctl::String currentDir = nc::fs::currentDir();
	printf("Splitting current directory into basename and dirname\n");

	nctl::String dirName = nc::fs::dirName(currentDir.data());
	nctl::String baseName = nc::fs::baseName(currentDir.data());
	printf("\"%s\" + \"%s\": \"%s\"\n", dirName.data(), baseName.data(), currentDir.data());
	nctl::String joinedPath = nc::fs::joinPath(dirName, baseName);

	ASSERT_STREQ(currentDir.data(), joinedPath.data());
}

TEST_F(FileSystemTest, RealPath)
{
	nctl::String currentDir = nc::fs::currentDir();

	nctl::String baseName = nc::fs::baseName(currentDir.data());
	nctl::String joinedPath = nc::fs::joinPath(currentDir, "..");
	joinedPath = nc::fs::joinPath(joinedPath, baseName);
	joinedPath = nc::fs::joinPath(joinedPath, ".");

	printf("Altered path: \"%s\"\n", joinedPath.data());
	joinedPath = nc::fs::absolutePath(joinedPath.data());
	printf("Real path: \"%s\"\n", joinedPath.data());
	ASSERT_STREQ(currentDir.data(), joinedPath.data());
}

TEST_F(FileSystemTest, AbsolutePath)
{
	nctl::String currentDir = nc::fs::currentDir();
	nctl::String absolutePath = nc::fs::absolutePath(".");
	printf("Absolute path for \".\": \"%s\"\n", absolutePath.data());
	ASSERT_STREQ(currentDir.data(), absolutePath.data());

	nctl::String dirName = nc::fs::dirName(currentDir.data());
	absolutePath = nc::fs::absolutePath("..");
	printf("Absolute path for \"..\": \"%s\"\n", absolutePath.data());
	ASSERT_STREQ(dirName.data(), absolutePath.data());
}
#endif

TEST_F(FileSystemTest, Extension)
{
	nctl::String currentDir = nc::fs::currentDir();

	nctl::String file = nc::fs::joinPath(currentDir, "document.txt");
	const char *extension = nc::fs::extension(file.data());
	printf("File \"%s\" has \"%s\" as extension\n", file.data(), extension);
	ASSERT_NE(extension, nullptr);
	ASSERT_STREQ(extension, "txt");
	ASSERT_TRUE(nc::fs::hasExtension(file.data(), "txt"));

	file = nc::fs::joinPath(currentDir, "image.jpeg");
	extension = nc::fs::extension(file.data());
	printf("File \"%s\" has \"%s\" as extension\n", file.data(), extension);
	ASSERT_NE(extension, nullptr);
	ASSERT_STREQ(extension, "jpeg");
	ASSERT_TRUE(nc::fs::hasExtension(file.data(), "jpeg"));

	file = nc::fs::joinPath(currentDir, "file.a b");
	extension = nc::fs::extension(file.data());
	printf("File \"%s\" has no valid extension (contains space)\n", file.data());
	ASSERT_EQ(extension, nullptr);
	ASSERT_FALSE(nc::fs::hasExtension(file.data(), "a b"));

	file = nc::fs::joinPath(currentDir, "file.toolong");
	extension = nc::fs::extension(file.data());
	printf("File \"%s\" has no valid extension (too long)\n", file.data());
	ASSERT_EQ(extension, nullptr);
	ASSERT_FALSE(nc::fs::hasExtension(file.data(), "toolong"));
}

TEST_F(FileSystemTest, LogicalDrives)
{
#ifndef _WIN32
	unsigned long logicalDrives = nc::fs::logicalDrives();
	ASSERT_EQ(logicalDrives, 0U);

	const char *logicalDriveStrings = nc::fs::logicalDriveStrings();
	ASSERT_EQ(logicalDriveStrings, nullptr);
#else
	unsigned long logicalDrives = nc::fs::logicalDrives();
	ASSERT_NE(logicalDrives, 0U);

	const char *logicalDriveStrings = nc::fs::logicalDriveStrings();
	ASSERT_NE(logicalDriveStrings, nullptr);

	printf("Logical drives in the system:");
	const char *driveString = logicalDriveStrings;
	while (driveString && *driveString != '\0')
	{
		printf(" \"%s\"\n", driveString);
		driveString += strnlen(driveString, 4);
	}
#endif
}

TEST_F(FileSystemTest, CurrentDir)
{
	nctl::String currentDir = nc::fs::currentDir();
	printf("Current directory is \"%s\"\n", currentDir.data());
	ASSERT_TRUE(nc::fs::isDirectory(currentDir.data()));
}

TEST_F(FileSystemTest, SetCurrentDir)
{
	nctl::String currentDir = nc::fs::currentDir();
	const nctl::String oldCurrentDir = currentDir;
	printf("Current directory is \"%s\"\n", currentDir.data());
	ASSERT_TRUE(nc::fs::isDirectory(currentDir.data()));

	currentDir = nc::fs::dirName(currentDir.data());
	bool dirChanged = nc::fs::setCurrentDir(currentDir.data());
	ASSERT_TRUE(dirChanged);
	printf("Current directory changed to \"%s\"\n", currentDir.data());
	ASSERT_TRUE(nc::fs::isDirectory(currentDir.data()));
	ASSERT_STREQ(currentDir.data(), nc::fs::currentDir().data());

	dirChanged = nc::fs::setCurrentDir(oldCurrentDir.data());
	ASSERT_TRUE(dirChanged);
}

TEST_F(FileSystemTest, HomeDir)
{
	nctl::String homeDir = nc::fs::homeDir();
	printf("Home directory is \"%s\"\n", homeDir.data());
	ASSERT_TRUE(nc::fs::isDirectory(homeDir.data()));
}

TEST_F(FileSystemTest, Hidden)
{
	printf("Creating a new file: \"%s\"\n", FileName);
	touchFile(FileName);

	ASSERT_TRUE(nc::fs::exists(FileName));
	ASSERT_TRUE(nc::fs::isFile(FileName));
	ASSERT_FALSE(nc::fs::isHidden(FileName));

	printf("Hiding the file\n");
	nc::fs::setHidden(FileName, true);
#ifndef _WIN32
	nctl::String HiddenName;
	HiddenName.format(".%s", FileName);
	ASSERT_TRUE(nc::fs::isHidden(HiddenName.data()));
	ASSERT_FALSE(nc::fs::exists(FileName));
	ASSERT_TRUE(nc::fs::exists(HiddenName.data()));
#else
	ASSERT_TRUE(nc::fs::isHidden(FileName));
#endif

	printf("Making the file visible again\n");
#ifndef _WIN32
	nc::fs::setHidden(HiddenName.data(), false);
	ASSERT_TRUE(nc::fs::exists(FileName));
	ASSERT_FALSE(nc::fs::exists(HiddenName.data()));
#else
	nc::fs::setHidden(FileName, false);
#endif
	ASSERT_FALSE(nc::fs::isHidden(FileName));

	const bool fileDeleted = nc::fs::deleteFile(FileName);
	ASSERT_TRUE(fileDeleted);
}

TEST_F(FileSystemTest, CreateDeleteDirectory)
{
	printf("Creating a new directory: \"%s\"\n", DirectoryName);
	const bool dirCreated = nc::fs::createDir(DirectoryName);
	ASSERT_TRUE(dirCreated);

	ASSERT_TRUE(nc::fs::exists(DirectoryName));
	ASSERT_TRUE(nc::fs::isDirectory(DirectoryName));
	ASSERT_FALSE(nc::fs::isFile(DirectoryName));
	ASSERT_FALSE(nc::fs::isReadableFile(DirectoryName));
	ASSERT_FALSE(nc::fs::isWritableFile(DirectoryName));
	ASSERT_TRUE(nc::fs::isReadable(DirectoryName));
	ASSERT_TRUE(nc::fs::isWritable(DirectoryName));
	ASSERT_TRUE(nc::fs::isExecutable(DirectoryName));

	printf("Deleting directory: \"%s\"\n", DirectoryName);
	const bool dirDeleted = nc::fs::deleteEmptyDir(DirectoryName);
	ASSERT_TRUE(dirDeleted);

	ASSERT_FALSE(nc::fs::exists(DirectoryName));
	ASSERT_FALSE(nc::fs::isDirectory(DirectoryName));
}

TEST_F(FileSystemTest, CreateDeleteFile)
{
	printf("Creating a new file: \"%s\"\n", FileName);
	touchFile(FileName);

	ASSERT_TRUE(nc::fs::exists(FileName));
	ASSERT_TRUE(nc::fs::isFile(FileName));
	ASSERT_FALSE(nc::fs::isDirectory(FileName));

	printf("Deleting file: \"%s\"\n", FileName);
	const bool fileDeleted = nc::fs::deleteFile(FileName);
	ASSERT_TRUE(fileDeleted);

	ASSERT_FALSE(nc::fs::exists(FileName));
	ASSERT_FALSE(nc::fs::isFile(FileName));
}

TEST_F(FileSystemTest, RenameFile)
{
	printf("Creating a new file: \"%s\"\n", FileName);
	touchFile(FileName);

	printf("Renaming the file to \"%s\"\n", FileRenamed);
	const bool fileRenamed = nc::fs::rename(FileName, FileRenamed);
	ASSERT_TRUE(fileRenamed);
	ASSERT_FALSE(nc::fs::exists(FileName));
	ASSERT_FALSE(nc::fs::isFile(FileName));
	ASSERT_TRUE(nc::fs::exists(FileRenamed));
	ASSERT_TRUE(nc::fs::isFile(FileRenamed));

	bool fileDeleted = nc::fs::deleteFile(FileName);
	ASSERT_FALSE(fileDeleted);
	fileDeleted = nc::fs::deleteFile(FileRenamed);
	ASSERT_TRUE(fileDeleted);
}

TEST_F(FileSystemTest, CopyFile)
{
	const int FileSize = 64;
	printf("Creating a new file: \"%s\"\n", FileName);
	fillFile(FileName, FileSize);

	printf("Copying the file to \"%s\"\n", FileRenamed);
	const bool fileCopied = nc::fs::copy(FileName, FileRenamed);
	ASSERT_TRUE(fileCopied);
	ASSERT_TRUE(nc::fs::exists(FileName));
	ASSERT_TRUE(nc::fs::isFile(FileName));
	ASSERT_EQ(nc::fs::fileSize(FileName), FileSize);
	ASSERT_TRUE(nc::fs::exists(FileRenamed));
	ASSERT_TRUE(nc::fs::isFile(FileRenamed));
	ASSERT_EQ(nc::fs::fileSize(FileRenamed), FileSize);
	ASSERT_EQ(nc::fs::fileSize(FileName), nc::fs::fileSize(FileRenamed));

	bool fileDeleted = nc::fs::deleteFile(FileName);
	ASSERT_TRUE(fileDeleted);
	fileDeleted = nc::fs::deleteFile(FileRenamed);
	ASSERT_TRUE(fileDeleted);
}

TEST_F(FileSystemTest, FileSize)
{
	for (unsigned int i = 1; i <= 4; i++)
	{
		int fileSize = 32 * i;
		printf("Creating a file that is %d bytes long\n", fileSize);
		fillFile(FileName, fileSize);
		ASSERT_EQ(nc::fs::fileSize(FileName), fileSize);

		bool fileDeleted = nc::fs::deleteFile(FileName);
		ASSERT_TRUE(fileDeleted);
	}
}

TEST_F(FileSystemTest, FileModificationDate)
{
	printf("Creating a new file: \"%s\"\n", FileName);
	time_t timer = time(nullptr);
	touchFile(FileName);

	struct tm *localTime = localtime(&timer);
	nc::fs::FileDate date = nc::fs::lastModificationTime(FileName);
	printf("Last modified on %.2d/%.2d/%.4d at %.2d:%.2d:%.2d\n", date.day, date.month, date.year, date.hour, date.minute, date.second);

	ASSERT_EQ(localTime->tm_year + 1900, date.year);
	ASSERT_EQ(localTime->tm_mon + 1, date.month);
	ASSERT_EQ(localTime->tm_mday, date.day);
	ASSERT_EQ(localTime->tm_wday, date.weekDay);
	ASSERT_EQ(localTime->tm_hour, date.hour);
	ASSERT_EQ(localTime->tm_min, date.minute);
	ASSERT_EQ(localTime->tm_sec, date.second);

	bool fileDeleted = nc::fs::deleteFile(FileName);
	ASSERT_TRUE(fileDeleted);
}

TEST_F(FileSystemTest, FileAccessDate)
{
	printf("Creating a new file: \"%s\"\n", FileName);
	time_t timer = time(nullptr);
	touchFile(FileName);

	struct tm *localTime = localtime(&timer);
	nc::fs::FileDate date = nc::fs::lastAccessTime(FileName);
	printf("Last accessed on %.2d/%.2d/%.4d at %.2d:%.2d:%.2d\n", date.day, date.month, date.year, date.hour, date.minute, date.second);

	ASSERT_EQ(localTime->tm_year + 1900, date.year);
	ASSERT_EQ(localTime->tm_mon + 1, date.month);
	ASSERT_EQ(localTime->tm_mday, date.day);
	ASSERT_EQ(localTime->tm_wday, date.weekDay);
	ASSERT_EQ(localTime->tm_hour, date.hour);
	ASSERT_EQ(localTime->tm_min, date.minute);
	ASSERT_EQ(localTime->tm_sec, date.second);

	bool fileDeleted = nc::fs::deleteFile(FileName);
	ASSERT_TRUE(fileDeleted);
}

#ifndef _WIN32
TEST_F(FileSystemTest, AddRemovePermissionsFile)
{
	printf("Creating a new file: \"%s\"\n", FileName);
	touchFile(FileName);

	ASSERT_TRUE(nc::fs::exists(FileName));
	ASSERT_TRUE(nc::fs::isFile(FileName));

	printf("Remove read permission from file\n");
	bool permissionChanged = nc::fs::removePermissions(FileName, nc::fs::READ);
	ASSERT_TRUE(permissionChanged);
	ASSERT_FALSE(nc::fs::isReadable(FileName));
	ASSERT_FALSE(nc::fs::isReadableFile(FileName));

	printf("Remove write permission from file\n");
	permissionChanged = nc::fs::removePermissions(FileName, nc::fs::WRITE);
	ASSERT_TRUE(permissionChanged);
	ASSERT_FALSE(nc::fs::isWritable(FileName));
	ASSERT_FALSE(nc::fs::isWritableFile(FileName));

	printf("Remove executable permission from file\n");
	permissionChanged = nc::fs::removePermissions(FileName, nc::fs::EXECUTE);
	ASSERT_TRUE(permissionChanged);
	ASSERT_FALSE(nc::fs::isExecutable(FileName));

	printf("Add read permission to file\n");
	permissionChanged = nc::fs::addPermissions(FileName, nc::fs::READ);
	ASSERT_TRUE(permissionChanged);
	ASSERT_TRUE(nc::fs::isReadable(FileName));
	ASSERT_FALSE(nc::fs::isWritable(FileName));
	ASSERT_FALSE(nc::fs::isExecutable(FileName));

	printf("Add write permission to file\n");
	permissionChanged = nc::fs::addPermissions(FileName, nc::fs::WRITE);
	ASSERT_TRUE(permissionChanged);
	ASSERT_TRUE(nc::fs::isReadable(FileName));
	ASSERT_TRUE(nc::fs::isWritable(FileName));
	ASSERT_FALSE(nc::fs::isExecutable(FileName));

	printf("Add executable permission to file\n");
	permissionChanged = nc::fs::addPermissions(FileName, nc::fs::EXECUTE);
	ASSERT_TRUE(permissionChanged);
	ASSERT_TRUE(nc::fs::isReadable(FileName));
	ASSERT_TRUE(nc::fs::isWritable(FileName));
	ASSERT_TRUE(nc::fs::isExecutable(FileName));

	const bool fileDeleted = nc::fs::deleteFile(FileName);
	ASSERT_TRUE(fileDeleted);

	ASSERT_FALSE(nc::fs::exists(FileName));
	ASSERT_FALSE(nc::fs::isFile(FileName));
}

TEST_F(FileSystemTest, ChangePermissionsFile)
{
	printf("Creating a new file: \"%s\"\n", FileName);
	touchFile(FileName);

	ASSERT_TRUE(nc::fs::exists(FileName));
	ASSERT_TRUE(nc::fs::isFile(FileName));

	printf("Make the file read only\n");
	bool permissionChanged = nc::fs::changePermissions(FileName, nc::fs::READ);
	ASSERT_TRUE(permissionChanged);
	ASSERT_TRUE(nc::fs::isReadable(FileName));
	ASSERT_TRUE(nc::fs::isReadableFile(FileName));
	ASSERT_FALSE(nc::fs::isWritable(FileName));
	ASSERT_FALSE(nc::fs::isExecutable(FileName));

	int permissions = nc::fs::permissions(FileName);
	ASSERT_TRUE(permissions & nc::fs::Permission::READ);
	ASSERT_FALSE(permissions & nc::fs::Permission::WRITE);
	ASSERT_FALSE(permissions & nc::fs::Permission::EXECUTE);

	printf("Make the file write only\n");
	permissionChanged = nc::fs::changePermissions(FileName, nc::fs::WRITE);
	ASSERT_TRUE(permissionChanged);
	ASSERT_FALSE(nc::fs::isReadable(FileName));
	ASSERT_FALSE(nc::fs::isReadableFile(FileName));
	ASSERT_TRUE(nc::fs::isWritable(FileName));
	ASSERT_TRUE(nc::fs::isWritableFile(FileName));
	ASSERT_FALSE(nc::fs::isExecutable(FileName));

	permissions = nc::fs::permissions(FileName);
	ASSERT_FALSE(permissions & nc::fs::Permission::READ);
	ASSERT_TRUE(permissions & nc::fs::Permission::WRITE);
	ASSERT_FALSE(permissions & nc::fs::Permission::EXECUTE);

	printf("Make the file executable only\n");
	permissionChanged = nc::fs::changePermissions(FileName, nc::fs::EXECUTE);
	ASSERT_TRUE(permissionChanged);
	ASSERT_FALSE(nc::fs::isReadable(FileName));
	ASSERT_FALSE(nc::fs::isWritable(FileName));
	ASSERT_TRUE(nc::fs::isExecutable(FileName));

	permissions = nc::fs::permissions(FileName);
	ASSERT_FALSE(permissions & nc::fs::Permission::READ);
	ASSERT_FALSE(permissions & nc::fs::Permission::WRITE);
	ASSERT_TRUE(permissions & nc::fs::Permission::EXECUTE);

	printf("Make the file readable and writable\n");
	permissionChanged = nc::fs::changePermissions(FileName, nc::fs::READ | nc::fs::WRITE);
	ASSERT_TRUE(permissionChanged);
	ASSERT_TRUE(nc::fs::isReadable(FileName));
	ASSERT_TRUE(nc::fs::isWritable(FileName));
	ASSERT_FALSE(nc::fs::isExecutable(FileName));

	permissions = nc::fs::permissions(FileName);
	ASSERT_TRUE(permissions & nc::fs::Permission::READ);
	ASSERT_TRUE(permissions & nc::fs::Permission::WRITE);
	ASSERT_FALSE(permissions & nc::fs::Permission::EXECUTE);

	printf("Make the file writable and executable\n");
	permissionChanged = nc::fs::changePermissions(FileName, nc::fs::WRITE | nc::fs::EXECUTE);
	ASSERT_TRUE(permissionChanged);
	ASSERT_FALSE(nc::fs::isReadable(FileName));
	ASSERT_TRUE(nc::fs::isWritable(FileName));
	ASSERT_TRUE(nc::fs::isExecutable(FileName));

	permissions = nc::fs::permissions(FileName);
	ASSERT_FALSE(permissions & nc::fs::Permission::READ);
	ASSERT_TRUE(permissions & nc::fs::Permission::WRITE);
	ASSERT_TRUE(permissions & nc::fs::Permission::EXECUTE);

	printf("Make the file readable, writable, and executable\n");
	permissionChanged = nc::fs::changePermissions(FileName, nc::fs::READ | nc::fs::WRITE | nc::fs::EXECUTE);
	ASSERT_TRUE(permissionChanged);
	ASSERT_TRUE(nc::fs::isReadable(FileName));
	ASSERT_TRUE(nc::fs::isWritable(FileName));
	ASSERT_TRUE(nc::fs::isExecutable(FileName));

	permissions = nc::fs::permissions(FileName);
	ASSERT_TRUE(permissions & nc::fs::Permission::READ);
	ASSERT_TRUE(permissions & nc::fs::Permission::WRITE);
	ASSERT_TRUE(permissions & nc::fs::Permission::EXECUTE);

	const bool fileDeleted = nc::fs::deleteFile(FileName);
	ASSERT_TRUE(fileDeleted);

	ASSERT_FALSE(nc::fs::exists(FileName));
	ASSERT_FALSE(nc::fs::isFile(FileName));
}
#else
TEST_F(FileSystemTest, AddRemovePermissionsFile)
{
	printf("Creating a new file: \"%s\"\n", FileName);
	touchFile(FileName);

	ASSERT_TRUE(nc::fs::exists(FileName));
	ASSERT_TRUE(nc::fs::isFile(FileName));
	ASSERT_FALSE(nc::fs::isExecutable(FileName));

	printf("Remove write permission from file\n");
	bool permissionChanged = nc::fs::removePermissions(FileName, nc::fs::WRITE);
	ASSERT_TRUE(permissionChanged);
	ASSERT_FALSE(nc::fs::isWritable(FileName));
	ASSERT_FALSE(nc::fs::isWritableFile(FileName));

	printf("Add write permission to file\n");
	permissionChanged = nc::fs::addPermissions(FileName, nc::fs::WRITE);
	ASSERT_TRUE(permissionChanged);
	ASSERT_TRUE(nc::fs::isWritable(FileName));
	ASSERT_TRUE(nc::fs::isWritableFile(FileName));

	bool fileDeleted = nc::fs::deleteFile(FileName);
	ASSERT_TRUE(fileDeleted);

	nctl::String executableFile;
	executableFile.format("%s.exe", FileName);
	printf("Creating a new file with an executable extension: \"%s\"\n", executableFile.data());
	touchFile(executableFile.data());

	ASSERT_TRUE(nc::fs::exists(executableFile.data()));
	ASSERT_TRUE(nc::fs::isFile(executableFile.data()));
	ASSERT_TRUE(nc::fs::isExecutable(executableFile.data()));

	fileDeleted = nc::fs::deleteFile(executableFile.data());
	ASSERT_TRUE(fileDeleted);

	ASSERT_FALSE(nc::fs::exists(executableFile.data()));
	ASSERT_FALSE(nc::fs::isFile(executableFile.data()));
}

TEST_F(FileSystemTest, ChangePermissionsFile)
{
	printf("Creating a new file: \"%s\"\n", FileName);
	touchFile(FileName);

	ASSERT_TRUE(nc::fs::exists(FileName));
	ASSERT_TRUE(nc::fs::isFile(FileName));

	printf("Make the file read only\n");
	bool permissionChanged = nc::fs::changePermissions(FileName, nc::fs::READ);
	ASSERT_TRUE(permissionChanged);
	ASSERT_TRUE(nc::fs::isReadable(FileName));
	ASSERT_TRUE(nc::fs::isReadableFile(FileName));
	ASSERT_FALSE(nc::fs::isWritable(FileName));
	ASSERT_FALSE(nc::fs::isWritableFile(FileName));

	int permissions = nc::fs::permissions(FileName);
	ASSERT_TRUE(permissions & nc::fs::Permission::READ);
	ASSERT_FALSE(permissions & nc::fs::Permission::WRITE);

	printf("Make the file readable and writable\n");
	permissionChanged = nc::fs::changePermissions(FileName, nc::fs::READ | nc::fs::WRITE);
	ASSERT_TRUE(permissionChanged);
	ASSERT_TRUE(nc::fs::isReadable(FileName));
	ASSERT_TRUE(nc::fs::isReadableFile(FileName));
	ASSERT_TRUE(nc::fs::isWritable(FileName));
	ASSERT_TRUE(nc::fs::isWritableFile(FileName));

	permissions = nc::fs::permissions(FileName);
	ASSERT_TRUE(permissions & nc::fs::Permission::READ);
	ASSERT_TRUE(permissions & nc::fs::Permission::WRITE);

	const bool fileDeleted = nc::fs::deleteFile(FileName);
	ASSERT_TRUE(fileDeleted);

	ASSERT_FALSE(nc::fs::exists(FileName));
	ASSERT_FALSE(nc::fs::isFile(FileName));
}
#endif

TEST_F(FileSystemTest, ReadEmptyDirectory)
{
	printf("Creating a new directory: \"%s\"\n", DirectoryName);
	const bool dirCreated = nc::fs::createDir(DirectoryName);
	ASSERT_TRUE(dirCreated);

	int numEntries = 0;
	nc::fs::Directory dir(DirectoryName);
	while (const char *entryName = dir.readNext())
		numEntries++;
	ASSERT_EQ(numEntries, 2); // "." and ".."

	const bool dirDeleted = nc::fs::deleteEmptyDir(DirectoryName);
	ASSERT_TRUE(dirDeleted);
}

TEST_F(FileSystemTest, ReadDirectory)
{
	printf("Creating a new directory: \"%s\"\n", DirectoryName);
	const bool dirCreated = nc::fs::createDir(DirectoryName);
	ASSERT_TRUE(dirCreated);

	printf("Creating some files inside the new directory\n");

	nctl::String filePath = nc::fs::joinPath(DirectoryName, "a");
	touchFile(filePath.data());

	filePath = nc::fs::joinPath(DirectoryName, "b");
	touchFile(filePath.data());

	filePath = nc::fs::joinPath(DirectoryName, "c");
	touchFile(filePath.data());

	int numEntries = 0;
	nc::fs::Directory dir(DirectoryName);
	while (const char *entryName = dir.readNext())
	{
		printf("Entry %d: \"%s\"\n", numEntries, entryName);
		if (*entryName != '.')
		{
			filePath = nc::fs::joinPath(DirectoryName, entryName);
			nc::fs::deleteFile(filePath.data());
		}
		numEntries++;
	}
	ASSERT_EQ(numEntries, 5); // The 3 files + "." and ".."

	const bool dirDeleted = nc::fs::deleteEmptyDir(DirectoryName);
	ASSERT_TRUE(dirDeleted);
}

}
