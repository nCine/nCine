#include "LuaTexture.h"
#include "LuaClassWrapper.h"
#include "LuaClassTracker.h"
#include "LuaUtils.h"
#include "Texture.h"

namespace ncine {

namespace LuaNames {
namespace Texture {
	static const char *Texture = "texture";

	static const char *width = "get_width";
	static const char *height = "get_height";
	static const char *mipMapLevels = "mip_levels";
	static const char *isCompressed = "is_compressed";
	static const char *numChannels = "num_channels";
	static const char *dataSize = "datasize";

	static const char *minFiltering = "get_min_filtering";
	static const char *magFiltering = "get_mag_filtering";
	static const char *wrap = "get_wrap";
	static const char *setMinFiltering = "set_min_filtering";
	static const char *setMagFiltering = "set_mag_filtering";
	static const char *setWrap = "set_wrap";

	static const char *NEAREST = "NEAREST";
	static const char *LINEAR = "LINEAR";
	static const char *NEAREST_MIPMAP_NEAREST = "NEAREST_MIPMAP_NEAREST";
	static const char *LINEAR_MIPMAP_NEAREST = "LINEAR_MIPMAP_NEAREST";
	static const char *NEAREST_MIPMAP_LINEAR = "NEAREST_MIPMAP_LINEAR";
	static const char *LINEAR_MIPMAP_LINEAR = "LINEAR_MIPMAP_LINEAR";
	static const char *Filtering = "tex_filtering";

	static const char *CLAMP_TO_EDGE = "CLAMP_TO_EDGE";
	static const char *MIRRORED_REPEAT = "MIRRORED_REPEAT";
	static const char *REPEAT = "REPEAT";
	static const char *Wrap = "tex_wrap";
}}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void LuaTexture::expose(LuaStateManager *stateManager)
{
	lua_State *L = stateManager->state();
	lua_newtable(L);

	if (stateManager->apiType() == LuaStateManager::ApiType::FULL)
	{
		LuaClassTracker<Texture>::exposeDelete(L);
		LuaUtils::addFunction(L, LuaNames::newObject, newObject);
	}

	LuaUtils::addFunction(L, LuaNames::Texture::width, width);
	LuaUtils::addFunction(L, LuaNames::Texture::height, height);
	LuaUtils::addFunction(L, LuaNames::Texture::mipMapLevels, mipMapLevels);
	LuaUtils::addFunction(L, LuaNames::Texture::isCompressed, isCompressed);
	LuaUtils::addFunction(L, LuaNames::Texture::numChannels, numChannels);
	LuaUtils::addFunction(L, LuaNames::Texture::dataSize, dataSize);

	LuaUtils::addFunction(L, LuaNames::Texture::minFiltering, minFiltering);
	LuaUtils::addFunction(L, LuaNames::Texture::magFiltering, magFiltering);
	LuaUtils::addFunction(L, LuaNames::Texture::wrap, wrap);
	LuaUtils::addFunction(L, LuaNames::Texture::setMinFiltering, setMinFiltering);
	LuaUtils::addFunction(L, LuaNames::Texture::setMagFiltering, setMagFiltering);
	LuaUtils::addFunction(L, LuaNames::Texture::setWrap, setWrap);

	lua_setfield(L, -2, LuaNames::Texture::Texture);
}

void LuaTexture::exposeConstants(lua_State *L)
{
	lua_createtable(L, 6, 0);

	LuaUtils::pushField(L, LuaNames::Texture::NEAREST, static_cast<int64_t>(Texture::Filtering::NEAREST));
	LuaUtils::pushField(L, LuaNames::Texture::LINEAR, static_cast<int64_t>(Texture::Filtering::LINEAR));
	LuaUtils::pushField(L, LuaNames::Texture::NEAREST_MIPMAP_NEAREST, static_cast<int64_t>(Texture::Filtering::NEAREST_MIPMAP_NEAREST));
	LuaUtils::pushField(L, LuaNames::Texture::LINEAR_MIPMAP_NEAREST, static_cast<int64_t>(Texture::Filtering::LINEAR_MIPMAP_NEAREST));
	LuaUtils::pushField(L, LuaNames::Texture::NEAREST_MIPMAP_LINEAR, static_cast<int64_t>(Texture::Filtering::NEAREST_MIPMAP_LINEAR));
	LuaUtils::pushField(L, LuaNames::Texture::LINEAR_MIPMAP_LINEAR, static_cast<int64_t>(Texture::Filtering::LINEAR_MIPMAP_LINEAR));

	lua_setfield(L, -2, LuaNames::Texture::Filtering);

	lua_createtable(L, 3, 0);

	LuaUtils::pushField(L, LuaNames::Texture::CLAMP_TO_EDGE, static_cast<int64_t>(Texture::Wrap::CLAMP_TO_EDGE));
	LuaUtils::pushField(L, LuaNames::Texture::MIRRORED_REPEAT, static_cast<int64_t>(Texture::Wrap::MIRRORED_REPEAT));
	LuaUtils::pushField(L, LuaNames::Texture::REPEAT, static_cast<int64_t>(Texture::Wrap::REPEAT));

	lua_setfield(L, -2, LuaNames::Texture::Wrap);
}

void LuaTexture::release(void *object)
{
	Texture *texture = reinterpret_cast<Texture *>(object);
	delete texture;
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

int LuaTexture::newObject(lua_State *L)
{
	const char *filename = LuaUtils::retrieve<const char *>(L, -1);

	LuaClassTracker<Texture>::newObject(L, filename);

	return 1;
}

int LuaTexture::width(lua_State *L)
{
	Texture *texture = LuaClassWrapper<Texture>::unwrapUserData(L, -1);

	const int width = texture->width();
	LuaUtils::push(L, width);

	return 1;
}

int LuaTexture::height(lua_State *L)
{
	Texture *texture = LuaClassWrapper<Texture>::unwrapUserData(L, -1);

	const int height = texture->height();
	LuaUtils::push(L, height);

	return 1;
}

int LuaTexture::mipMapLevels(lua_State *L)
{
	Texture *texture = LuaClassWrapper<Texture>::unwrapUserData(L, -1);

	const int mipMapLevels = texture->mipMapLevels();
	LuaUtils::push(L, mipMapLevels);

	return 1;
}

int LuaTexture::isCompressed(lua_State *L)
{
	Texture *texture = LuaClassWrapper<Texture>::unwrapUserData(L, -1);

	const bool isCompressed = texture->isCompressed();
	LuaUtils::push(L, isCompressed);

	return 1;
}

int LuaTexture::numChannels(lua_State *L)
{
	Texture *texture = LuaClassWrapper<Texture>::unwrapUserData(L, -1);

	const unsigned int numChannels = texture->numChannels();
	LuaUtils::push(L, numChannels);

	return 1;
}

int LuaTexture::dataSize(lua_State *L)
{
	Texture *texture = LuaClassWrapper<Texture>::unwrapUserData(L, -1);

	const unsigned long dataSize = texture->dataSize();
	LuaUtils::push(L, static_cast<uint64_t>(dataSize));

	return 1;
}

int LuaTexture::minFiltering(lua_State *L)
{
	Texture *texture = LuaClassWrapper<Texture>::unwrapUserData(L, -1);

	LuaUtils::push(L, static_cast<int64_t>(texture->minFiltering()));

	return 1;
}

int LuaTexture::magFiltering(lua_State *L)
{
	Texture *texture = LuaClassWrapper<Texture>::unwrapUserData(L, -1);

	LuaUtils::push(L, static_cast<int64_t>(texture->magFiltering()));

	return 1;
}

int LuaTexture::wrap(lua_State *L)
{
	Texture *texture = LuaClassWrapper<Texture>::unwrapUserData(L, -1);

	LuaUtils::push(L, static_cast<int64_t>(texture->wrap()));

	return 1;
}

int LuaTexture::setMinFiltering(lua_State *L)
{
	Texture *texture = LuaClassWrapper<Texture>::unwrapUserData(L, -2);
	const Texture::Filtering filter = static_cast<Texture::Filtering>(LuaUtils::retrieve<int64_t>(L, -1));

	texture->setMinFiltering(filter);

	return 0;
}

int LuaTexture::setMagFiltering(lua_State *L)
{
	Texture *texture = LuaClassWrapper<Texture>::unwrapUserData(L, -2);
	const Texture::Filtering filter = static_cast<Texture::Filtering>(LuaUtils::retrieve<int64_t>(L, -1));

	texture->setMagFiltering(filter);

	return 0;
}

int LuaTexture::setWrap(lua_State *L)
{
	Texture *texture = LuaClassWrapper<Texture>::unwrapUserData(L, -2);
	const Texture::Wrap wrapMode = static_cast<Texture::Wrap>(LuaUtils::retrieve<int64_t>(L, -1));

	texture->setWrap(wrapMode);

	return 0;
}

}
