#include "LuaHash64.h"
#include "LuaUtils.h"
#include "Hash64.h"

namespace ncine {

namespace LuaNames {
namespace Hash64 {
	static const char *Hash64 = "hash64";

	static const char *hashString = "hash_string";
	static const char *hashFileStat = "hash_file_stat";
	static const char *scanHashString = "scan_hash_string";
}}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void LuaHash64::expose(lua_State *L)
{
	LuaUtils::createTable(L, 0, 3);

	LuaUtils::addFunction(L, LuaNames::Hash64::hashString, hashString);
	LuaUtils::addFunction(L, LuaNames::Hash64::hashFileStat, hashFileStat);
	LuaUtils::addFunction(L, LuaNames::Hash64::scanHashString, scanHashString);

	LuaUtils::setField(L, -2, LuaNames::Hash64::Hash64);
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

int LuaHash64::hashString(lua_State *L)
{
	size_t length = 0;
	const char *string = LuaUtils::retrieve(L, -1, &length);

	const uint64_t hash = hash64().hashString(string, length);
	LuaUtils::push(L, hash);

	return 1;
}

int LuaHash64::hashFileStat(lua_State *L)
{
	const char *filePath = LuaUtils::retrieve<const char *>(L, -1);

	const uint64_t hash = hash64().hashFileStat(filePath);
	LuaUtils::push(L, hash);

	return 1;
}

int LuaHash64::scanHashString(lua_State *L)
{
	size_t length = 0;
	const char *string = LuaUtils::retrieve(L, -1, &length);

	const uint64_t hash = hash64().scanHashString(string, length);
	LuaUtils::push(L, hash);

	return 1;
}

}
