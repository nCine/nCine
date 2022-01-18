#include "LuaViewport.h"
#include "LuaClassWrapper.h"
#include "LuaClassTracker.h"
#include "LuaRectUtils.h"
#include "LuaColorUtils.h"
#include "LuaUtils.h"
#include "Viewport.h"

namespace ncine {

class Camera;

namespace LuaNames {
namespace Viewport {
	static const char *Viewport = "viewport";

	static const char *EVERY_FRAME = "EVERY_FRAME";
	static const char *THIS_FRAME_ONLY = "THIS_FRAME_ONLY";
	static const char *NEXT_FRAME_ONLY = "NEXT_FRAME_ONLY";
	static const char *NEVER = "NEVER";
	static const char *ClearMode = "clear_mode";

	static const char *RGB8 = "RGB8";
	static const char *RGBA8 = "RGBA8";
	static const char *ColorFormat = "color_format";

	static const char *NONE = "NONE";
	static const char *DEPTH16 = "DEPTH16";
	static const char *DEPTH24 = "DEPTH24";
	static const char *DEPTH24_STENCIL8 = "DEPTH24_STENCIL8";
	static const char *DepthStencilFormat = "depth_stencil_format";

	static const char *initTexture = "init_texture";

	static const char *width = "get_width";
	static const char *height = "get_height";

	static const char *viewportRect = "get_viewport_rect";
	static const char *setViewportRect = "set_viewport_rect";
	static const char *scissorRect = "get_scissor_rect";
	static const char *setScissorRect = "set_scissor_rect";
	static const char *cullingRect = "get_culling_rect";

	static const char *colorFormat = "get_color_format";
	static const char *depthStencilFormat = "get_depth_stencil_format";

	static const char *clearMode = "get_clear_mode";
	static const char *setClearMode = "set_clear_mode";

	static const char *clearColor = "get_clear_color";
	static const char *setClearColor = "set_clear_color";

	static const char *texture = "get_texture";

	static const char *rootNode = "get_rootnode";
	static const char *setRootNode = "set_rootnode";

	static const char *nextViewport = "get_next_viewport";
	static const char *setNextViewport = "set_next_viewport";

	static const char *camera = "get_camera";
	static const char *setCamera = "set_camera";
}}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void LuaViewport::exposeConstants(lua_State *L)
{
	lua_createtable(L, 0, 4);

	LuaUtils::pushField(L, LuaNames::Viewport::EVERY_FRAME, static_cast<int64_t>(Viewport::ClearMode::EVERY_FRAME));
	LuaUtils::pushField(L, LuaNames::Viewport::THIS_FRAME_ONLY, static_cast<int64_t>(Viewport::ClearMode::THIS_FRAME_ONLY));
	LuaUtils::pushField(L, LuaNames::Viewport::NEXT_FRAME_ONLY, static_cast<int64_t>(Viewport::ClearMode::NEXT_FRAME_ONLY));
	LuaUtils::pushField(L, LuaNames::Viewport::NEVER, static_cast<int64_t>(Viewport::ClearMode::NEVER));

	lua_setfield(L, -2, LuaNames::Viewport::ClearMode);

	lua_createtable(L, 0, 2);

	LuaUtils::pushField(L, LuaNames::Viewport::RGB8, static_cast<int64_t>(Viewport::ColorFormat::RGB8));
	LuaUtils::pushField(L, LuaNames::Viewport::RGBA8, static_cast<int64_t>(Viewport::ColorFormat::RGBA8));

	lua_setfield(L, -2, LuaNames::Viewport::ColorFormat);

	lua_createtable(L, 0, 4);

	LuaUtils::pushField(L, LuaNames::Viewport::NONE, static_cast<int64_t>(Viewport::DepthStencilFormat::NONE));
	LuaUtils::pushField(L, LuaNames::Viewport::DEPTH16, static_cast<int64_t>(Viewport::DepthStencilFormat::DEPTH16));
	LuaUtils::pushField(L, LuaNames::Viewport::DEPTH24, static_cast<int64_t>(Viewport::DepthStencilFormat::DEPTH24));
	LuaUtils::pushField(L, LuaNames::Viewport::DEPTH24_STENCIL8, static_cast<int64_t>(Viewport::DepthStencilFormat::DEPTH24_STENCIL8));

	lua_setfield(L, -2, LuaNames::Viewport::DepthStencilFormat);
}

void LuaViewport::expose(LuaStateManager *stateManager)
{
	lua_State *L = stateManager->state();
	lua_newtable(L);

	if (stateManager->apiType() == LuaStateManager::ApiType::FULL)
	{
		LuaClassTracker<Viewport>::exposeDelete(L);
		LuaUtils::addFunction(L, LuaNames::newObject, newObject);
	}

	LuaUtils::addFunction(L, LuaNames::Viewport::initTexture, initTexture);

	LuaUtils::addFunction(L, LuaNames::Viewport::width, width);
	LuaUtils::addFunction(L, LuaNames::Viewport::height, height);

	LuaUtils::addFunction(L, LuaNames::Viewport::viewportRect, viewportRect);
	LuaUtils::addFunction(L, LuaNames::Viewport::setViewportRect, setViewportRect);
	LuaUtils::addFunction(L, LuaNames::Viewport::scissorRect, scissorRect);
	LuaUtils::addFunction(L, LuaNames::Viewport::setScissorRect, setScissorRect);
	LuaUtils::addFunction(L, LuaNames::Viewport::cullingRect, cullingRect);

	LuaUtils::addFunction(L, LuaNames::Viewport::colorFormat, colorFormat);
	LuaUtils::addFunction(L, LuaNames::Viewport::depthStencilFormat, depthStencilFormat);

	LuaUtils::addFunction(L, LuaNames::Viewport::clearMode, clearMode);
	LuaUtils::addFunction(L, LuaNames::Viewport::setClearMode, setClearMode);

	LuaUtils::addFunction(L, LuaNames::Viewport::clearColor, clearColor);
	LuaUtils::addFunction(L, LuaNames::Viewport::setClearColor, setClearColor);

	LuaUtils::addFunction(L, LuaNames::Viewport::texture, texture);

	LuaUtils::addFunction(L, LuaNames::Viewport::rootNode, rootNode);
	LuaUtils::addFunction(L, LuaNames::Viewport::setRootNode, setRootNode);

	LuaUtils::addFunction(L, LuaNames::Viewport::nextViewport, nextViewport);
	LuaUtils::addFunction(L, LuaNames::Viewport::setNextViewport, setNextViewport);

	LuaUtils::addFunction(L, LuaNames::Viewport::camera, camera);
	LuaUtils::addFunction(L, LuaNames::Viewport::setCamera, setCamera);

	lua_setfield(L, -2, LuaNames::Viewport::Viewport);
}

void LuaViewport::release(void *object)
{
	Viewport *viewport = reinterpret_cast<Viewport *>(object);
	delete viewport;
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

int LuaViewport::newObject(lua_State *L)
{
	LuaClassTracker<Viewport>::newObject(L);

	return 1;
}

int LuaViewport::initTexture(lua_State *L)
{
	Viewport *viewport = LuaClassWrapper<Viewport>::unwrapUserData(L, -5);
	const int width = LuaUtils::retrieve<int>(L, -4);
	const int height = LuaUtils::retrieve<int>(L, -3);
	const Viewport::ColorFormat colorFormat = static_cast<Viewport::ColorFormat>(LuaUtils::retrieve<int64_t>(L, -2));
	const Viewport::DepthStencilFormat depthStencilFormat = static_cast<Viewport::DepthStencilFormat>(LuaUtils::retrieve<int64_t>(L, -1));

	viewport->initTexture(width, height, colorFormat, depthStencilFormat);

	return 0;
}

int LuaViewport::width(lua_State *L)
{
	Viewport *viewport = LuaClassWrapper<Viewport>::unwrapUserData(L, -1);

	LuaUtils::push(L, viewport->width());

	return 1;
}

int LuaViewport::height(lua_State *L)
{
	Viewport *viewport = LuaClassWrapper<Viewport>::unwrapUserData(L, -1);

	LuaUtils::push(L, viewport->height());

	return 1;
}

int LuaViewport::viewportRect(lua_State *L)
{
	Viewport *viewport = LuaClassWrapper<Viewport>::unwrapUserData(L, -1);
	const Recti viewportRect = viewport->viewportRect();

	LuaRectiUtils::push(L, viewportRect);

	return 1;
}

int LuaViewport::setViewportRect(lua_State *L)
{
	int rectIndex = 0;
	const Recti viewportRect = LuaRectiUtils::retrieve(L, -1, rectIndex);
	Viewport *viewport = LuaClassWrapper<Viewport>::unwrapUserData(L, rectIndex - 1);

	viewport->setViewportRect(viewportRect);

	return 0;
}

int LuaViewport::scissorRect(lua_State *L)
{
	Viewport *viewport = LuaClassWrapper<Viewport>::unwrapUserData(L, -1);
	const Recti scissorRect = viewport->scissorRect();

	LuaRectiUtils::push(L, scissorRect);

	return 1;
}

int LuaViewport::setScissorRect(lua_State *L)
{
	int rectIndex = 0;
	const Recti scissorRect = LuaRectiUtils::retrieve(L, -1, rectIndex);
	Viewport *viewport = LuaClassWrapper<Viewport>::unwrapUserData(L, rectIndex - 1);

	viewport->setScissorRect(scissorRect);

	return 0;
}

int LuaViewport::cullingRect(lua_State *L)
{
	Viewport *viewport = LuaClassWrapper<Viewport>::unwrapUserData(L, -1);
	const Rectf cullingRect = viewport->cullingRect();

	LuaRectfUtils::push(L, cullingRect);

	return 1;
}

int LuaViewport::colorFormat(lua_State *L)
{
	Viewport *viewport = LuaClassWrapper<Viewport>::unwrapUserData(L, -1);
	const Viewport::ColorFormat colorFormat = viewport->colorFormat();

	LuaUtils::push(L, static_cast<int64_t>(colorFormat));

	return 1;
}

int LuaViewport::depthStencilFormat(lua_State *L)
{
	Viewport *viewport = LuaClassWrapper<Viewport>::unwrapUserData(L, -1);
	const Viewport::DepthStencilFormat depthStencilFormat = viewport->depthStencilFormat();

	LuaUtils::push(L, static_cast<int64_t>(depthStencilFormat));

	return 1;
}

int LuaViewport::clearMode(lua_State *L)
{
	Viewport *viewport = LuaClassWrapper<Viewport>::unwrapUserData(L, -1);
	const Viewport::ClearMode clearMode = viewport->clearMode();

	LuaUtils::push(L, static_cast<int64_t>(clearMode));

	return 1;
}

int LuaViewport::setClearMode(lua_State *L)
{
	Viewport *viewport = LuaClassWrapper<Viewport>::unwrapUserData(L, -2);
	const Viewport::ClearMode clearMode = static_cast<Viewport::ClearMode>(LuaUtils::retrieve<int64_t>(L, -1));

	viewport->setClearMode(clearMode);

	return 0;
}

int LuaViewport::clearColor(lua_State *L)
{
	Viewport *viewport = LuaClassWrapper<Viewport>::unwrapUserData(L, -1);
	const Colorf clearColor = viewport->clearColor();

	LuaColorUtils::push(L, clearColor);

	return 1;
}

int LuaViewport::setClearColor(lua_State *L)
{
	int colorIndex = 0;
	const Colorf clearColor = LuaColorUtils::retrieve(L, -1, colorIndex);
	Viewport *viewport = LuaClassWrapper<Viewport>::unwrapUserData(L, colorIndex - 1);

	viewport->setClearColor(clearColor);

	return 0;
}

int LuaViewport::texture(lua_State *L)
{
	Viewport *viewport = LuaClassWrapper<Viewport>::unwrapUserData(L, -1);

	LuaClassWrapper<Texture>::pushUntrackedUserData(L, viewport->texture());

	return 1;
}

int LuaViewport::rootNode(lua_State *L)
{
	Viewport *viewport = LuaClassWrapper<Viewport>::unwrapUserData(L, -1);

	LuaClassWrapper<SceneNode>::pushUntrackedUserData(L, viewport->rootNode());

	return 1;
}

int LuaViewport::setRootNode(lua_State *L)
{
	Viewport *viewport = LuaClassWrapper<Viewport>::unwrapUserData(L, -2);
	SceneNode *rootNode = LuaClassWrapper<SceneNode>::unwrapUserData(L, -1);

	viewport->setRootNode(rootNode);

	return 0;
}

int LuaViewport::nextViewport(lua_State *L)
{
	Viewport *viewport = LuaClassWrapper<Viewport>::unwrapUserData(L, -1);

	LuaClassWrapper<Viewport>::pushUntrackedUserData(L, viewport->nextViewport());

	return 1;
}

int LuaViewport::setNextViewport(lua_State *L)
{
	Viewport *viewport = LuaClassWrapper<Viewport>::unwrapUserData(L, -2);
	Viewport *nextViewport = LuaClassWrapper<Viewport>::unwrapUserData(L, -1);

	viewport->setNextViewport(nextViewport);

	return 0;
}

int LuaViewport::camera(lua_State *L)
{
	Viewport *viewport = LuaClassWrapper<Viewport>::unwrapUserData(L, -1);

	LuaClassWrapper<Camera>::pushUntrackedUserData(L, viewport->camera());

	return 1;
}

int LuaViewport::setCamera(lua_State *L)
{
	Viewport *viewport = LuaClassWrapper<Viewport>::unwrapUserData(L, -2);
	Camera *camera = LuaClassWrapper<Camera>::unwrapUserData(L, -1);

	viewport->setCamera(camera);

	return 0;
}

}
