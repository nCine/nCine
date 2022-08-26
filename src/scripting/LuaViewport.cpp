#include "LuaViewport.h"
#include "LuaUntrackedUserData.h"
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

	static const char *WITH_TEXTURE = "WITH_TEXTURE";
	static const char *NO_TEXTURE = "NO_TEXTURE";
	static const char *SCREEN = "SCREEN";
	static const char *Type = "viewport_type";

	static const char *EVERY_DRAW = "EVERY_DRAW";
	static const char *EVERY_FRAME = "EVERY_FRAME";
	static const char *THIS_FRAME_ONLY = "THIS_FRAME_ONLY";
	static const char *NEXT_FRAME_ONLY = "NEXT_FRAME_ONLY";
	static const char *NEVER = "NEVER";
	static const char *ClearMode = "clear_mode";

	static const char *NONE = "NONE";
	static const char *DEPTH16 = "DEPTH16";
	static const char *DEPTH24 = "DEPTH24";
	static const char *DEPTH24_STENCIL8 = "DEPTH24_STENCIL8";
	static const char *DepthStencilFormat = "depth_stencil_format";

	static const char *type = "get_type";

	static const char *texture = "get_texture";
	static const char *setTexture = "set_texture";

	static const char *depthStencilFormat = "get_depth_stencil_format";
	static const char *setDepthStencilFormat = "set_depth_stencil_format";

	static const char *removeAllTextures = "remove_all_textures";

	static const char *width = "get_width";
	static const char *height = "get_height";
	static const char *numColorAttachments = "get_num_color_attachments";

	static const char *viewportRect = "get_viewport_rect";
	static const char *setViewportRect = "set_viewport_rect";
	static const char *scissorRect = "get_scissor_rect";
	static const char *setScissorRect = "set_scissor_rect";
	static const char *cullingRect = "get_culling_rect";

	static const char *lastFrameCleared = "get_last_frame_cleared";

	static const char *clearMode = "get_clear_mode";
	static const char *setClearMode = "set_clear_mode";

	static const char *clearColor = "get_clear_color";
	static const char *setClearColor = "set_clear_color";

	static const char *rootNode = "get_rootnode";
	static const char *setRootNode = "set_rootnode";

	static const char *clearChain = "clear_chain";
	static const char *pushBackChain = "push_back_chain";
	static const char *pushFrontChain = "push_front_chain";

	static const char *camera = "get_camera";
	static const char *setCamera = "set_camera";

	static const char *setGLFramebufferLabel = "set_glframebuffer_label";
}}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void LuaViewport::exposeConstants(lua_State *L)
{
	lua_createtable(L, 0, 3);

	LuaUtils::pushField(L, LuaNames::Viewport::WITH_TEXTURE, static_cast<int64_t>(Viewport::Type::WITH_TEXTURE));
	LuaUtils::pushField(L, LuaNames::Viewport::NO_TEXTURE, static_cast<int64_t>(Viewport::Type::NO_TEXTURE));
	LuaUtils::pushField(L, LuaNames::Viewport::SCREEN, static_cast<int64_t>(Viewport::Type::SCREEN));

	lua_setfield(L, -2, LuaNames::Viewport::Type);

	lua_createtable(L, 0, 5);

	LuaUtils::pushField(L, LuaNames::Viewport::EVERY_DRAW, static_cast<int64_t>(Viewport::ClearMode::EVERY_DRAW));
	LuaUtils::pushField(L, LuaNames::Viewport::EVERY_FRAME, static_cast<int64_t>(Viewport::ClearMode::EVERY_FRAME));
	LuaUtils::pushField(L, LuaNames::Viewport::THIS_FRAME_ONLY, static_cast<int64_t>(Viewport::ClearMode::THIS_FRAME_ONLY));
	LuaUtils::pushField(L, LuaNames::Viewport::NEXT_FRAME_ONLY, static_cast<int64_t>(Viewport::ClearMode::NEXT_FRAME_ONLY));
	LuaUtils::pushField(L, LuaNames::Viewport::NEVER, static_cast<int64_t>(Viewport::ClearMode::NEVER));

	lua_setfield(L, -2, LuaNames::Viewport::ClearMode);

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
	lua_createtable(L, 0, 29);

	if (stateManager->apiType() == LuaStateManager::ApiType::FULL)
	{
		LuaClassTracker<Viewport>::exposeDelete(L);
		LuaUtils::addFunction(L, LuaNames::newObject, newObject);
	}

	LuaUtils::addFunction(L, LuaNames::Viewport::type, type);

	LuaUtils::addFunction(L, LuaNames::Viewport::texture, texture);
	LuaUtils::addFunction(L, LuaNames::Viewport::setTexture, setTexture);

	LuaUtils::addFunction(L, LuaNames::Viewport::depthStencilFormat, depthStencilFormat);
	LuaUtils::addFunction(L, LuaNames::Viewport::setDepthStencilFormat, setDepthStencilFormat);

	LuaUtils::addFunction(L, LuaNames::Viewport::removeAllTextures, removeAllTextures);

	LuaUtils::addFunction(L, LuaNames::Viewport::width, width);
	LuaUtils::addFunction(L, LuaNames::Viewport::height, height);
	LuaUtils::addFunction(L, LuaNames::Viewport::numColorAttachments, numColorAttachments);

	LuaUtils::addFunction(L, LuaNames::Viewport::viewportRect, viewportRect);
	LuaUtils::addFunction(L, LuaNames::Viewport::setViewportRect, setViewportRect);
	LuaUtils::addFunction(L, LuaNames::Viewport::scissorRect, scissorRect);
	LuaUtils::addFunction(L, LuaNames::Viewport::setScissorRect, setScissorRect);
	LuaUtils::addFunction(L, LuaNames::Viewport::cullingRect, cullingRect);

	LuaUtils::addFunction(L, LuaNames::Viewport::lastFrameCleared, lastFrameCleared);

	LuaUtils::addFunction(L, LuaNames::Viewport::clearMode, clearMode);
	LuaUtils::addFunction(L, LuaNames::Viewport::setClearMode, setClearMode);

	LuaUtils::addFunction(L, LuaNames::Viewport::clearColor, clearColor);
	LuaUtils::addFunction(L, LuaNames::Viewport::setClearColor, setClearColor);

	LuaUtils::addFunction(L, LuaNames::Viewport::rootNode, rootNode);
	LuaUtils::addFunction(L, LuaNames::Viewport::setRootNode, setRootNode);

	LuaUtils::addFunction(L, LuaNames::Viewport::clearChain, clearChain);
	LuaUtils::addFunction(L, LuaNames::Viewport::pushBackChain, pushBackChain);
	LuaUtils::addFunction(L, LuaNames::Viewport::pushFrontChain, pushFrontChain);

	LuaUtils::addFunction(L, LuaNames::Viewport::camera, camera);
	LuaUtils::addFunction(L, LuaNames::Viewport::setCamera, setCamera);

	LuaUtils::addFunction(L, LuaNames::Viewport::setGLFramebufferLabel, setGLFramebufferLabel);

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

int LuaViewport::type(lua_State *L)
{
	Viewport *viewport = LuaUntrackedUserData<Viewport>::retrieve(L, -1);

	if (viewport)
		LuaUtils::push(L, static_cast<int64_t>(viewport->type()));
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaViewport::texture(lua_State *L)
{
	Viewport *viewport = LuaUntrackedUserData<Viewport>::retrieve(L, -2);
	const unsigned int index = LuaUtils::retrieve<int>(L, -1);

	if (viewport)
		LuaUntrackedUserData<Texture>::push(L, viewport->texture(index));
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaViewport::setTexture(lua_State *L)
{
	Viewport *viewport = LuaUntrackedUserData<Viewport>::retrieve(L, -3);
	const unsigned int index = LuaUtils::retrieve<int>(L, -2);
	Texture *texture = LuaUntrackedUserData<Texture>::retrieveOrNil(L, -1);

	if (viewport)
		viewport->setTexture(index, texture);

	return 0;
}

int LuaViewport::depthStencilFormat(lua_State *L)
{
	Viewport *viewport = LuaUntrackedUserData<Viewport>::retrieve(L, -1);

	if (viewport)
		LuaUtils::push(L, static_cast<int64_t>(viewport->depthStencilFormat()));
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaViewport::setDepthStencilFormat(lua_State *L)
{
	Viewport *viewport = LuaUntrackedUserData<Viewport>::retrieve(L, -2);
	const Viewport::DepthStencilFormat depthStencilFormat = static_cast<Viewport::DepthStencilFormat>(LuaUtils::retrieve<int64_t>(L, -1));

	if (viewport)
		viewport->setDepthStencilFormat(depthStencilFormat);

	return 0;
}

int LuaViewport::removeAllTextures(lua_State *L)
{
	Viewport *viewport = LuaUntrackedUserData<Viewport>::retrieve(L, -1);

	if (viewport)
		viewport->removeAllTextures();

	return 0;
}

int LuaViewport::width(lua_State *L)
{
	Viewport *viewport = LuaUntrackedUserData<Viewport>::retrieve(L, -1);

	if (viewport)
		LuaUtils::push(L, viewport->width());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaViewport::height(lua_State *L)
{
	Viewport *viewport = LuaUntrackedUserData<Viewport>::retrieve(L, -1);

	if (viewport)
		LuaUtils::push(L, viewport->height());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaViewport::numColorAttachments(lua_State *L)
{
	Viewport *viewport = LuaUntrackedUserData<Viewport>::retrieve(L, -1);

	if (viewport)
		LuaUtils::push(L, viewport->numColorAttachments());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaViewport::viewportRect(lua_State *L)
{
	Viewport *viewport = LuaUntrackedUserData<Viewport>::retrieve(L, -1);

	if (viewport)
		LuaRectiUtils::push(L, viewport->viewportRect());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaViewport::setViewportRect(lua_State *L)
{
	int rectIndex = 0;
	const Recti viewportRect = LuaRectiUtils::retrieve(L, -1, rectIndex);
	Viewport *viewport = LuaUntrackedUserData<Viewport>::retrieve(L, rectIndex - 1);

	if (viewport)
		viewport->setViewportRect(viewportRect);

	return 0;
}

int LuaViewport::scissorRect(lua_State *L)
{
	Viewport *viewport = LuaUntrackedUserData<Viewport>::retrieve(L, -1);

	if (viewport)
		LuaRectiUtils::push(L, viewport->scissorRect());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaViewport::setScissorRect(lua_State *L)
{
	int rectIndex = 0;
	const Recti scissorRect = LuaRectiUtils::retrieve(L, -1, rectIndex);
	Viewport *viewport = LuaUntrackedUserData<Viewport>::retrieve(L, rectIndex - 1);

	if (viewport)
		viewport->setScissorRect(scissorRect);

	return 0;
}

int LuaViewport::cullingRect(lua_State *L)
{
	Viewport *viewport = LuaUntrackedUserData<Viewport>::retrieve(L, -1);

	if (viewport)
		LuaRectfUtils::push(L, viewport->cullingRect());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaViewport::lastFrameCleared(lua_State *L)
{
	Viewport *viewport = LuaUntrackedUserData<Viewport>::retrieve(L, -1);

	if (viewport)
		LuaUtils::push(L, static_cast<uint64_t>(viewport->lastFrameCleared()));
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaViewport::clearMode(lua_State *L)
{
	Viewport *viewport = LuaUntrackedUserData<Viewport>::retrieve(L, -1);

	if (viewport)
		LuaUtils::push(L, static_cast<int64_t>(viewport->clearMode()));
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaViewport::setClearMode(lua_State *L)
{
	Viewport *viewport = LuaUntrackedUserData<Viewport>::retrieve(L, -2);
	const Viewport::ClearMode clearMode = static_cast<Viewport::ClearMode>(LuaUtils::retrieve<int64_t>(L, -1));

	if (viewport)
		viewport->setClearMode(clearMode);

	return 0;
}

int LuaViewport::clearColor(lua_State *L)
{
	Viewport *viewport = LuaUntrackedUserData<Viewport>::retrieve(L, -1);

	if (viewport)
		LuaColorUtils::push(L, viewport->clearColor());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaViewport::setClearColor(lua_State *L)
{
	int colorIndex = 0;
	const Colorf clearColor = LuaColorUtils::retrieve(L, -1, colorIndex);
	Viewport *viewport = LuaUntrackedUserData<Viewport>::retrieve(L, colorIndex - 1);

	if (viewport)
		viewport->setClearColor(clearColor);

	return 0;
}

int LuaViewport::rootNode(lua_State *L)
{
	Viewport *viewport = LuaUntrackedUserData<Viewport>::retrieve(L, -1);

	if (viewport)
		LuaUntrackedUserData<SceneNode>::push(L, viewport->rootNode());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaViewport::setRootNode(lua_State *L)
{
	Viewport *viewport = LuaUntrackedUserData<Viewport>::retrieve(L, -2);
	SceneNode *rootNode = LuaUntrackedUserData<SceneNode>::retrieveOrNil(L, -1);

	if (viewport)
		viewport->setRootNode(rootNode);

	return 0;
}

int LuaViewport::clearChain(lua_State *L)
{
	Viewport::chain().clear();

	return 0;
}

int LuaViewport::pushBackChain(lua_State *L)
{
	Viewport *viewport = LuaUntrackedUserData<Viewport>::retrieve(L, -1);

	if (viewport)
		Viewport::chain().pushBack(viewport);

	return 0;
}

int LuaViewport::pushFrontChain(lua_State *L)
{
	Viewport *viewport = LuaUntrackedUserData<Viewport>::retrieve(L, -1);

	if (viewport)
		Viewport::chain().insertAt(0, viewport);

	return 0;
}

int LuaViewport::camera(lua_State *L)
{
	Viewport *viewport = LuaUntrackedUserData<Viewport>::retrieve(L, -1);

	if (viewport)
		LuaUntrackedUserData<Camera>::push(L, viewport->camera());
	else
		LuaUtils::pushNil(L);

	return 1;
}

int LuaViewport::setCamera(lua_State *L)
{
	Viewport *viewport = LuaUntrackedUserData<Viewport>::retrieve(L, -2);
	Camera *camera = LuaUntrackedUserData<Camera>::retrieve(L, -1);

	if (viewport)
		viewport->setCamera(camera);

	return 0;
}

int LuaViewport::setGLFramebufferLabel(lua_State *L)
{
	Viewport *viewport = LuaUntrackedUserData<Viewport>::retrieve(L, -2);
	const char *label = LuaUtils::retrieve<const char *>(L, -1);

	if (viewport)
		viewport->setGLFramebufferLabel(label);

	return 0;
}

}
