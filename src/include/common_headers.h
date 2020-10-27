#if defined(NCINE_INCLUDE_OPENGL)
	#if defined(__ANDROID__) || defined(WITH_ANGLE)
		#define GL_GLEXT_PROTOTYPES
		#include <GLES3/gl3.h>
		#include <GLES2/gl2ext.h>
	#elif defined(WITH_GLEW)
		#define GLEW_NO_GLU
		#include <GL/glew.h>
	#elif defined(__APPLE__)
		#define GL_SILENCE_DEPRECATION
		#include <OpenGL/gl3.h>
		#include <OpenGL/gl3ext.h>
	#else
		#define GL_GLEXT_PROTOTYPES
		#include <GL/gl.h>
		#include <GL/glext.h>
	#endif
#endif

#if defined(NCINE_INCLUDE_OPENAL)
	#ifdef __APPLE__
		#include <OpenAL/al.h>
	#else
		#include <AL/al.h>
	#endif
#endif

#if defined(NCINE_INCLUDE_OPENALC)
	#ifdef __APPLE__
		#include <OpenAL/alc.h>
		#include <OpenAL/al.h>
	#else
		#include <AL/alc.h>
		#include <AL/al.h>
	#endif
#endif

#if defined(NCINE_INCLUDE_LUA)
extern "C"
{
	#include "lauxlib.h"
	#include "lua.h"
	#include "lualib.h"
}
#endif
