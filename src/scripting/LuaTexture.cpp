#include "LuaTexture.h"
#include "LuaUntrackedUserData.h"
#include "LuaClassTracker.h"
#include "LuaColorUtils.h"
#include "LuaVector2Utils.h"
#include "LuaUtils.h"
#include "Texture.h"

namespace ncine {

namespace LuaNames {
namespace Texture {
	static const char *Texture = "texture";

	static const char *newInit = "new_init";
	static const char *init = "init";

	static const char *width = "get_width";
	static const char *height = "get_height";
	static const char *mipMapLevels = "mip_levels";
	static const char *isCompressed = "is_compressed";
	static const char *format = "get_format";
	static const char *numChannels = "num_channels";
	static const char *dataSize = "datasize";

	static const char *minFiltering = "get_min_filtering";
	static const char *magFiltering = "get_mag_filtering";
	static const char *wrap = "get_wrap";
	static const char *setMinFiltering = "set_min_filtering";
	static const char *setMagFiltering = "set_mag_filtering";
	static const char *setWrap = "set_wrap";

	static const char *isChromaKeyEnabled = "is_chromakey_enabled";
	static const char *chromaKeyColor = "get_chromakey_color";
	static const char *setChromaKeyEnabled = "set_chromakey_enabled";
	static const char *setChromaKeyColor = "set_chromakey_color";

	static const char *setGLTextureLabel = "set_gltexture_label";

	static const char *R8 = "R8";
	static const char *RG8 = "RG8";
	static const char *RGB8 = "RGB8";
	static const char *RGBA8 = "RGBA8";
	static const char *UNKNOWN = "UNKNOWN";
	static const char *Format = "tex_format";

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
	lua_createtable(L, 0, 22);

	if (stateManager->apiType() == LuaStateManager::ApiType::FULL)
	{
		LuaClassTracker<Texture>::exposeDelete(L);
		LuaUtils::addFunction(L, LuaNames::newObject, newObject);
		LuaUtils::addFunction(L, LuaNames::Texture::newInit, newInit);
	}

	LuaUtils::addFunction(L, LuaNames::Texture::init, init);
	LuaUtils::addFunction(L, LuaNames::Texture::width, width);
	LuaUtils::addFunction(L, LuaNames::Texture::height, height);
	LuaUtils::addFunction(L, LuaNames::Texture::mipMapLevels, mipMapLevels);
	LuaUtils::addFunction(L, LuaNames::Texture::isCompressed, isCompressed);
	LuaUtils::addFunction(L, LuaNames::Texture::format, format);
	LuaUtils::addFunction(L, LuaNames::Texture::numChannels, numChannels);
	LuaUtils::addFunction(L, LuaNames::Texture::dataSize, dataSize);

	LuaUtils::addFunction(L, LuaNames::Texture::minFiltering, minFiltering);
	LuaUtils::addFunction(L, LuaNames::Texture::magFiltering, magFiltering);
	LuaUtils::addFunction(L, LuaNames::Texture::wrap, wrap);
	LuaUtils::addFunction(L, LuaNames::Texture::setMinFiltering, setMinFiltering);
	LuaUtils::addFunction(L, LuaNames::Texture::setMagFiltering, setMagFiltering);
	LuaUtils::addFunction(L, LuaNames::Texture::setWrap, setWrap);

	LuaUtils::addFunction(L, LuaNames::Texture::isChromaKeyEnabled, isChromaKeyEnabled);
	LuaUtils::addFunction(L, LuaNames::Texture::chromaKeyColor, chromaKeyColor);
	LuaUtils::addFunction(L, LuaNames::Texture::setChromaKeyEnabled, setChromaKeyEnabled);
	LuaUtils::addFunction(L, LuaNames::Texture::setChromaKeyColor, setChromaKeyColor);

	LuaUtils::addFunction(L, LuaNames::Texture::setGLTextureLabel, setGLTextureLabel);

	lua_setfield(L, -2, LuaNames::Texture::Texture);
}

void LuaTexture::exposeConstants(lua_State *L)
{
	lua_createtable(L, 0, 5);

	LuaUtils::pushField(L, LuaNames::Texture::R8, static_cast<int64_t>(Texture::Format::R8));
	LuaUtils::pushField(L, LuaNames::Texture::RG8, static_cast<int64_t>(Texture::Format::RG8));
	LuaUtils::pushField(L, LuaNames::Texture::RGB8, static_cast<int64_t>(Texture::Format::RGB8));
	LuaUtils::pushField(L, LuaNames::Texture::RGBA8, static_cast<int64_t>(Texture::Format::RGBA8));
	LuaUtils::pushField(L, LuaNames::Texture::UNKNOWN, static_cast<int64_t>(Texture::Format::UNKNOWN));

	lua_setfield(L, -2, LuaNames::Texture::Format);

	lua_createtable(L, 0, 6);

	LuaUtils::pushField(L, LuaNames::Texture::NEAREST, static_cast<int64_t>(Texture::Filtering::NEAREST));
	LuaUtils::pushField(L, LuaNames::Texture::LINEAR, static_cast<int64_t>(Texture::Filtering::LINEAR));
	LuaUtils::pushField(L, LuaNames::Texture::NEAREST_MIPMAP_NEAREST, static_cast<int64_t>(Texture::Filtering::NEAREST_MIPMAP_NEAREST));
	LuaUtils::pushField(L, LuaNames::Texture::LINEAR_MIPMAP_NEAREST, static_cast<int64_t>(Texture::Filtering::LINEAR_MIPMAP_NEAREST));
	LuaUtils::pushField(L, LuaNames::Texture::NEAREST_MIPMAP_LINEAR, static_cast<int64_t>(Texture::Filtering::NEAREST_MIPMAP_LINEAR));
	LuaUtils::pushField(L, LuaNames::Texture::LINEAR_MIPMAP_LINEAR, static_cast<int64_t>(Texture::Filtering::LINEAR_MIPMAP_LINEAR));

	lua_setfield(L, -2, LuaNames::Texture::Filtering);

	lua_createtable(L, 0, 3);

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

int LuaTexture::newInit(lua_State *L)
{
	int vectorIndex = 0;
	const Vector2i &size = LuaVector2iUtils::retrieve(L, -1, vectorIndex);

	const char *name = LuaUtils::retrieve<const char *>(L, vectorIndex - 3);
	const Texture::Format format = static_cast<Texture::Format>(LuaUtils::retrieve<int64_t>(L, vectorIndex - 2));
	const int mipCount = LuaUtils::retrieve<int>(L, vectorIndex - 1);

	LuaClassTracker<Texture>::newObject(L, name, format, mipCount, size);

	return 1;
}

int LuaTexture::init(lua_State *L)
{
	int vectorIndex = 0;
	const Vector2i &size = LuaVector2iUtils::retrieve(L, -1, vectorIndex);

	Texture *texture = LuaUntrackedUserData<Texture>::retrieve(L, vectorIndex - 4);
	const char *name = LuaUtils::retrieve<const char *>(L, vectorIndex - 3);
	const Texture::Format format = static_cast<Texture::Format>(LuaUtils::retrieve<int64_t>(L, vectorIndex - 2));
	const int mipCount = LuaUtils::retrieve<int>(L, vectorIndex - 1);

	if (texture)
		texture->init(name, format, mipCount, size);

	return 0;
}

int LuaTexture::width(lua_State *L)
{
	Texture *texture = LuaUntrackedUserData<Texture>::retrieve(L, -1);

	if (texture)
		LuaUtils::push(L, texture->width());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaTexture::height(lua_State *L)
{
	Texture *texture = LuaUntrackedUserData<Texture>::retrieve(L, -1);

	if (texture)
		LuaUtils::push(L, texture->height());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaTexture::mipMapLevels(lua_State *L)
{
	Texture *texture = LuaUntrackedUserData<Texture>::retrieve(L, -1);

	if (texture)
		LuaUtils::push(L, texture->mipMapLevels());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaTexture::isCompressed(lua_State *L)
{
	Texture *texture = LuaUntrackedUserData<Texture>::retrieve(L, -1);

	if (texture)
		LuaUtils::push(L, texture->isCompressed());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaTexture::format(lua_State *L)
{
	Texture *texture = LuaUntrackedUserData<Texture>::retrieve(L, -1);

	if (texture)
		LuaUtils::push(L, static_cast<int64_t>(texture->format()));
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaTexture::numChannels(lua_State *L)
{
	Texture *texture = LuaUntrackedUserData<Texture>::retrieve(L, -1);

	if (texture)
		LuaUtils::push(L, texture->numChannels());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaTexture::dataSize(lua_State *L)
{
	Texture *texture = LuaUntrackedUserData<Texture>::retrieve(L, -1);

	if (texture)
		LuaUtils::push(L, static_cast<uint64_t>(texture->dataSize()));
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaTexture::minFiltering(lua_State *L)
{
	Texture *texture = LuaUntrackedUserData<Texture>::retrieve(L, -1);

	if (texture)
		LuaUtils::push(L, static_cast<int64_t>(texture->minFiltering()));
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaTexture::magFiltering(lua_State *L)
{
	Texture *texture = LuaUntrackedUserData<Texture>::retrieve(L, -1);

	if (texture)
		LuaUtils::push(L, static_cast<int64_t>(texture->magFiltering()));
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaTexture::wrap(lua_State *L)
{
	Texture *texture = LuaUntrackedUserData<Texture>::retrieve(L, -1);

	if (texture)
		LuaUtils::push(L, static_cast<int64_t>(texture->wrap()));
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaTexture::setMinFiltering(lua_State *L)
{
	Texture *texture = LuaUntrackedUserData<Texture>::retrieve(L, -2);
	const Texture::Filtering filter = static_cast<Texture::Filtering>(LuaUtils::retrieve<int64_t>(L, -1));

	if (texture)
		texture->setMinFiltering(filter);

	return 0;
}

int LuaTexture::setMagFiltering(lua_State *L)
{
	Texture *texture = LuaUntrackedUserData<Texture>::retrieve(L, -2);
	const Texture::Filtering filter = static_cast<Texture::Filtering>(LuaUtils::retrieve<int64_t>(L, -1));

	if (texture)
		texture->setMagFiltering(filter);

	return 0;
}

int LuaTexture::setWrap(lua_State *L)
{
	Texture *texture = LuaUntrackedUserData<Texture>::retrieve(L, -2);
	const Texture::Wrap wrapMode = static_cast<Texture::Wrap>(LuaUtils::retrieve<int64_t>(L, -1));

	if (texture)
		texture->setWrap(wrapMode);

	return 0;
}

int LuaTexture::isChromaKeyEnabled(lua_State *L)
{
	Texture *texture = LuaUntrackedUserData<Texture>::retrieve(L, -1);

	if (texture)
		LuaUtils::push(L, texture->isChromaKeyEnabled());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaTexture::chromaKeyColor(lua_State *L)
{
	Texture *texture = LuaUntrackedUserData<Texture>::retrieve(L, -1);

	if (texture)
		LuaColorUtils::push(L, Colorf(texture->chromaKeyColor()));
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaTexture::setChromaKeyEnabled(lua_State *L)
{
	Texture *texture = LuaUntrackedUserData<Texture>::retrieve(L, -2);
	const bool chromaKeyEnabled = LuaUtils::retrieve<bool>(L, -1);

	if (texture)
		texture->setChromaKeyEnabled(chromaKeyEnabled);

	return 0;
}

int LuaTexture::setChromaKeyColor(lua_State *L)
{
	int colorIndex = 0;
	const Colorf chromaKeyColor = LuaColorUtils::retrieve(L, -1, colorIndex);
	Texture *texture = LuaUntrackedUserData<Texture>::retrieve(L, colorIndex - 1);

	if (texture)
		texture->setChromaKeyColor(chromaKeyColor);

	return 0;
}

int LuaTexture::setGLTextureLabel(lua_State *L)
{
	Texture *texture = LuaUntrackedUserData<Texture>::retrieve(L, -2);
	const char *label = LuaUtils::retrieve<const char *>(L, -1);

	if (texture)
		texture->setGLTextureLabel(label);

	return 0;
}

}
