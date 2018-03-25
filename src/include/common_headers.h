#if defined(NCINE_INCLUDE_OPENGL)
	#if defined(__ANDROID__)
		#define GL_GLEXT_PROTOTYPES
		#include <GLES3/gl3.h>
		#include <GLES2/gl2ext.h>
	#elif defined(WITH_GLEW)
		#include <GL/glew.h>
	#elif defined(__APPLE__)
		#include <OpenGL/gl.h>
		#include <OpenGL/glext.h>
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
