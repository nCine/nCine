#include "LuaMeshSprite.h"
#include "LuaClassWrapper.h"
#include "LuaClassTracker.h"
#include "LuaBaseSprite.h"
#include "LuaVector2Utils.h"
#include "MeshSprite.h"

namespace ncine {

namespace LuaNames {
namespace MeshSprite {
	static const char *MeshSprite = "mesh_sprite";

	static const char *numVertices = "num_vertices";
	static const char *copyVertices = "copy_vertices";
	static const char *setVertices = "set_vertices";
	static const char *createVerticesFromTexels = "create_vertices_from_texels";

	static const char *numIndices = "num_indices";
	static const char *copyIndices = "copy_indices";
	static const char *setIndices = "set_indices";

	static const char *RESIZE = "RESIZE";
	static const char *CROP = "CROP";
	static const char *TextureCutMode = "texture_cut_mode";

	static const char *x = "x";
	static const char *y = "y";
	static const char *u = "u";
	static const char *v = "v";
}}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void LuaMeshSprite::expose(LuaStateManager *stateManager)
{
	lua_State *L = stateManager->state();
	lua_newtable(L);

	if (stateManager->apiType() == LuaStateManager::ApiType::FULL)
	{
		LuaClassTracker<MeshSprite>::exposeDelete(L);
		LuaUtils::addFunction(L, LuaNames::newObject, newObject);
	}

	LuaUtils::addFunction(L, LuaNames::MeshSprite::numVertices, numVertices);
	LuaUtils::addFunction(L, LuaNames::MeshSprite::copyVertices, copyVertices);
	LuaUtils::addFunction(L, LuaNames::MeshSprite::setVertices, setVertices);
	LuaUtils::addFunction(L, LuaNames::MeshSprite::createVerticesFromTexels, createVerticesFromTexels);

	LuaUtils::addFunction(L, LuaNames::MeshSprite::numIndices, numIndices);
	LuaUtils::addFunction(L, LuaNames::MeshSprite::copyIndices, copyIndices);
	LuaUtils::addFunction(L, LuaNames::MeshSprite::setIndices, setIndices);

	LuaBaseSprite::exposeFunctions(L);

	lua_setfield(L, -2, LuaNames::MeshSprite::MeshSprite);
}

void LuaMeshSprite::exposeConstants(lua_State *L)
{
	lua_createtable(L, 2, 0);

	LuaUtils::pushField(L, LuaNames::MeshSprite::RESIZE, static_cast<int64_t>(MeshSprite::TextureCutMode::RESIZE));
	LuaUtils::pushField(L, LuaNames::MeshSprite::CROP, static_cast<int64_t>(MeshSprite::TextureCutMode::CROP));

	lua_setfield(L, -2, LuaNames::MeshSprite::TextureCutMode);
}

void LuaMeshSprite::release(void *object)
{
	MeshSprite *sprite = reinterpret_cast<MeshSprite *>(object);
	delete sprite;
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

namespace {

	void retrieveVertexTable(lua_State *L, int index, MeshSprite::Vertex &vertex)
	{
		vertex.x = LuaUtils::retrieveField<float>(L, index, LuaNames::MeshSprite::x);
		vertex.y = LuaUtils::retrieveField<float>(L, index, LuaNames::MeshSprite::y);
		vertex.u = LuaUtils::retrieveField<float>(L, index, LuaNames::MeshSprite::u);
		vertex.v = LuaUtils::retrieveField<float>(L, index, LuaNames::MeshSprite::v);
	}

	nctl::UniquePtr<MeshSprite::Vertex[]> retrieveVertices(lua_State *L, int index, unsigned int &length)
	{
		length = lua_rawlen(L, index);

		nctl::UniquePtr<MeshSprite::Vertex[]> vertices = nctl::makeUnique<MeshSprite::Vertex[]>(length);

		for (unsigned int i = 0; i < length; i++)
		{
			const int type = lua_rawgeti(L, index, i + 1);
			if (type != LUA_TTABLE)
				luaL_argerror(L, -1, "Expecting a table");

			retrieveVertexTable(L, -1, vertices[i]);
			lua_pop(L, 1);
		}

		return nctl::move(vertices);
	}

	nctl::UniquePtr<Vector2f[]> retrieveTexels(lua_State *L, int index, unsigned int &length)
	{
		length = lua_rawlen(L, index);

		nctl::UniquePtr<Vector2f[]> vertices = nctl::makeUnique<Vector2f[]>(length);

		for (unsigned int i = 0; i < length; i++)
		{
			const int type = lua_rawgeti(L, index, i + 1);
			if (type != LUA_TTABLE)
				luaL_argerror(L, -1, "Expecting a table");

			vertices[i] = LuaVector2fUtils::retrieveTable(L, -1);
			lua_pop(L, 1);
		}

		return nctl::move(vertices);
	}

	nctl::UniquePtr<unsigned short[]> retrieveIndices(lua_State *L, unsigned int &length)
	{
		length = lua_rawlen(L, -1);

		nctl::UniquePtr<unsigned short[]> indices = nctl::makeUnique<unsigned short[]>(length);

		for (unsigned int i = 0; i < length; i++)
		{
			lua_rawgeti(L, -1, i + 1);
			indices[i] = static_cast<unsigned short>(LuaUtils::retrieve<uint32_t>(L, -1));
			lua_pop(L, 1);
		}

		return nctl::move(indices);
	}

}

int LuaMeshSprite::newObject(lua_State *L)
{
	SceneNode *parent = LuaClassWrapper<SceneNode>::unwrapUserDataOrNil(L, -4);
	Texture *texture = LuaClassWrapper<Texture>::unwrapUserData(L, -3);
	const float x = LuaUtils::retrieve<float>(L, -2);
	const float y = LuaUtils::retrieve<float>(L, -1);

	LuaClassTracker<MeshSprite>::newObject(L, parent, texture, x, y);

	return 1;
}

int LuaMeshSprite::numVertices(lua_State *L)
{
	MeshSprite *sprite = LuaClassWrapper<MeshSprite>::unwrapUserData(L, -1);

	const unsigned int numVertices = sprite->numVertices();
	LuaUtils::push(L, numVertices);

	return 1;
}

int LuaMeshSprite::copyVertices(lua_State *L)
{
	if (lua_istable(L, -1) == false)
		luaL_argerror(L, -1, "Expecting a table");

	unsigned int numVertices = 0;
	MeshSprite *sprite = LuaClassWrapper<MeshSprite>::unwrapUserData(L, -2);
	nctl::UniquePtr<MeshSprite::Vertex[]> vertices = retrieveVertices(L, -1, numVertices);

	sprite->copyVertices(numVertices, vertices.get());

	return 0;
}

int LuaMeshSprite::setVertices(lua_State *L)
{
	MeshSprite *sprite = LuaClassWrapper<MeshSprite>::unwrapUserData(L, -2);
	MeshSprite *sourceSprite = LuaClassWrapper<MeshSprite>::unwrapUserData(L, -1);

	sprite->setVertices(*sourceSprite);

	return 0;
}

int LuaMeshSprite::createVerticesFromTexels(lua_State *L)
{
	if (lua_istable(L, -2) == false)
		luaL_argerror(L, -2, "Expecting a table");

	unsigned int numVertices = 0;
	MeshSprite *sprite = LuaClassWrapper<MeshSprite>::unwrapUserData(L, -3);
	nctl::UniquePtr<Vector2f[]> texels = retrieveTexels(L, -2, numVertices);
	const MeshSprite::TextureCutMode cutMode = static_cast<MeshSprite::TextureCutMode>(LuaUtils::retrieve<int64_t>(L, -1));

	sprite->createVerticesFromTexels(numVertices, texels.get(), cutMode);

	return 0;
}

int LuaMeshSprite::numIndices(lua_State *L)
{
	MeshSprite *sprite = LuaClassWrapper<MeshSprite>::unwrapUserData(L, -1);

	const unsigned int numIndices = sprite->numIndices();
	LuaUtils::push(L, numIndices);

	return 1;
}

int LuaMeshSprite::copyIndices(lua_State *L)
{
	if (lua_istable(L, -1) == false)
		luaL_argerror(L, -1, "Expecting a table");

	unsigned int numIndices = 0;
	MeshSprite *sprite = LuaClassWrapper<MeshSprite>::unwrapUserData(L, -2);
	nctl::UniquePtr<unsigned short[]> indices = retrieveIndices(L, numIndices);

	sprite->copyIndices(numIndices, indices.get());

	return 0;
}

int LuaMeshSprite::setIndices(lua_State *L)
{
	MeshSprite *sprite = LuaClassWrapper<MeshSprite>::unwrapUserData(L, -2);
	MeshSprite *sourceSprite = LuaClassWrapper<MeshSprite>::unwrapUserData(L, -1);

	sprite->setIndices(*sourceSprite);

	return 0;
}

}
