#include "LuaMeshSprite.h"
#include "LuaUntrackedUserData.h"
#include "LuaClassTracker.h"
#include "LuaBaseSprite.h"
#include "LuaVector2Utils.h"
#include "MeshSprite.h"

namespace ncine {

namespace LuaNames {
namespace MeshSprite {
	static const char *MeshSprite = "mesh_sprite";

	static const char *bytesPerVertex = "get_bytes_per_vertex";
	static const char *numVertices = "get_num_vertices";
	static const char *numBytes = "get_num_bytes";
	static const char *vertices = "get_vertices";
	static const char *uniqueVertices = "are_unique_vertices";

	static const char *copyVertices = "copy_vertices";
	static const char *setVertices = "set_vertices";
	static const char *createVerticesFromTexels = "create_vertices_from_texels";
	static const char *emplaceVertices = "emplace_vertices";

	static const char *numIndices = "get_num_indices";
	static const char *indices = "get_indices";
	static const char *uniqueIndices = "are_unique_indices";

	static const char *copyIndices = "copy_indices";
	static const char *setIndices = "set_indices";
	static const char *emplaceIndices = "emplace_indices";

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
		LuaUtils::addFunction(L, LuaNames::cloneNode, cloneNode);
	}

	LuaUtils::addFunction(L, LuaNames::MeshSprite::bytesPerVertex, bytesPerVertex);
	LuaUtils::addFunction(L, LuaNames::MeshSprite::numVertices, numVertices);
	LuaUtils::addFunction(L, LuaNames::MeshSprite::numBytes, numBytes);
	LuaUtils::addFunction(L, LuaNames::MeshSprite::vertices, vertices);
	LuaUtils::addFunction(L, LuaNames::MeshSprite::uniqueVertices, uniqueVertices);

	LuaUtils::addFunction(L, LuaNames::MeshSprite::copyVertices, copyVertices);
	LuaUtils::addFunction(L, LuaNames::MeshSprite::setVertices, setVertices);
	LuaUtils::addFunction(L, LuaNames::MeshSprite::createVerticesFromTexels, createVerticesFromTexels);
	LuaUtils::addFunction(L, LuaNames::MeshSprite::emplaceVertices, emplaceVertices);

	LuaUtils::addFunction(L, LuaNames::MeshSprite::numIndices, numIndices);
	LuaUtils::addFunction(L, LuaNames::MeshSprite::indices, indices);
	LuaUtils::addFunction(L, LuaNames::MeshSprite::uniqueIndices, uniqueIndices);

	LuaUtils::addFunction(L, LuaNames::MeshSprite::copyIndices, copyIndices);
	LuaUtils::addFunction(L, LuaNames::MeshSprite::setIndices, setIndices);
	LuaUtils::addFunction(L, LuaNames::MeshSprite::emplaceIndices, emplaceIndices);

	LuaBaseSprite::exposeFunctions(L);

	lua_setfield(L, -2, LuaNames::MeshSprite::MeshSprite);
}

void LuaMeshSprite::exposeConstants(lua_State *L)
{
	lua_createtable(L, 0, 2);

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

	void retrieveVertexTableNoTexture(lua_State *L, int index, MeshSprite::VertexNoTexture &vertex)
	{
		vertex.x = LuaUtils::retrieveField<float>(L, index, LuaNames::MeshSprite::x);
		vertex.y = LuaUtils::retrieveField<float>(L, index, LuaNames::MeshSprite::y);
	}

	nctl::UniquePtr<MeshSprite::Vertex[]> retrieveVertices(lua_State *L, int index, unsigned int &length)
	{
		length = lua_rawlen(L, index);

		nctl::UniquePtr<MeshSprite::Vertex[]> vertices = nctl::makeUnique<MeshSprite::Vertex[]>(length);

		for (unsigned int i = 0; i < length; i++)
		{
			const int type = lua_rawgeti(L, index, i + 1);
			if (type != LUA_TTABLE)
				LOGW_X("Expecting a table at index %u of table at index %d", i + 1, index);
			else
				retrieveVertexTable(L, -1, vertices[i]);

			lua_pop(L, 1);
		}

		return nctl::move(vertices);
	}

	nctl::UniquePtr<MeshSprite::VertexNoTexture[]> retrieveVerticesNoTexture(lua_State *L, int index, unsigned int &length)
	{
		length = lua_rawlen(L, index);

		nctl::UniquePtr<MeshSprite::VertexNoTexture[]> vertices = nctl::makeUnique<MeshSprite::VertexNoTexture[]>(length);

		for (unsigned int i = 0; i < length; i++)
		{
			const int type = lua_rawgeti(L, index, i + 1);
			if (type != LUA_TTABLE)
				LOGW_X("Expecting a table at index %u of table at index %d", i + 1, index);
			else
				retrieveVertexTableNoTexture(L, -1, vertices[i]);

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
				LOGW_X("Expecting a table at index %u of table at index %d", i + 1, index);
			else
				vertices[i] = LuaVector2fUtils::retrieveTable(L, -1);

			lua_pop(L, 1);
		}

		return nctl::move(vertices);
	}

	nctl::UniquePtr<unsigned short[]> retrieveIndices(lua_State *L, int index, unsigned int &length)
	{
		length = lua_rawlen(L, index);

		nctl::UniquePtr<unsigned short[]> indices = nctl::makeUnique<unsigned short[]>(length);

		for (unsigned int i = 0; i < length; i++)
		{
			const int type = lua_rawgeti(L, index, i + 1);
			if (type != LUA_TNUMBER)
				LOGW_X("Expecting a number at index %u of table at index %d", i + 1, index);
			else
				indices[i] = static_cast<unsigned short>(LuaUtils::retrieve<uint32_t>(L, -1));

			lua_pop(L, 1);
		}

		return nctl::move(indices);
	}

}

int LuaMeshSprite::newObject(lua_State *L)
{
	SceneNode *parent = LuaUntrackedUserData<SceneNode>::retrieveOrNil(L, -4);
	Texture *texture = LuaUntrackedUserData<Texture>::retrieveOrNil(L, -3);
	const float x = LuaUtils::retrieve<float>(L, -2);
	const float y = LuaUtils::retrieve<float>(L, -1);

	LuaClassTracker<MeshSprite>::newObject(L, parent, texture, x, y);

	return 1;
}

int LuaMeshSprite::cloneNode(lua_State *L)
{
	const MeshSprite *sprite = LuaUntrackedUserData<MeshSprite>::retrieve(L, -1);

	if (sprite)
		LuaClassTracker<MeshSprite>::cloneNode(L, *sprite);
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaMeshSprite::bytesPerVertex(lua_State *L)
{
	MeshSprite *sprite = LuaUntrackedUserData<MeshSprite>::retrieve(L, -1);

	if (sprite)
		LuaUtils::push(L, sprite->bytesPerVertex());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaMeshSprite::numVertices(lua_State *L)
{
	MeshSprite *sprite = LuaUntrackedUserData<MeshSprite>::retrieve(L, -1);

	if (sprite)
		LuaUtils::push(L, sprite->numVertices());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaMeshSprite::numBytes(lua_State *L)
{
	MeshSprite *sprite = LuaUntrackedUserData<MeshSprite>::retrieve(L, -1);

	if (sprite)
		LuaUtils::push(L, sprite->numBytes());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaMeshSprite::vertices(lua_State *L)
{
	MeshSprite *sprite = LuaUntrackedUserData<MeshSprite>::retrieve(L, -1);

	if (sprite)
	{
		const unsigned int numElements = sprite->numVertices() * sprite->bytesPerVertex() / sizeof(float);
		const float *vertices = sprite->vertices();

		LuaUtils::createTable(L, 0, numElements);
		for (unsigned int i = 0; i < numElements; i++)
		{
			LuaUtils::push(L, vertices[i]);
			lua_rawseti(L, -2, i + 1); // Lua arrays start from index 1
		}
	}
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaMeshSprite::uniqueVertices(lua_State *L)
{
	MeshSprite *sprite = LuaUntrackedUserData<MeshSprite>::retrieve(L, -1);

	if (sprite)
		LuaUtils::push(L, sprite->uniqueVertices());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaMeshSprite::copyVertices(lua_State *L)
{
	if (lua_istable(L, -1) == false)
	{
		LOGW("Expecting a table at index -1");
		return 0;
	}

	MeshSprite *sprite = LuaUntrackedUserData<MeshSprite>::retrieve(L, -2);

	if (sprite)
	{
		unsigned int numVertices = 0;
		if (sprite->texture() != nullptr)
		{
			nctl::UniquePtr<MeshSprite::Vertex[]> vertices = retrieveVertices(L, -1, numVertices);
			sprite->copyVertices(numVertices, vertices.get());
		}
		else
		{
			nctl::UniquePtr<MeshSprite::VertexNoTexture[]> vertices = retrieveVerticesNoTexture(L, -1, numVertices);
			sprite->copyVertices(numVertices, vertices.get());
		}
	}
	else
		lua_pop(L, 1);

	return 0;
}

int LuaMeshSprite::setVertices(lua_State *L)
{
	MeshSprite *sprite = LuaUntrackedUserData<MeshSprite>::retrieve(L, -2);
	MeshSprite *sourceSprite = LuaUntrackedUserData<MeshSprite>::retrieve(L, -1);

	if (sprite && sourceSprite)
		sprite->setVertices(*sourceSprite);

	return 0;
}

int LuaMeshSprite::createVerticesFromTexels(lua_State *L)
{
	if (lua_istable(L, -2) == false)
	{
		LOGW("Expecting a table at index -2");
		return 0;
	}

	unsigned int numVertices = 0;
	MeshSprite *sprite = LuaUntrackedUserData<MeshSprite>::retrieve(L, -3);
	nctl::UniquePtr<Vector2f[]> texels = retrieveTexels(L, -2, numVertices);
	const MeshSprite::TextureCutMode cutMode = static_cast<MeshSprite::TextureCutMode>(LuaUtils::retrieve<int64_t>(L, -1));

	if (sprite)
		sprite->createVerticesFromTexels(numVertices, texels.get(), cutMode);

	return 0;
}

int LuaMeshSprite::emplaceVertices(lua_State *L)
{
	if (lua_istable(L, -1) == false)
	{
		LOGW("Expecting a table at index -1");
		return 0;
	}

	MeshSprite *sprite = LuaUntrackedUserData<MeshSprite>::retrieve(L, -2);
	const unsigned int numElements = lua_rawlen(L, -1);

	if (sprite && numElements > 0)
	{
		float *vertices = sprite->emplaceVertices(numElements);

		for (unsigned int i = 0; i < numElements; i++)
		{
			const int type = lua_rawgeti(L, -1, i + 1);
			if (type != LUA_TNUMBER)
				LOGW_X("Expecting a number at index %u of table at index %d", i + 1, -1);
			else
				vertices[i] = LuaUtils::retrieve<float>(L, -1);

			lua_pop(L, 1);
		}
	}

	return 0;
}

int LuaMeshSprite::numIndices(lua_State *L)
{
	MeshSprite *sprite = LuaUntrackedUserData<MeshSprite>::retrieve(L, -1);

	if (sprite)
		LuaUtils::push(L, sprite->numIndices());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaMeshSprite::indices(lua_State *L)
{
	MeshSprite *sprite = LuaUntrackedUserData<MeshSprite>::retrieve(L, -1);

	if (sprite)
	{
		const unsigned int numIndices = sprite->numIndices();
		const unsigned short *indices = sprite->indices();

		LuaUtils::createTable(L, 0, numIndices);
		for (unsigned int i = 0; i < numIndices; i++)
		{
			LuaUtils::push(L, indices[i]);
			lua_rawseti(L, -2, i + 1); // Lua arrays start from index 1
		}
	}
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaMeshSprite::uniqueIndices(lua_State *L)
{
	MeshSprite *sprite = LuaUntrackedUserData<MeshSprite>::retrieve(L, -1);

	if (sprite)
		LuaUtils::push(L, sprite->uniqueIndices());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaMeshSprite::copyIndices(lua_State *L)
{
	if (lua_istable(L, -1) == false)
	{
		LOGW("Expecting a table at index -1");
		return 0;
	}

	MeshSprite *sprite = LuaUntrackedUserData<MeshSprite>::retrieve(L, -2);

	if (sprite)
	{
		unsigned int numIndices = 0;
		nctl::UniquePtr<unsigned short[]> indices = retrieveIndices(L, -1, numIndices);
		sprite->copyIndices(numIndices, indices.get());
	}
	else
		lua_pop(L, 1);

	return 0;
}

int LuaMeshSprite::setIndices(lua_State *L)
{
	MeshSprite *sprite = LuaUntrackedUserData<MeshSprite>::retrieve(L, -2);
	MeshSprite *sourceSprite = LuaUntrackedUserData<MeshSprite>::retrieve(L, -1);

	if (sprite && sourceSprite)
		sprite->setIndices(*sourceSprite);

	return 0;
}

int LuaMeshSprite::emplaceIndices(lua_State *L)
{
	if (lua_istable(L, -1) == false)
	{
		LOGW("Expecting a table at index -1");
		return 0;
	}

	MeshSprite *sprite = LuaUntrackedUserData<MeshSprite>::retrieve(L, -2);
	const unsigned int numIndices = lua_rawlen(L, -1);

	if (sprite && numIndices > 0)
	{
		unsigned short *indices = sprite->emplaceIndices(numIndices);

		for (unsigned int i = 0; i < numIndices; i++)
		{
			const int type = lua_rawgeti(L, -1, i + 1);
			if (type != LUA_TNUMBER)
				LOGW_X("Expecting a number at index %u of table at index %d", i + 1, -1);
			else
				indices[i] = static_cast<unsigned short>(LuaUtils::retrieve<uint32_t>(L, -1));

			lua_pop(L, 1);
		}
	}

	return 0;
}

}
