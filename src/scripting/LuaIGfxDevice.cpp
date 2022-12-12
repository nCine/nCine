#define NCINE_INCLUDE_LUA
#include "common_headers.h"

#include "LuaIGfxDevice.h"
#include "LuaApplication.h"
#include "LuaUntrackedUserData.h"
#include "LuaVector2Utils.h"
#include "IGfxDevice.h"
#include "Application.h"

namespace ncine {

namespace LuaNames {
namespace VideoMode {
	static const char *width = "width";
	static const char *height = "height";
	static const char *refreshRate = "refresh_rate";
	static const char *redBits = "red_bits";
	static const char *greenBits = "green_bits";
	static const char *blueBits = "blue_bits";
}

namespace Monitor {
	static const char *name = "name";
	static const char *position = "position";
	static const char *scale = "scale";
	static const char *numVideoModes = "num_video_modes";
	static const char *videoModes = "video_modes";
}

namespace IGfxDevice {
	static const char *IGfxDevice = "gfx_device";

	static const char *setSwapInterval = "set_swap_interval";

	static const char *isFullScreen = "is_fullscreen";
	static const char *setFullScreen = "set_fullscreen";

	static const char *isResizable = "is_resizable";

	static const char *windowPositionX = "get_window_position_x";
	static const char *windowPositionY = "get_window_position_y";
	static const char *windowPosition = "get_window_position";
	static const char *setWindowPosition = "set_window_position";

	static const char *width = "get_width";
	static const char *height = "get_height";
	static const char *resolution = "get_resolution";
	static const char *aspect = "get_aspect_ratio";
	static const char *setWindowSize = "set_window_size";

	static const char *drawableWidth = "get_drawable_width";
	static const char *drawableHeight = "get_hdrawable_eight";
	static const char *drawableResolution = "get_drawable_resolution";

	static const char *setWindowTitle = "set_window_title";
	static const char *setWindowIcon = "set_window_icon";
	static const char *flashWindow = "flash_window";

	static const char *numMonitors = "num_monitors";
	static const char *primaryMonitorIndex = "get_primary_monitor_index";
	static const char *windowMonitorIndex = "get_window_monitor_index";
	static const char *monitor = "get_monitor";

	static const char *currentVideoMode = "get_current_video_mode";
	static const char *setVideoMode = "set_video_mode";

	static const char *windowScalingFactor = "get_window_scaling_factor";
}}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void LuaIGfxDevice::expose(lua_State *L)
{
	lua_createtable(L, 0, 25);

	LuaUtils::addFunction(L, LuaNames::IGfxDevice::setSwapInterval, setSwapInterval);

	LuaUtils::addFunction(L, LuaNames::IGfxDevice::isFullScreen, isFullScreen);
	LuaUtils::addFunction(L, LuaNames::IGfxDevice::setFullScreen, setFullScreen);

	LuaUtils::addFunction(L, LuaNames::IGfxDevice::isResizable, isResizable);

	LuaUtils::addFunction(L, LuaNames::IGfxDevice::windowPositionX, windowPositionX);
	LuaUtils::addFunction(L, LuaNames::IGfxDevice::windowPositionY, windowPositionY);
	LuaUtils::addFunction(L, LuaNames::IGfxDevice::windowPosition, windowPosition);
	LuaUtils::addFunction(L, LuaNames::IGfxDevice::setWindowPosition, setWindowPosition);

	LuaUtils::addFunction(L, LuaNames::IGfxDevice::width, width);
	LuaUtils::addFunction(L, LuaNames::IGfxDevice::height, height);
	LuaUtils::addFunction(L, LuaNames::IGfxDevice::resolution, resolution);
	LuaUtils::addFunction(L, LuaNames::IGfxDevice::aspect, aspect);
	LuaUtils::addFunction(L, LuaNames::IGfxDevice::setWindowSize, setWindowSize);

	LuaUtils::addFunction(L, LuaNames::IGfxDevice::drawableWidth, drawableWidth);
	LuaUtils::addFunction(L, LuaNames::IGfxDevice::drawableHeight, drawableHeight);
	LuaUtils::addFunction(L, LuaNames::IGfxDevice::drawableResolution, drawableResolution);

	LuaUtils::addFunction(L, LuaNames::IGfxDevice::setWindowTitle, setWindowTitle);
	LuaUtils::addFunction(L, LuaNames::IGfxDevice::setWindowIcon, setWindowIcon);
	LuaUtils::addFunction(L, LuaNames::IGfxDevice::flashWindow, flashWindow);

	LuaUtils::addFunction(L, LuaNames::IGfxDevice::numMonitors, numMonitors);
	LuaUtils::addFunction(L, LuaNames::IGfxDevice::primaryMonitorIndex, primaryMonitorIndex);
	LuaUtils::addFunction(L, LuaNames::IGfxDevice::windowMonitorIndex, windowMonitorIndex);
	LuaUtils::addFunction(L, LuaNames::IGfxDevice::monitor, monitor);

	LuaUtils::addFunction(L, LuaNames::IGfxDevice::currentVideoMode, currentVideoMode);
	LuaUtils::addFunction(L, LuaNames::IGfxDevice::setVideoMode, setVideoMode);

	LuaUtils::addFunction(L, LuaNames::IGfxDevice::windowScalingFactor, windowScalingFactor);

	lua_setfield(L, -2, LuaNames::IGfxDevice::IGfxDevice);
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

namespace {

	void pushVideoMode(lua_State *L, const IGfxDevice::VideoMode &videoMode)
	{
		lua_createtable(L, 0, 6);

		LuaUtils::pushField(L, LuaNames::VideoMode::width, videoMode.width);
		LuaUtils::pushField(L, LuaNames::VideoMode::height, videoMode.height);
		LuaUtils::pushField(L, LuaNames::VideoMode::refreshRate, videoMode.refreshRate);
		LuaUtils::pushField(L, LuaNames::VideoMode::redBits, videoMode.redBits);
		LuaUtils::pushField(L, LuaNames::VideoMode::greenBits, videoMode.greenBits);
		LuaUtils::pushField(L, LuaNames::VideoMode::blueBits, videoMode.blueBits);
	}

	void pushMonitor(lua_State *L, const IGfxDevice::Monitor &monitor)
	{
		lua_createtable(L, 0, 5);

		LuaUtils::pushField(L, LuaNames::Monitor::name, monitor.name);
		LuaVector2iUtils::pushField(L, LuaNames::Monitor::position, monitor.position);
		LuaVector2fUtils::pushField(L, LuaNames::Monitor::scale, monitor.scale);
		LuaUtils::pushField(L, LuaNames::Monitor::numVideoModes, monitor.numVideoModes);

		lua_createtable(L, monitor.numVideoModes, 0);

		for (unsigned int i = 0; i < monitor.numVideoModes; i++)
		{
			const IGfxDevice::VideoMode &videoMode = monitor.videoModes[i];
			pushVideoMode(L, videoMode);
			lua_rawseti(L, -2, i + 1); // Lua arrays start from index 1
		}
		lua_setfield(L, -2, LuaNames::Monitor::videoModes);
	}
}

int LuaIGfxDevice::setSwapInterval(lua_State *L)
{
	const int swapInterval = LuaUtils::retrieve<int>(L, -1);

	IGfxDevice &gfxDevice = theApplication().gfxDevice();
	gfxDevice.setSwapInterval(swapInterval);

	return 0;
}

int LuaIGfxDevice::isFullScreen(lua_State *L)
{
	const IGfxDevice &gfxDevice = theApplication().gfxDevice();
	LuaUtils::push(L, gfxDevice.isFullScreen());

	return 1;
}

int LuaIGfxDevice::setFullScreen(lua_State *L)
{
	const bool fullscreen = LuaUtils::retrieve<bool>(L, -1);

	IGfxDevice &gfxDevice = theApplication().gfxDevice();
	gfxDevice.setFullScreen(fullscreen);

	return 0;
}

int LuaIGfxDevice::isResizable(lua_State *L)
{
	const IGfxDevice &gfxDevice = theApplication().gfxDevice();
	LuaUtils::push(L, gfxDevice.isResizable());

	return 1;
}

int LuaIGfxDevice::windowPositionX(lua_State *L)
{
	const IGfxDevice &gfxDevice = theApplication().gfxDevice();
	LuaUtils::push(L, gfxDevice.windowPositionX());

	return 1;
}

int LuaIGfxDevice::windowPositionY(lua_State *L)
{
	const IGfxDevice &gfxDevice = theApplication().gfxDevice();
	LuaUtils::push(L, gfxDevice.windowPositionY());

	return 1;
}

int LuaIGfxDevice::windowPosition(lua_State *L)
{
	const IGfxDevice &gfxDevice = theApplication().gfxDevice();
	LuaVector2iUtils::push(L, gfxDevice.windowPosition());

	return 1;
}

int LuaIGfxDevice::setWindowPosition(lua_State *L)
{
	int vectorIndex = 0;
	const Vector2i &position = LuaVector2iUtils::retrieve(L, -1, vectorIndex);

	IGfxDevice &gfxDevice = theApplication().gfxDevice();
	gfxDevice.setWindowPosition(position);

	return 0;
}

int LuaIGfxDevice::width(lua_State *L)
{
	const IGfxDevice &gfxDevice = theApplication().gfxDevice();
	LuaUtils::push(L, gfxDevice.width());

	return 1;
}

int LuaIGfxDevice::height(lua_State *L)
{
	const IGfxDevice &gfxDevice = theApplication().gfxDevice();
	LuaUtils::push(L, gfxDevice.height());

	return 1;
}

int LuaIGfxDevice::resolution(lua_State *L)
{
	const IGfxDevice &gfxDevice = theApplication().gfxDevice();
	LuaVector2iUtils::push(L, gfxDevice.resolution());

	return 1;
}

int LuaIGfxDevice::aspect(lua_State *L)
{
	const IGfxDevice &gfxDevice = theApplication().gfxDevice();
	LuaUtils::push(L, gfxDevice.aspect());

	return 1;
}

int LuaIGfxDevice::setWindowSize(lua_State *L)
{
	int vectorIndex = 0;
	const Vector2i &windowSize = LuaVector2iUtils::retrieve(L, -1, vectorIndex);

	IGfxDevice &gfxDevice = theApplication().gfxDevice();
	gfxDevice.setWindowSize(windowSize);

	return 0;
}

int LuaIGfxDevice::drawableWidth(lua_State *L)
{
	const IGfxDevice &gfxDevice = theApplication().gfxDevice();
	LuaUtils::push(L, gfxDevice.drawableWidth());

	return 1;
}

int LuaIGfxDevice::drawableHeight(lua_State *L)
{
	const IGfxDevice &gfxDevice = theApplication().gfxDevice();
	LuaUtils::push(L, gfxDevice.drawableHeight());

	return 1;
}

int LuaIGfxDevice::drawableResolution(lua_State *L)
{
	const IGfxDevice &gfxDevice = theApplication().gfxDevice();
	LuaVector2iUtils::push(L, gfxDevice.drawableResolution());

	return 1;
}

int LuaIGfxDevice::setWindowTitle(lua_State *L)
{
	const char *windowTitle = LuaUtils::retrieve<const char *>(L, -1);

	IGfxDevice &gfxDevice = theApplication().gfxDevice();
	gfxDevice.setWindowTitle(windowTitle);

	return 0;
}

int LuaIGfxDevice::setWindowIcon(lua_State *L)
{
	const char *windowIconFilename = LuaUtils::retrieve<const char *>(L, -1);

	IGfxDevice &gfxDevice = theApplication().gfxDevice();
	gfxDevice.setWindowIcon(windowIconFilename);

	return 0;
}

int LuaIGfxDevice::flashWindow(lua_State *L)
{
	IGfxDevice &gfxDevice = theApplication().gfxDevice();
	gfxDevice.flashWindow();

	return 0;
}

int LuaIGfxDevice::numMonitors(lua_State *L)
{
	const IGfxDevice &gfxDevice = theApplication().gfxDevice();
	LuaUtils::push(L, gfxDevice.numMonitors());

	return 1;
}

int LuaIGfxDevice::primaryMonitorIndex(lua_State *L)
{
	const IGfxDevice &gfxDevice = theApplication().gfxDevice();
	LuaUtils::push(L, gfxDevice.primaryMonitorIndex());

	return 1;
}

int LuaIGfxDevice::windowMonitorIndex(lua_State *L)
{
	const IGfxDevice &gfxDevice = theApplication().gfxDevice();
	LuaUtils::push(L, gfxDevice.windowMonitorIndex());

	return 1;
}

int LuaIGfxDevice::monitor(lua_State *L)
{
	const int index = LuaUtils::retrieve<int>(L, -1);

	const IGfxDevice &gfxDevice = theApplication().gfxDevice();
	const IGfxDevice::Monitor &monitor = gfxDevice.monitor(index);

	pushMonitor(L, monitor);

	return 1;
}

int LuaIGfxDevice::currentVideoMode(lua_State *L)
{
	const IGfxDevice &gfxDevice = theApplication().gfxDevice();
	const IGfxDevice::VideoMode &currentVideoMode = gfxDevice.currentVideoMode();

	pushVideoMode(L, currentVideoMode);

	return 1;
}

int LuaIGfxDevice::setVideoMode(lua_State *L)
{
	const int index = LuaUtils::retrieve<int>(L, -1);

	IGfxDevice &gfxDevice = theApplication().gfxDevice();
	gfxDevice.setVideoMode(index);

	return 0;
}

int LuaIGfxDevice::windowScalingFactor(lua_State *L)
{
	const IGfxDevice &gfxDevice = theApplication().gfxDevice();
	LuaUtils::push(L, gfxDevice.windowScalingFactor());

	return 1;
}

}
