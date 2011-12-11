#if defined(__ANDROID__)
	#include <GLES/gl.h>
#elif !defined(NO_GLEW)
	#include <GL/glew.h>
#else
	#include <GL/gl.h>
#endif

#include <cstring> // for CheckGLExtension()
#include "ncServiceLocator.h"
#include "ncIGfxDevice.h"

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/// Log OpenGL device info
void ncIGfxDevice::LogGLInfo()
{
	ncServiceLocator::GetLogger().Write(ncILogger::LOG_INFO, "ncIGfxDevice::LogGLInfo - OpenGL device info ---");
	ncServiceLocator::GetLogger().Write(ncILogger::LOG_INFO, "Vendor: %s", glGetString(GL_VENDOR));
	ncServiceLocator::GetLogger().Write(ncILogger::LOG_INFO, "Renderer: %s", glGetString(GL_RENDERER));
	ncServiceLocator::GetLogger().Write(ncILogger::LOG_INFO, "OpenGL Version: %s", glGetString(GL_VERSION));
#ifndef __ANDROID__
	// Using OpenGL ES 1.1 at the moment
	ncServiceLocator::GetLogger().Write(ncILogger::LOG_INFO, "GLSL Version: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));
#endif
	ncServiceLocator::GetLogger().Write(ncILogger::LOG_INFO, "ncIGfxDevice::LogGLInfo - OpenGL device info ---");
}

/// Log OpenGL extensions
void ncIGfxDevice::LogGLExtensions()
{
	ncServiceLocator::GetLogger().Write(ncILogger::LOG_INFO, "ncIGfxDevice::LogGLExtensions - OpenGL extensions ---");
	ncServiceLocator::GetLogger().Write(ncILogger::LOG_INFO, "Extensions: %s", glGetString(GL_EXTENSIONS));
	ncServiceLocator::GetLogger().Write(ncILogger::LOG_INFO, "ncIGfxDevice::LogGLExtensions - OpenGL extensions ---");
}

/// Check for an OpenGL extension
bool ncIGfxDevice::CheckGLExtension(const char *pExtensionName)
{
	/*
	 ** Search for extName in the extensions string.  Use of strstr()
	 ** is not sufficient because extension names can be prefixes of
	 ** other extension names.  Could use strtok() but the constant
	 ** string returned by glGetString can be in read-only memory.
	 */
	char *p = (char *) glGetString(GL_EXTENSIONS);
	char *end;
	int extNameLen;

	extNameLen = strlen(pExtensionName);
	end = p + strlen(p);

	while (p < end) {
		int n = strcspn(p, " ");
		if ((extNameLen == n) && (strncmp(pExtensionName, p, n) == 0)) {
			return true;
		}
		p += (n + 1);
	}
	return false;
}
