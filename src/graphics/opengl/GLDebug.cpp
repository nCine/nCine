#include "common_macros.h"
#include "GLDebug.h"
#include "IGfxCapabilities.h"
#include "Application.h"

#if !defined(__ANDROID__) && defined(WITH_OPENGLES) && defined(__linux__)
	#include <GLES3/gl32.h>
#endif

#if ((defined(__ANDROID__) && __ANDROID_API__ >= 21) || (!defined(__ANDROID__) && defined(WITH_OPENGLES))) && !defined(__APPLE__) && !defined(__EMSCRIPTEN__) && (GL_ES_VERSION_3_0 && !GL_ES_VERSION_3_2)
	#define glPushDebugGroup glPushDebugGroupKHR
	#define glPopDebugGroup glPopDebugGroupKHR
	#define glDebugMessageInsert glDebugMessageInsertKHR
	#define glObjectLabel glObjectLabelKHR
	#define glGetObjectLabel glGetObjectLabelKHR
	#define GL_MAX_LABEL_LENGTH GL_MAX_LABEL_LENGTH_KHR

	#define glDebugMessageCallback glDebugMessageCallbackKHR
	#define GLDEBUGPROC GLDEBUGPROCKHR
	#define GL_DEBUG_OUTPUT_SYNCHRONOUS GL_DEBUG_OUTPUT_SYNCHRONOUS_KHR

	#define GL_DEBUG_SOURCE_API GL_DEBUG_SOURCE_API_KHR
	#define GL_DEBUG_SOURCE_WINDOW_SYSTEM GL_DEBUG_SOURCE_WINDOW_SYSTEM_KHR
	#define GL_DEBUG_SOURCE_SHADER_COMPILER GL_DEBUG_SOURCE_SHADER_COMPILER_KHR
	#define GL_DEBUG_SOURCE_THIRD_PARTY GL_DEBUG_SOURCE_THIRD_PARTY_KHR
	#define GL_DEBUG_SOURCE_APPLICATION GL_DEBUG_SOURCE_APPLICATION_KHR
	#define GL_DEBUG_SOURCE_OTHER GL_DEBUG_SOURCE_OTHER_KHR

	#define GL_DEBUG_TYPE_ERROR GL_DEBUG_TYPE_ERROR_KHR
	#define GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_KHR
	#define GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_KHR
	#define GL_DEBUG_TYPE_PORTABILITY GL_DEBUG_TYPE_PORTABILITY_KHR
	#define GL_DEBUG_TYPE_PERFORMANCE GL_DEBUG_TYPE_PERFORMANCE_KHR
	#define GL_DEBUG_TYPE_MARKER GL_DEBUG_TYPE_MARKER_KHR
	#define GL_DEBUG_TYPE_PUSH_GROUP GL_DEBUG_TYPE_PUSH_GROUP_KHR
	#define GL_DEBUG_TYPE_POP_GROUP GL_DEBUG_TYPE_POP_GROUP_KHR
	#define GL_DEBUG_TYPE_OTHER GL_DEBUG_TYPE_OTHER_KHR

	#define GL_DEBUG_SEVERITY_NOTIFICATION GL_DEBUG_SEVERITY_NOTIFICATION_KHR
	#define GL_DEBUG_SEVERITY_LOW GL_DEBUG_SEVERITY_LOW_KHR
	#define GL_DEBUG_SEVERITY_MEDIUM GL_DEBUG_SEVERITY_MEDIUM_KHR
	#define GL_DEBUG_SEVERITY_HIGH GL_DEBUG_SEVERITY_HIGH_KHR
#endif

namespace ncine {

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

bool GLDebug::debugAvailable_ = false;
GLuint GLDebug::debugGroupId_ = 0;
int GLDebug::maxLabelLength_ = 0;

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void GLDebug::init(const IGfxCapabilities &gfxCaps)
{
#if (!defined(__ANDROID__) && !defined(__APPLE__) && !defined(__EMSCRIPTEN__)) || (GL_ES_VERSION_3_0 && __ANDROID_API__ >= 21)
	debugAvailable_ = gfxCaps.hasExtension(IGfxCapabilities::GLExtensions::KHR_DEBUG) &&
	                  theApplication().gfxDevice().glContextInfo().debugContext;

	glGetIntegerv(GL_MAX_LABEL_LENGTH, &maxLabelLength_);

	if (debugAvailable_)
		enableDebugOutput();
#endif
}

void GLDebug::pushGroup(const char *message)
{
#if (!defined(__ANDROID__) && !defined(__APPLE__) && !defined(__EMSCRIPTEN__)) || (GL_ES_VERSION_3_0 && __ANDROID_API__ >= 21)
	if (debugAvailable_)
		glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, debugGroupId_++, -1, message);
#endif
}

void GLDebug::popGroup()
{
#if (!defined(__ANDROID__) && !defined(__APPLE__) && !defined(__EMSCRIPTEN__)) || (GL_ES_VERSION_3_0 && __ANDROID_API__ >= 21)
	if (debugAvailable_)
		glPopDebugGroup();
#endif
}

void GLDebug::messageInsert(const char *message)
{
#if (!defined(__ANDROID__) && !defined(__APPLE__) && !defined(__EMSCRIPTEN__)) || (GL_ES_VERSION_3_0 && __ANDROID_API__ >= 21)
	if (debugAvailable_)
		glDebugMessageInsert(GL_DEBUG_SOURCE_APPLICATION, GL_DEBUG_TYPE_MARKER, debugGroupId_++, GL_DEBUG_SEVERITY_NOTIFICATION, -1, message);
#endif
}

void GLDebug::objectLabel(LabelTypes identifier, GLuint name, const char *label)
{
#if (!defined(__ANDROID__) && !defined(__APPLE__) && !defined(__EMSCRIPTEN__)) || (GL_ES_VERSION_3_0 && __ANDROID_API__ >= 21)
	if (debugAvailable_)
		glObjectLabel(static_cast<GLenum>(identifier), name, -1, label);
#endif
}

void GLDebug::objectLabel(LabelTypes identifier, GLuint name, GLsizei length, const char *label)
{
#if (!defined(__ANDROID__) && !defined(__APPLE__) && !defined(__EMSCRIPTEN__)) || (GL_ES_VERSION_3_0 && __ANDROID_API__ >= 21)
	if (debugAvailable_)
		glObjectLabel(static_cast<GLenum>(identifier), name, length, label);
#endif
}

void GLDebug::getObjectLabel(LabelTypes identifier, GLuint name, GLsizei bufSize, GLsizei *length, char *label)
{
#if (!defined(__ANDROID__) && !defined(__APPLE__) && !defined(__EMSCRIPTEN__)) || (GL_ES_VERSION_3_0 && __ANDROID_API__ >= 21)
	if (debugAvailable_)
		glGetObjectLabel(static_cast<GLenum>(identifier), name, bufSize, length, label);
#endif
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

#if ((defined(__ANDROID__) && __ANDROID_API__ >= 21) || (!defined(__ANDROID__) && defined(WITH_OPENGLES))) && !defined(__APPLE__) && !defined(__EMSCRIPTEN__) && GL_ES_VERSION_3_0

/// Callback for `glDebugMessageCallback()`
void debugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const char *message, const void *userParam)
{
	const char *sourceString = "Unknown";
	switch (source)
	{
		case GL_DEBUG_SOURCE_API: sourceString = "API"; break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM: sourceString = "Window system"; break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER: sourceString = "Shader compiler"; break;
		case GL_DEBUG_SOURCE_THIRD_PARTY: sourceString = "Third party"; break;
		case GL_DEBUG_SOURCE_APPLICATION: sourceString = "Application"; break;
		case GL_DEBUG_SOURCE_OTHER: sourceString = "Other"; break;
		default: sourceString = "Unknown"; break;
	}

	const char *typeString = "Unknown";
	switch (type)
	{
		case GL_DEBUG_TYPE_ERROR: typeString = "Error"; break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: typeString = "Deprecated behavior"; break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: typeString = "Undefined behavior"; break;
		case GL_DEBUG_TYPE_PORTABILITY: typeString = "Portability"; break;
		case GL_DEBUG_TYPE_PERFORMANCE: typeString = "Performance"; break;
		case GL_DEBUG_TYPE_MARKER: typeString = "Marker"; break;
		case GL_DEBUG_TYPE_PUSH_GROUP: typeString = "Push group"; break;
		case GL_DEBUG_TYPE_POP_GROUP: typeString = "Pop group"; break;
		case GL_DEBUG_TYPE_OTHER: typeString = "Other"; break;
		default: typeString = "Unknown"; break;
	}

	const char *severityString = "unknown";
	switch (severity)
	{
		case GL_DEBUG_SEVERITY_NOTIFICATION: severityString = "notification"; break;
		case GL_DEBUG_SEVERITY_LOW: severityString = "low"; break;
		case GL_DEBUG_SEVERITY_MEDIUM: severityString = "medium"; break;
		case GL_DEBUG_SEVERITY_HIGH: severityString = "high"; break;
		default: severityString = "unknown"; break;
	}

	LOGD_X("OpenGL message %u of type \"%s\" from source \"%s\" with %s severity: \"%s\"", id, typeString, sourceString, severityString, message);
}
#endif

void GLDebug::enableDebugOutput()
{
#if ((defined(__ANDROID__) && __ANDROID_API__ >= 21) || (!defined(__ANDROID__) && defined(WITH_OPENGLES))) && !defined(__APPLE__) && !defined(__EMSCRIPTEN__) && GL_ES_VERSION_3_0
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(static_cast<GLDEBUGPROC>(debugCallback), nullptr);

	LOGI("OpenGL debug callback set");
#endif
}

}
