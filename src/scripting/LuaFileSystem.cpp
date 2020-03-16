#define NCINE_INCLUDE_LUA
#include "common_headers.h"

#include "LuaFileSystem.h"
#include "LuaUtils.h"
#include "FileSystem.h"

namespace ncine {

namespace LuaNames {
namespace FileSystem {
	static const char *FileSystem = "fs";

	static const char *READ = "READ";
	static const char *WRITE = "WRITE";
	static const char *EXECUTE = "EXECUTE";
	static const char *Permission = "permission";

	static const char *DateYear = "year";
	static const char *DateMonth = "month";
	static const char *DateDay = "day";
	static const char *DateWeekDay = "week_day";
	static const char *DateHour = "hour";
	static const char *DateMinute = "minute";
	static const char *DateSecond = "second";

	static const char *joinPath = "join_path";
	static const char *absoluteJoinPath = "absolute_join_path";

	static const char *dirName = "dirname";
	static const char *baseName = "basename";
	static const char *absolutePath = "absolute_path";

	static const char *extension = "get_extension";
	static const char *hasExtension = "has_extension";
	static const char *logicalDrives = "logical_drives";
	static const char *logicalDriveStrings = "logical_drive_strings";

	static const char *currentDir = "get_current_dir";
	static const char *setCurrentDir = "set_current_dir";
	static const char *homeDir = "get_home_dir";

	static const char *isDirectory = "is_directory";
	static const char *isFile = "is_file";

	static const char *exists = "exists";
	static const char *isReadable = "is_readable";
	static const char *isWritable = "is_writable";
	static const char *isExecutable = "is_executable";

	static const char *isReadableFile = "is_readable_file";
	static const char *isWritableFile = "is_writable_file";

	static const char *isHidden = "is_hidden";
	static const char *setHidden = "set_hidden";

	static const char *createDir = "create_dir";
	static const char *deleteEmptyDir = "delete_empty_dir";
	static const char *deleteFile = "delete_file";
	static const char *rename = "rename";
	static const char *copy = "copy";

	static const char *fileSize = "file_size";
	static const char *lastModificationTime = "last_modification_time";
	static const char *lastAccessTime = "last_access_time";

	static const char *permissions = "permissions";
	static const char *changePermissions = "change_permissions";
	static const char *addPermissions = "add_permissions";
	static const char *removePermissions = "remove_permissions";

	static const char *datapath = "get_datapath";
	static const char *savepath = "get_savepath";
}}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void LuaFileSystem::exposeConstants(lua_State *L)
{
	lua_createtable(L, 3, 0);

	LuaUtils::pushField(L, LuaNames::FileSystem::READ, static_cast<int64_t>(FileSystem::Permission::READ));
	LuaUtils::pushField(L, LuaNames::FileSystem::WRITE, static_cast<int64_t>(FileSystem::Permission::WRITE));
	LuaUtils::pushField(L, LuaNames::FileSystem::EXECUTE, static_cast<int64_t>(FileSystem::Permission::EXECUTE));

	lua_setfield(L, -2, LuaNames::FileSystem::Permission);
}

void LuaFileSystem::expose(lua_State *L)
{
	lua_newtable(L);

	LuaUtils::addFunction(L, LuaNames::FileSystem::joinPath, joinPath);
	LuaUtils::addFunction(L, LuaNames::FileSystem::absoluteJoinPath, absoluteJoinPath);

	LuaUtils::addFunction(L, LuaNames::FileSystem::dirName, dirName);
	LuaUtils::addFunction(L, LuaNames::FileSystem::baseName, baseName);
	LuaUtils::addFunction(L, LuaNames::FileSystem::absolutePath, absolutePath);

	LuaUtils::addFunction(L, LuaNames::FileSystem::extension, extension);
	LuaUtils::addFunction(L, LuaNames::FileSystem::hasExtension, hasExtension);

	LuaUtils::addFunction(L, LuaNames::FileSystem::logicalDrives, logicalDrives);
	LuaUtils::addFunction(L, LuaNames::FileSystem::logicalDriveStrings, logicalDriveStrings);

	LuaUtils::addFunction(L, LuaNames::FileSystem::currentDir, currentDir);
	LuaUtils::addFunction(L, LuaNames::FileSystem::setCurrentDir, setCurrentDir);
	LuaUtils::addFunction(L, LuaNames::FileSystem::homeDir, homeDir);

	LuaUtils::addFunction(L, LuaNames::FileSystem::isDirectory, isDirectory);
	LuaUtils::addFunction(L, LuaNames::FileSystem::isFile, isFile);

	LuaUtils::addFunction(L, LuaNames::FileSystem::exists, exists);
	LuaUtils::addFunction(L, LuaNames::FileSystem::isReadable, isReadable);
	LuaUtils::addFunction(L, LuaNames::FileSystem::isWritable, isWritable);
	LuaUtils::addFunction(L, LuaNames::FileSystem::isExecutable, isExecutable);

	LuaUtils::addFunction(L, LuaNames::FileSystem::isReadableFile, isReadableFile);
	LuaUtils::addFunction(L, LuaNames::FileSystem::isWritableFile, isWritableFile);

	LuaUtils::addFunction(L, LuaNames::FileSystem::isHidden, isHidden);
	LuaUtils::addFunction(L, LuaNames::FileSystem::setHidden, setHidden);

	LuaUtils::addFunction(L, LuaNames::FileSystem::createDir, createDir);
	LuaUtils::addFunction(L, LuaNames::FileSystem::deleteEmptyDir, deleteEmptyDir);
	LuaUtils::addFunction(L, LuaNames::FileSystem::deleteFile, deleteFile);
	LuaUtils::addFunction(L, LuaNames::FileSystem::rename, rename);
	LuaUtils::addFunction(L, LuaNames::FileSystem::copy, copy);

	LuaUtils::addFunction(L, LuaNames::FileSystem::fileSize, fileSize);
	LuaUtils::addFunction(L, LuaNames::FileSystem::lastModificationTime, lastModificationTime);
	LuaUtils::addFunction(L, LuaNames::FileSystem::lastAccessTime, lastAccessTime);

	LuaUtils::addFunction(L, LuaNames::FileSystem::permissions, permissions);
	LuaUtils::addFunction(L, LuaNames::FileSystem::changePermissions, changePermissions);
	LuaUtils::addFunction(L, LuaNames::FileSystem::addPermissions, addPermissions);
	LuaUtils::addFunction(L, LuaNames::FileSystem::removePermissions, removePermissions);

	LuaUtils::addFunction(L, LuaNames::FileSystem::datapath, datapath);
	LuaUtils::addFunction(L, LuaNames::FileSystem::savepath, savepath);

	lua_setfield(L, -2, LuaNames::FileSystem::FileSystem);
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

int LuaFileSystem::joinPath(lua_State *L)
{
	const char *first = LuaUtils::retrieve<const char *>(L, -2);
	const char *second = LuaUtils::retrieve<const char *>(L, -1);

	const nctl::String joinedPath = fs::joinPath(first, second);
	LuaUtils::push(L, joinedPath.data());

	return 1;
}

int LuaFileSystem::absoluteJoinPath(lua_State *L)
{
	const char *first = LuaUtils::retrieve<const char *>(L, -2);
	const char *second = LuaUtils::retrieve<const char *>(L, -1);

	const nctl::String absJoinedPath = fs::absoluteJoinPath(first, second);
	LuaUtils::push(L, absJoinedPath.data());

	return 1;
}

int LuaFileSystem::dirName(lua_State *L)
{
	const char *path = LuaUtils::retrieve<const char *>(L, -1);

	const nctl::String dirName = fs::dirName(path);
	LuaUtils::push(L, dirName.data());

	return 1;
}

int LuaFileSystem::baseName(lua_State *L)
{
	const char *path = LuaUtils::retrieve<const char *>(L, -1);

	const nctl::String baseName = fs::baseName(path);
	LuaUtils::push(L, baseName.data());

	return 1;
}

int LuaFileSystem::absolutePath(lua_State *L)
{
	const char *path = LuaUtils::retrieve<const char *>(L, -1);

	const nctl::String absPath = fs::absolutePath(path);
	LuaUtils::push(L, absPath.data());

	return 1;
}

int LuaFileSystem::extension(lua_State *L)
{
	const char *path = LuaUtils::retrieve<const char *>(L, -1);

	const char *extension = fs::extension(path);
	LuaUtils::push(L, extension);

	return 1;
}

int LuaFileSystem::hasExtension(lua_State *L)
{
	const char *path = LuaUtils::retrieve<const char *>(L, -2);
	const char *extension = LuaUtils::retrieve<const char *>(L, -1);

	const bool hasExtension = fs::hasExtension(path, extension);
	LuaUtils::push(L, hasExtension);

	return 1;
}

int LuaFileSystem::logicalDrives(lua_State *L)
{
	const unsigned long logicalDrives = fs::logicalDrives();
	LuaUtils::push(L, static_cast<uint64_t>(logicalDrives));

	return 1;
}

int LuaFileSystem::logicalDriveStrings(lua_State *L)
{
	const char *logicalDriveString = fs::logicalDriveStrings();
	LuaUtils::push(L, logicalDriveString);

	return 1;
}

int LuaFileSystem::currentDir(lua_State *L)
{
	const nctl::String currentDir = fs::currentDir();
	LuaUtils::push(L, currentDir.data());

	return 1;
}

int LuaFileSystem::setCurrentDir(lua_State *L)
{
	const char *path = LuaUtils::retrieve<const char *>(L, -1);

	const bool dirChanged = fs::setCurrentDir(path);
	LuaUtils::push(L, dirChanged);

	return 1;
}

int LuaFileSystem::homeDir(lua_State *L)
{
	const nctl::String homeDir = fs::homeDir();
	LuaUtils::push(L, homeDir.data());

	return 1;
}

int LuaFileSystem::isDirectory(lua_State *L)
{
	const char *path = LuaUtils::retrieve<const char *>(L, -1);

	const bool isDirectory = fs::isDirectory(path);
	LuaUtils::push(L, isDirectory);

	return 1;
}

int LuaFileSystem::isFile(lua_State *L)
{
	const char *path = LuaUtils::retrieve<const char *>(L, -1);

	const bool isFile = fs::isFile(path);
	LuaUtils::push(L, isFile);

	return 1;
}

int LuaFileSystem::exists(lua_State *L)
{
	const char *path = LuaUtils::retrieve<const char *>(L, -1);

	const bool exists = fs::exists(path);
	LuaUtils::push(L, exists);

	return 1;
}

int LuaFileSystem::isReadable(lua_State *L)
{
	const char *path = LuaUtils::retrieve<const char *>(L, -1);

	const bool isReadable = fs::isReadable(path);
	LuaUtils::push(L, isReadable);

	return 1;
}

int LuaFileSystem::isWritable(lua_State *L)
{
	const char *path = LuaUtils::retrieve<const char *>(L, -1);

	const bool isWritable = fs::isWritable(path);
	LuaUtils::push(L, isWritable);

	return 1;
}

int LuaFileSystem::isExecutable(lua_State *L)
{
	const char *path = LuaUtils::retrieve<const char *>(L, -1);

	const bool isExecutable = fs::isExecutable(path);
	LuaUtils::push(L, isExecutable);

	return 1;
}

int LuaFileSystem::isReadableFile(lua_State *L)
{
	const char *path = LuaUtils::retrieve<const char *>(L, -1);

	const bool isReadableFile = fs::isReadableFile(path);
	LuaUtils::push(L, isReadableFile);

	return 1;
}

int LuaFileSystem::isWritableFile(lua_State *L)
{
	const char *path = LuaUtils::retrieve<const char *>(L, -1);

	const bool isWritableFile = fs::isWritableFile(path);
	LuaUtils::push(L, isWritableFile);

	return 1;
}

int LuaFileSystem::isHidden(lua_State *L)
{
	const char *path = LuaUtils::retrieve<const char *>(L, -1);

	const bool isHidden = fs::isFile(path);
	LuaUtils::push(L, isHidden);

	return 1;
}

int LuaFileSystem::setHidden(lua_State *L)
{
	const char *path = LuaUtils::retrieve<const char *>(L, -2);
	const bool hidden = LuaUtils::retrieve<bool>(L, -1);

	const bool fileChanged = fs::setHidden(path, hidden);
	LuaUtils::push(L, fileChanged);

	return 1;
}

int LuaFileSystem::createDir(lua_State *L)
{
	const char *path = LuaUtils::retrieve<const char *>(L, -1);

	const bool dirCreated = fs::createDir(path);
	LuaUtils::push(L, dirCreated);

	return 1;
}

int LuaFileSystem::deleteEmptyDir(lua_State *L)
{
	const char *path = LuaUtils::retrieve<const char *>(L, -1);

	const bool dirDeleted = fs::deleteEmptyDir(path);
	LuaUtils::push(L, dirDeleted);

	return 1;
}

int LuaFileSystem::deleteFile(lua_State *L)
{
	const char *path = LuaUtils::retrieve<const char *>(L, -1);

	const bool fileDeleted = fs::deleteFile(path);
	LuaUtils::push(L, fileDeleted);

	return 1;
}

int LuaFileSystem::rename(lua_State *L)
{
	const char *oldPath = LuaUtils::retrieve<const char *>(L, -2);
	const char *newPath = LuaUtils::retrieve<const char *>(L, -1);

	const bool renamed = fs::rename(oldPath, newPath);
	LuaUtils::push(L, renamed);

	return 1;
}

int LuaFileSystem::copy(lua_State *L)
{
	const char *oldPath = LuaUtils::retrieve<const char *>(L, -2);
	const char *newPath = LuaUtils::retrieve<const char *>(L, -1);

	const bool copied = fs::copy(oldPath, newPath);
	LuaUtils::push(L, copied);

	return 1;
}

int LuaFileSystem::fileSize(lua_State *L)
{
	const char *path = LuaUtils::retrieve<const char *>(L, -1);

	const long int fileSize = fs::fileSize(path);
	LuaUtils::push(L, static_cast<int64_t>(fileSize));

	return 1;
}

int LuaFileSystem::lastModificationTime(lua_State *L)
{
	const char *path = LuaUtils::retrieve<const char *>(L, -1);

	const fs::FileDate date = fs::lastModificationTime(path);
	LuaUtils::createTable(L, 7, 0);
	LuaUtils::pushField(L, LuaNames::FileSystem::DateYear, date.year);
	LuaUtils::pushField(L, LuaNames::FileSystem::DateMonth, date.month);
	LuaUtils::pushField(L, LuaNames::FileSystem::DateDay, date.day);
	LuaUtils::pushField(L, LuaNames::FileSystem::DateWeekDay, date.weekDay);
	LuaUtils::pushField(L, LuaNames::FileSystem::DateHour, date.hour);
	LuaUtils::pushField(L, LuaNames::FileSystem::DateMinute, date.minute);
	LuaUtils::pushField(L, LuaNames::FileSystem::DateSecond, date.second);

	return 1;
}

int LuaFileSystem::lastAccessTime(lua_State *L)
{
	const char *path = LuaUtils::retrieve<const char *>(L, -1);

	const fs::FileDate date = fs::lastAccessTime(path);
	LuaUtils::createTable(L, 7, 0);
	LuaUtils::pushField(L, LuaNames::FileSystem::DateYear, date.year);
	LuaUtils::pushField(L, LuaNames::FileSystem::DateMonth, date.month);
	LuaUtils::pushField(L, LuaNames::FileSystem::DateDay, date.day);
	LuaUtils::pushField(L, LuaNames::FileSystem::DateWeekDay, date.weekDay);
	LuaUtils::pushField(L, LuaNames::FileSystem::DateHour, date.hour);
	LuaUtils::pushField(L, LuaNames::FileSystem::DateMinute, date.minute);
	LuaUtils::pushField(L, LuaNames::FileSystem::DateSecond, date.second);

	return 1;
}

int LuaFileSystem::permissions(lua_State *L)
{
	const char *path = LuaUtils::retrieve<const char *>(L, -1);

	const int permissions = fs::permissions(path);
	LuaUtils::push(L, permissions);

	return 1;
}

int LuaFileSystem::changePermissions(lua_State *L)
{
	const char *path = LuaUtils::retrieve<const char *>(L, -2);
	const int mode = LuaUtils::retrieve<int>(L, -1);

	const bool changed = fs::changePermissions(path, mode);
	LuaUtils::push(L, changed);

	return 1;
}

int LuaFileSystem::addPermissions(lua_State *L)
{
	const char *path = LuaUtils::retrieve<const char *>(L, -2);
	const int mode = LuaUtils::retrieve<int>(L, -1);

	const bool added = fs::addPermissions(path, mode);
	LuaUtils::push(L, added);

	return 1;
}

int LuaFileSystem::removePermissions(lua_State *L)
{
	const char *path = LuaUtils::retrieve<const char *>(L, -2);
	const int mode = LuaUtils::retrieve<int>(L, -1);

	const bool removed = fs::removePermissions(path, mode);
	LuaUtils::push(L, removed);

	return 1;
}

int LuaFileSystem::datapath(lua_State *L)
{
	const nctl::String dataPath = fs::dataPath();
	LuaUtils::push(L, dataPath.data());

	return 1;
}

int LuaFileSystem::savepath(lua_State *L)
{
	const nctl::String savePath = fs::savePath();
	LuaUtils::push(L, savePath.data());

	return 1;
}

}
