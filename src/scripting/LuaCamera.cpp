#include "LuaCamera.h"
#include "LuaClassWrapper.h"
#include "LuaClassTracker.h"
#include "LuaUtils.h"
#include "Camera.h"

namespace ncine {

namespace LuaNames {
namespace Camera {
	static const char *Camera = "camera";

	static const char *left = "left";
	static const char *right = "right";
	static const char *top = "top";
	static const char *bottom = "bottom";

	static const char *x = "x";
	static const char *y = "y";
	static const char *rotation = "rotation";
	static const char *scale = "scale";

	static const char *projectionValues = "get_projection_values";
	static const char *viewValues = "get_view_values";

	static const char *setOrthoProjection = "set_ortho_projection";
	static const char *setView = "set_view";
}}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void LuaCamera::expose(LuaStateManager *stateManager)
{
	lua_State *L = stateManager->state();
	lua_newtable(L);

	if (stateManager->apiType() == LuaStateManager::ApiType::FULL)
	{
		LuaClassTracker<Camera>::exposeDelete(L);
		LuaUtils::addFunction(L, LuaNames::newObject, newObject);
	}

	LuaUtils::addFunction(L, LuaNames::Camera::projectionValues, projectionValues);
	LuaUtils::addFunction(L, LuaNames::Camera::viewValues, viewValues);

	LuaUtils::addFunction(L, LuaNames::Camera::setOrthoProjection, setOrthoProjection);
	LuaUtils::addFunction(L, LuaNames::Camera::setView, setView);

	lua_setfield(L, -2, LuaNames::Camera::Camera);
}

void LuaCamera::release(void *object)
{
	Camera *camera = reinterpret_cast<Camera *>(object);
	delete camera;
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

int LuaCamera::newObject(lua_State *L)
{
	LuaClassTracker<Camera>::newObject(L);

	return 1;
}

int LuaCamera::projectionValues(lua_State *L)
{
	Camera *camera = LuaClassWrapper<Camera>::unwrapUserData(L, -1);
	const Camera::ProjectionValues projectionValues = camera->projectionValues();

	LuaUtils::createTable(L, 0, 4);
	LuaUtils::pushField(L, LuaNames::Camera::left, projectionValues.left);
	LuaUtils::pushField(L, LuaNames::Camera::right, projectionValues.right);
	LuaUtils::pushField(L, LuaNames::Camera::top, projectionValues.top);
	LuaUtils::pushField(L, LuaNames::Camera::bottom, projectionValues.bottom);

	return 1;
}

int LuaCamera::viewValues(lua_State *L)
{
	Camera *camera = LuaClassWrapper<Camera>::unwrapUserData(L, -1);
	const Camera::ViewValues viewValues = camera->viewValues();

	LuaUtils::createTable(L, 0, 4);
	LuaUtils::pushField(L, LuaNames::Camera::x, viewValues.position.x);
	LuaUtils::pushField(L, LuaNames::Camera::y, viewValues.position.y);
	LuaUtils::pushField(L, LuaNames::Camera::rotation, viewValues.rotation);
	LuaUtils::pushField(L, LuaNames::Camera::scale, viewValues.scale);

	return 1;
}

int LuaCamera::setOrthoProjection(lua_State *L)
{
	if (lua_istable(L, -1) == false)
	{
		LOGW("Expecting a table at index -1");
		return 0;
	}

	Camera *camera = LuaClassWrapper<Camera>::unwrapUserData(L, -2);
	Camera::ProjectionValues projectionValues;
	projectionValues.left = LuaUtils::retrieveField<float>(L, -1, LuaNames::Camera::left);
	projectionValues.right = LuaUtils::retrieveField<float>(L, -1, LuaNames::Camera::right);
	projectionValues.top = LuaUtils::retrieveField<float>(L, -1, LuaNames::Camera::top);
	projectionValues.bottom = LuaUtils::retrieveField<float>(L, -1, LuaNames::Camera::bottom);

	camera->setOrthoProjection(projectionValues);

	return 0;
}

int LuaCamera::setView(lua_State *L)
{
	if (lua_istable(L, -1) == false)
	{
		LOGW("Expecting a table at index -1");
		return 0;
	}

	Camera *camera = LuaClassWrapper<Camera>::unwrapUserData(L, -2);
	Camera::ViewValues viewValues;
	viewValues.position.x = LuaUtils::retrieveField<float>(L, -1, LuaNames::Camera::x);
	viewValues.position.y = LuaUtils::retrieveField<float>(L, -1, LuaNames::Camera::y);
	viewValues.rotation = LuaUtils::retrieveField<float>(L, -1, LuaNames::Camera::rotation);
	viewValues.scale = LuaUtils::retrieveField<float>(L, -1, LuaNames::Camera::scale);

	camera->setView(viewValues);

	return 0;
}

}
