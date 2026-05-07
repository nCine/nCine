#define NCINE_INCLUDE_LUA
#include "common_headers.h"

#include "LuaAppConfiguration.h"
#include "LuaUtils.h"
#include "LuaVector2Utils.h"
#include "AppConfiguration.h"

namespace ncine {

namespace LuaNames {
namespace AppConfiguration {
	static const char *WindowIgnorePosition = "window_ignore_position";

	// ----- Logging -----
	static const char *logging = "logging";
namespace Logging {
	static const char *file = "file";
	static const char *consoleLevel = "console_level";
	static const char *fileLevel = "file_level";
	static const char *frameTimerInterval = "frame_timer_interval";
	static const char *consoleColors = "console_colors";
} // Logging

	// ----- Window -----
	static const char *window = "window";
namespace Window {
	static const char *resolution = "resolution";
	static const char *refreshRate = "refresh_rate";
	static const char *position = "position";
	static const char *fullscreen = "fullscreen";
	static const char *resizable = "resizable";
	static const char *scaling = "scaling";

	static const char *title = "title";
	static const char *iconFilename = "icon";
} // Window

	// ----- Graphics -----
	static const char *graphics = "graphics";
namespace Graphics {
	static const char *vsync = "vsync";
	static const char *frameLimit = "frame_limit";

	static const char *openglCapabilities = "opengl_caps";
namespace OpenGLCapabilities {
	static const char *coreProfile = "core_profile";
	static const char *forwardCompatible = "forward_compatible";
	static const char *majorVersion = "major_version";
	static const char *minorVersion = "minor_version";
} // OpenGLCapabilities
	static const char *opengl = "opengl";
namespace OpenGL {
	static const char *debugContext = "debug_context";
	static const char *useBufferMapping = "buffer_mapping";

	static const char *vboSize = "vbo_size";
	static const char *iboSize = "ibo_size";
	static const char *vaoPoolSize = "vao_pool_size";
	static const char *renderCommandPoolSize = "rendercommand_pool_size";

	static const char *deferShaderQueries = "defer_shader_queries";
	static const char *fixedBatchSize = "fixed_batch_size";
	static const char *useBinaryShaderCache = "binary_shader_cache";
	static const char *shaderCacheDirname = "shader_cache_dirname";
	static const char *compileBatchedShadersTwice = "compile_batched_shaders_twice";
} // OpenGL
} // Graphics

	// ----- Audio -----
	static const char *audio = "audio";
namespace Audio {
	static const char *enabled = "enabled";
	static const char *frequency = "frequency";
	static const char *monoSources = "mono_sources";
	static const char *stereoSources = "stereo_sources";
} // Audio

	// ----- JobSystem -----
	static const char *jobSystem = "job_system";
namespace JobSystem {
	static const char *enabled = "enabled";
	static const char *numThreads = "num_threads";
} // JobSystem

	// ----- Features -----
	static const char *features = "features";
namespace Features {
	static const char *scenegraph = "scenegraph";
	static const char *debugOverlay = "debug_overlay";
} // Features

	static const char *profileTextUpdateTime = "profile_text_update_time";
	static const char *dataPath = "data_path";

	static const char *argc = "argc";
	static const char *argv = "argv";
}}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void LuaAppConfiguration::exposeConstants(lua_State *L)
{
	lua_pushinteger(L, AppConfiguration::Window::IgnorePosition);
	lua_setfield(L, -2, LuaNames::AppConfiguration::WindowIgnorePosition);
}

void LuaAppConfiguration::push(lua_State *L, const AppConfiguration &appCfg)
{
	lua_createtable(L, 0, 9 + appCfg.argc());

	// ----- Logging -----
	lua_createtable(L, 0, 5);

	LuaUtils::pushField(L, LuaNames::AppConfiguration::Logging::file, appCfg.logging.file.data());
	LuaUtils::pushField(L, LuaNames::AppConfiguration::Logging::consoleLevel, static_cast<int64_t>(appCfg.logging.consoleLevel));
	LuaUtils::pushField(L, LuaNames::AppConfiguration::Logging::fileLevel, static_cast<int64_t>(appCfg.logging.fileLevel));
	LuaUtils::pushField(L, LuaNames::AppConfiguration::Logging::frameTimerInterval, appCfg.logging.frameTimerInterval);
	LuaUtils::pushField(L, LuaNames::AppConfiguration::Logging::consoleColors, appCfg.logging.consoleColors);

	lua_setfield(L, -2, LuaNames::AppConfiguration::logging);

	// ----- Window -----
	lua_createtable(L, 0, 8);

	LuaVector2iUtils::pushField(L, LuaNames::AppConfiguration::Window::resolution, appCfg.window.resolution);
	LuaUtils::pushField(L, LuaNames::AppConfiguration::Window::refreshRate, appCfg.window.refreshRate);
	LuaVector2iUtils::pushField(L, LuaNames::AppConfiguration::Window::position, appCfg.window.position);
	LuaUtils::pushField(L, LuaNames::AppConfiguration::Window::fullscreen, appCfg.window.fullscreen);
	LuaUtils::pushField(L, LuaNames::AppConfiguration::Window::resizable, appCfg.window.resizable);
	LuaUtils::pushField(L, LuaNames::AppConfiguration::Window::scaling, appCfg.window.scaling);

	LuaUtils::pushField(L, LuaNames::AppConfiguration::Window::title, appCfg.window.title.data());
	LuaUtils::pushField(L, LuaNames::AppConfiguration::Window::iconFilename, appCfg.window.iconFilename.data());

	lua_setfield(L, -2, LuaNames::AppConfiguration::window);

	// ----- Graphics -----
	lua_createtable(L, 0, 4);

	LuaUtils::pushField(L, LuaNames::AppConfiguration::Graphics::vsync, appCfg.graphics.vsync);
	LuaUtils::pushField(L, LuaNames::AppConfiguration::Graphics::frameLimit, appCfg.graphics.frameLimit);

	// ----- Graphics.OpenGLCapabilities -----
	lua_createtable(L, 0, 4);

	LuaUtils::pushField(L, LuaNames::AppConfiguration::Graphics::OpenGLCapabilities::coreProfile, appCfg.graphics.openglCaps.coreProfile);
	LuaUtils::pushField(L, LuaNames::AppConfiguration::Graphics::OpenGLCapabilities::forwardCompatible, appCfg.graphics.openglCaps.forwardCompatible);
	LuaUtils::pushField(L, LuaNames::AppConfiguration::Graphics::OpenGLCapabilities::majorVersion, appCfg.graphics.openglCaps.majorVersion);
	LuaUtils::pushField(L, LuaNames::AppConfiguration::Graphics::OpenGLCapabilities::minorVersion, appCfg.graphics.openglCaps.minorVersion);

	lua_setfield(L, -2, LuaNames::AppConfiguration::Graphics::openglCapabilities);

	// ----- Graphics.OpenGL -----
	lua_createtable(L, 0, 11);

	LuaUtils::pushField(L, LuaNames::AppConfiguration::Graphics::OpenGL::debugContext, appCfg.graphics.opengl.debugContext);
	LuaUtils::pushField(L, LuaNames::AppConfiguration::Graphics::OpenGL::useBufferMapping, appCfg.graphics.opengl.useBufferMapping);

	LuaUtils::pushField(L, LuaNames::AppConfiguration::Graphics::OpenGL::vboSize, static_cast<int64_t>(appCfg.graphics.opengl.vboSize));
	LuaUtils::pushField(L, LuaNames::AppConfiguration::Graphics::OpenGL::iboSize, static_cast<int64_t>(appCfg.graphics.opengl.iboSize));
	LuaUtils::pushField(L, LuaNames::AppConfiguration::Graphics::OpenGL::vaoPoolSize, appCfg.graphics.opengl.vaoPoolSize);
	LuaUtils::pushField(L, LuaNames::AppConfiguration::Graphics::OpenGL::renderCommandPoolSize, appCfg.graphics.opengl.renderCommandPoolSize);

	LuaUtils::pushField(L, LuaNames::AppConfiguration::Graphics::OpenGL::deferShaderQueries, appCfg.graphics.opengl.deferShaderQueries);
	LuaUtils::pushField(L, LuaNames::AppConfiguration::Graphics::OpenGL::fixedBatchSize, appCfg.graphics.opengl.fixedBatchSize);
	LuaUtils::pushField(L, LuaNames::AppConfiguration::Graphics::OpenGL::useBinaryShaderCache, appCfg.graphics.opengl.useBinaryShaderCache);
	LuaUtils::pushField(L, LuaNames::AppConfiguration::Graphics::OpenGL::shaderCacheDirname, appCfg.graphics.opengl.shaderCacheDirname.data());
	LuaUtils::pushField(L, LuaNames::AppConfiguration::Graphics::OpenGL::compileBatchedShadersTwice, appCfg.graphics.opengl.compileBatchedShadersTwice);

	lua_setfield(L, -2, LuaNames::AppConfiguration::Graphics::opengl);

	lua_setfield(L, -2, LuaNames::AppConfiguration::graphics);

	// ----- Audio -----
	lua_createtable(L, 0, 4);

	LuaUtils::pushField(L, LuaNames::AppConfiguration::Audio::enabled, appCfg.audio.enabled);
	LuaUtils::pushField(L, LuaNames::AppConfiguration::Audio::frequency, appCfg.audio.frequency);
	LuaUtils::pushField(L, LuaNames::AppConfiguration::Audio::monoSources, appCfg.audio.monoSources);
	LuaUtils::pushField(L, LuaNames::AppConfiguration::Audio::stereoSources, appCfg.audio.stereoSources);

	lua_setfield(L, -2, LuaNames::AppConfiguration::audio);

	// ----- JobSystem -----
	lua_createtable(L, 0, 2);

	LuaUtils::pushField(L, LuaNames::AppConfiguration::JobSystem::enabled, appCfg.jobSystem.enabled);
	LuaUtils::pushField(L, LuaNames::AppConfiguration::JobSystem::numThreads, appCfg.jobSystem.numThreads);

	lua_setfield(L, -2, LuaNames::AppConfiguration::jobSystem);

	// ----- Features -----
	lua_createtable(L, 0, 2);

	LuaUtils::pushField(L, LuaNames::AppConfiguration::Features::scenegraph, appCfg.features.scenegraph);
	LuaUtils::pushField(L, LuaNames::AppConfiguration::Features::debugOverlay, appCfg.features.debugOverlay);

	lua_setfield(L, -2, LuaNames::AppConfiguration::features);

	// -----
	LuaUtils::pushField(L, LuaNames::AppConfiguration::profileTextUpdateTime, appCfg.profileTextUpdateTime);
	LuaUtils::pushField(L, LuaNames::AppConfiguration::dataPath, appCfg.dataPath().data());
	LuaUtils::pushField(L, LuaNames::AppConfiguration::argc, appCfg.argc());

	if (appCfg.argc() > 0)
	{
		lua_createtable(L, appCfg.argc(), 0);
		for (int i = 0; i < appCfg.argc(); i++)
		{
			lua_pushstring(L, appCfg.argv(i));
			lua_rawseti(L, -2, i + 1); // Lua arrays start from index 1
		}
		lua_setfield(L, -2, LuaNames::AppConfiguration::argv);
	}
}

void LuaAppConfiguration::retrieveAndSet(lua_State *L, AppConfiguration &appCfg)
{
	if (lua_istable(L, -1) == false)
	{
		LOGW("Expecting a table at index -1");
		return;
	}

	// ----- Logging -----
	{
		LuaUtils::retrieveFieldTable(L, -1, LuaNames::AppConfiguration::logging);

		const char *file = LuaUtils::retrieveField<const char *>(L, -1, LuaNames::AppConfiguration::Logging::file);
		appCfg.logging.file = file;
		const ILogger::LogLevel consoleLevel = static_cast<ILogger::LogLevel>(LuaUtils::retrieveField<int64_t>(L, -1, LuaNames::AppConfiguration::Logging::consoleLevel));
		appCfg.logging.consoleLevel = consoleLevel;
		const ILogger::LogLevel fileLevel = static_cast<ILogger::LogLevel>(LuaUtils::retrieveField<int64_t>(L, -1, LuaNames::AppConfiguration::Logging::fileLevel));
		appCfg.logging.fileLevel = fileLevel;
		const float frameTimerInterval = LuaUtils::retrieveField<float>(L, -1, LuaNames::AppConfiguration::Logging::frameTimerInterval);
		appCfg.logging.frameTimerInterval = frameTimerInterval;
		const bool consoleColors = LuaUtils::retrieveField<bool>(L, -1, LuaNames::AppConfiguration::Logging::consoleColors);
		appCfg.logging.consoleColors = consoleColors;

		lua_pop(L, 1);
	}

	// ----- Window -----
	{
		LuaUtils::retrieveFieldTable(L, -1, LuaNames::AppConfiguration::window);

		const Vector2i resolution = LuaVector2iUtils::retrieveTableField(L, -1, LuaNames::AppConfiguration::Window::resolution);
		appCfg.window.resolution = resolution;
		const float refreshRate = LuaUtils::retrieveField<float>(L, -1, LuaNames::AppConfiguration::Window::refreshRate);
		appCfg.window.refreshRate = refreshRate;
		const Vector2i position = LuaVector2iUtils::retrieveTableField(L, -1, LuaNames::AppConfiguration::Window::position);
		appCfg.window.position = position;
		const bool fullscreen = LuaUtils::retrieveField<bool>(L, -1, LuaNames::AppConfiguration::Window::fullscreen);
		appCfg.window.fullscreen = fullscreen;
		const bool resizable = LuaUtils::retrieveField<bool>(L, -1, LuaNames::AppConfiguration::Window::resizable);
		appCfg.window.resizable = resizable;
		const bool scaling = LuaUtils::retrieveField<bool>(L, -1, LuaNames::AppConfiguration::Window::scaling);
		appCfg.window.scaling = scaling;

		const char *title = LuaUtils::retrieveField<const char *>(L, -1, LuaNames::AppConfiguration::Window::title);
		appCfg.window.title = title;
		const char *iconFilename = LuaUtils::retrieveField<const char *>(L, -1, LuaNames::AppConfiguration::Window::iconFilename);
		appCfg.window.iconFilename = iconFilename;

		lua_pop(L, 1);
	}

	// ----- Graphics -----
	{
		LuaUtils::retrieveFieldTable(L, -1, LuaNames::AppConfiguration::graphics);

		const bool vsync = LuaUtils::retrieveField<bool>(L, -1, LuaNames::AppConfiguration::Graphics::vsync);
		appCfg.graphics.vsync = vsync;
		const unsigned int frameLimit = LuaUtils::retrieveField<uint32_t>(L, -1, LuaNames::AppConfiguration::Graphics::frameLimit);
		appCfg.graphics.frameLimit = frameLimit;

		// ----- OpenGL -----
		{
			LuaUtils::retrieveFieldTable(L, -1, LuaNames::AppConfiguration::Graphics::opengl);

			const bool debugContext = LuaUtils::retrieveField<bool>(L, -1, LuaNames::AppConfiguration::Graphics::OpenGL::debugContext);
			appCfg.graphics.opengl.debugContext = debugContext;
			const bool useBufferMapping = LuaUtils::retrieveField<bool>(L, -1, LuaNames::AppConfiguration::Graphics::OpenGL::useBufferMapping);
			appCfg.graphics.opengl.useBufferMapping = useBufferMapping;

			const unsigned long vboSize = LuaUtils::retrieveField<uint64_t>(L, -1, LuaNames::AppConfiguration::Graphics::OpenGL::vboSize);
			appCfg.graphics.opengl.vboSize = vboSize;
			const unsigned long iboSize = LuaUtils::retrieveField<uint64_t>(L, -1, LuaNames::AppConfiguration::Graphics::OpenGL::iboSize);
			appCfg.graphics.opengl.iboSize = iboSize;
			const unsigned int vaoPoolSize = LuaUtils::retrieveField<uint32_t>(L, -1, LuaNames::AppConfiguration::Graphics::OpenGL::vaoPoolSize);
			appCfg.graphics.opengl.vaoPoolSize = vaoPoolSize;
			const unsigned int renderCommandPoolSize = LuaUtils::retrieveField<uint32_t>(L, -1, LuaNames::AppConfiguration::Graphics::OpenGL::renderCommandPoolSize);
			appCfg.graphics.opengl.renderCommandPoolSize = renderCommandPoolSize;

			const bool deferShaderQueries = LuaUtils::retrieveField<bool>(L, -1, LuaNames::AppConfiguration::Graphics::OpenGL::deferShaderQueries);
			appCfg.graphics.opengl.deferShaderQueries = deferShaderQueries;
			const unsigned int fixedBatchSize = LuaUtils::retrieveField<uint32_t>(L, -1, LuaNames::AppConfiguration::Graphics::OpenGL::fixedBatchSize);
			appCfg.graphics.opengl.fixedBatchSize = fixedBatchSize;
			const bool useBinaryShaderCache = LuaUtils::retrieveField<bool>(L, -1, LuaNames::AppConfiguration::Graphics::OpenGL::useBinaryShaderCache);
			appCfg.graphics.opengl.useBinaryShaderCache = useBinaryShaderCache;
			const char *shaderCacheDirname = LuaUtils::retrieveField<const char *>(L, -1, LuaNames::AppConfiguration::Graphics::OpenGL::shaderCacheDirname);
			appCfg.graphics.opengl.shaderCacheDirname = shaderCacheDirname;
			const bool compileBatchedShadersTwice = LuaUtils::retrieveField<bool>(L, -1, LuaNames::AppConfiguration::Graphics::OpenGL::compileBatchedShadersTwice);
			appCfg.graphics.opengl.compileBatchedShadersTwice = compileBatchedShadersTwice;

			lua_pop(L, 1);
		}

		lua_pop(L, 1);
	}

	// ----- Audio -----
	{
		LuaUtils::retrieveFieldTable(L, -1, LuaNames::AppConfiguration::audio);

		const bool enabled = LuaUtils::retrieveField<bool>(L, -1, LuaNames::AppConfiguration::Audio::enabled);
		appCfg.audio.enabled = enabled;
		const unsigned int frequency = LuaUtils::retrieveField<uint32_t>(L, -1, LuaNames::AppConfiguration::Audio::frequency);
		appCfg.audio.frequency = frequency;
		const unsigned int monoSources = LuaUtils::retrieveField<uint32_t>(L, -1, LuaNames::AppConfiguration::Audio::monoSources);
		appCfg.audio.monoSources = monoSources;
		const unsigned int stereoSources = LuaUtils::retrieveField<uint32_t>(L, -1, LuaNames::AppConfiguration::Audio::stereoSources);
		appCfg.audio.stereoSources = stereoSources;

		lua_pop(L, 1);
	}

	// ----- JobSystem -----
	{
		LuaUtils::retrieveFieldTable(L, -1, LuaNames::AppConfiguration::jobSystem);

		const bool enabled = LuaUtils::retrieveField<bool>(L, -1, LuaNames::AppConfiguration::JobSystem::enabled);
		appCfg.jobSystem.enabled = enabled;
		const unsigned int numThreads = LuaUtils::retrieveField<uint32_t>(L, -1, LuaNames::AppConfiguration::JobSystem::numThreads);
		appCfg.jobSystem.numThreads = numThreads;

		lua_pop(L, 1);
	}

	// ----- Features -----
	{
		LuaUtils::retrieveFieldTable(L, -1, LuaNames::AppConfiguration::features);

		const bool scenegraph = LuaUtils::retrieveField<bool>(L, -1, LuaNames::AppConfiguration::Features::scenegraph);
		appCfg.features.scenegraph = scenegraph;
		const bool debugOverlay = LuaUtils::retrieveField<bool>(L, -1, LuaNames::AppConfiguration::Features::debugOverlay);
		appCfg.features.debugOverlay = debugOverlay;

		lua_pop(L, 1);
	}

	const char *dataPath = LuaUtils::retrieveField<const char *>(L, -1, LuaNames::AppConfiguration::dataPath);
	appCfg.dataPath() = dataPath;
}

}
