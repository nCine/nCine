#ifndef CLASS_NCINE_LUAFILESYSTEM
#define CLASS_NCINE_LUAFILESYSTEM

struct lua_State;

namespace ncine {

/// Lua bindings around the `FileSystem` class
class LuaFileSystem
{
  public:
	static void exposeConstants(lua_State *L);
	static void expose(lua_State *L);

  private:
	static int joinPath(lua_State *L);
	static int absoluteJoinPath(lua_State *L);

	static int dirName(lua_State *L);
	static int baseName(lua_State *L);
	static int absolutePath(lua_State *L);

	static int extension(lua_State *L);
	static int hasExtension(lua_State *L);

	static int logicalDrives(lua_State *L);
	static int logicalDriveStrings(lua_State *L);

	static int currentDir(lua_State *L);
	static int setCurrentDir(lua_State *L);
	static int homeDir(lua_State *L);

	static int isDirectory(lua_State *L);
	static int isFile(lua_State *L);

	static int exists(lua_State *L);
	static int isReadable(lua_State *L);
	static int isWritable(lua_State *L);
	static int isExecutable(lua_State *L);

	static int isReadableFile(lua_State *L);
	static int isWritableFile(lua_State *L);

	static int isHidden(lua_State *L);
	static int setHidden(lua_State *L);

	static int createDir(lua_State *L);
	static int deleteEmptyDir(lua_State *L);
	static int deleteFile(lua_State *L);
	static int rename(lua_State *L);
	static int copy(lua_State *L);

	static int fileSize(lua_State *L);
	static int lastModificationTime(lua_State *L);
	static int lastAccessTime(lua_State *L);

	static int permissions(lua_State *L);
	static int changePermissions(lua_State *L);
	static int addPermissions(lua_State *L);
	static int removePermissions(lua_State *L);

	static int datapath(lua_State *L);
	static int savepath(lua_State *L);
};

}

#endif
