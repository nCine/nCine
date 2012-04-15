#if defined(__ANDROID__)
	#include <GLES/gl.h>
	#include <GLES/glext.h>
#elif defined(WITH_GLEW)
	#include <GL/glew.h>
#else
	#include <GL/gl.h>
	#include <GL/glext.h>
#endif

#include <cstring> // for CheckGLExtension()
#include "ncGfxCapabilities.h"
#include "ncServiceLocator.h"

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ncGfxCapabilities::ncGfxCapabilities()
	: m_iGLMajor(-1),
	  m_iGLMinor(-1),
#ifndef __ANDROID__
	  m_iGLRelease(-1),
#endif
	  m_iMaxTextureSize(-1),
	  m_iMaxTextureUnits(-1),
#ifndef __ANDROID__
	  m_bEXTTextureCompressionS3TC (false)
#else
	  m_bOESCompressedETC1RGB8Texture(false),
	  m_bAMDCompressedATCTexture(false),
	  m_bIMGTextureCompressionPVRTC(false)
#endif
{

}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/// Logs OpenGL device info
void ncGfxCapabilities::LogGLInfo()
{
	ncServiceLocator::Logger().Write(ncILogger::LOG_INFO, "ncGfxCapabilities::LogGLInfo - OpenGL device info ---");
	ncServiceLocator::Logger().Write(ncILogger::LOG_INFO, "Vendor: %s", glGetString(GL_VENDOR));
	ncServiceLocator::Logger().Write(ncILogger::LOG_INFO, "Renderer: %s", glGetString(GL_RENDERER));
	ncServiceLocator::Logger().Write(ncILogger::LOG_INFO, "OpenGL Version: %s", glGetString(GL_VERSION));
#ifndef __ANDROID__
	// Using OpenGL ES 1.1 at the moment
	ncServiceLocator::Logger().Write(ncILogger::LOG_INFO, "GLSL Version: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));
#endif
	ncServiceLocator::Logger().Write(ncILogger::LOG_INFO, "ncGfxCapabilities::LogGLInfo - OpenGL device info ---");
}

/// Logs OpenGL extensions
void ncGfxCapabilities::LogGLExtensions()
{
	ncServiceLocator::Logger().Write(ncILogger::LOG_INFO, "ncGfxCapabilities::LogGLExtensions - OpenGL extensions ---");
	ncServiceLocator::Logger().Write(ncILogger::LOG_INFO, "Extensions: %s", glGetString(GL_EXTENSIONS));
	ncServiceLocator::Logger().Write(ncILogger::LOG_INFO, "ncGfxCapabilities::LogGLExtensions - OpenGL extensions ---");
}

/// Logs OpenGL device capabilites
void ncGfxCapabilities::LogGLCaps() const
{
	ncServiceLocator::Logger().Write(ncILogger::LOG_INFO, "ncGfxCapabilities::LogGLCaps - OpenGL device capabilities ---");
	ncServiceLocator::Logger().Write(ncILogger::LOG_INFO, "OpenGL Major: %d", m_iGLMajor);
	ncServiceLocator::Logger().Write(ncILogger::LOG_INFO, "OpenGL Minor: %d", m_iGLMinor);
#ifndef __ANDROID__
	ncServiceLocator::Logger().Write(ncILogger::LOG_INFO, "OpenGL Release: %d", m_iGLRelease);
#endif
	ncServiceLocator::Logger().Write(ncILogger::LOG_INFO, "ncGfxCapabilities::LogGLCaps - ---");
	ncServiceLocator::Logger().Write(ncILogger::LOG_INFO, "GL_MAX_TEXTURE_SIZE: %d", m_iMaxTextureSize);
	ncServiceLocator::Logger().Write(ncILogger::LOG_INFO, "GL_MAX_TEXTURE_UNITS: %d", m_iMaxTextureUnits);
	ncServiceLocator::Logger().Write(ncILogger::LOG_INFO, "ncGfxCapabilities::LogGLCaps - ---");
#ifndef __ANDROID__
	ncServiceLocator::Logger().Write(ncILogger::LOG_INFO, "GL_EXT_texture_compression_s3tc: %d", m_bEXTTextureCompressionS3TC);
#else
	ncServiceLocator::Logger().Write(ncILogger::LOG_INFO, "GL_OES_compressed_ETC1_RGB8_texture: %d", m_bOESCompressedETC1RGB8Texture);
	ncServiceLocator::Logger().Write(ncILogger::LOG_INFO, "GL_AMD_compressed_ATC_texture: %d", m_bAMDCompressedATCTexture);
	ncServiceLocator::Logger().Write(ncILogger::LOG_INFO, "GL_IMG_texture_compression_pvrtc: %d", m_bIMGTextureCompressionPVRTC);
#endif
	ncServiceLocator::Logger().Write(ncILogger::LOG_INFO, "ncGfxCapabilities::LogGLCaps - OpenGL device capabilities ---");
}

/// Check for an OpenGL extension
bool ncGfxCapabilities::CheckGLExtension(const char *pExtensionName) const
{
	/*
	 ** Search for extName in the extensions string.  Use of strstr()
	 ** is not sufficient because extension names can be prefixes of
	 ** other extension names.  Could use strtok() but the constant
	 ** string returned by glGetString can be in read-only memory.
	 */
	char *pExtensions = (char *)glGetString(GL_EXTENSIONS);
	char *pEnd;

	int iNameLength = strlen(pExtensionName);
	pEnd = pExtensions + strlen(pExtensions);

	while (pExtensions < pEnd) {
		int n = strcspn(pExtensions, " ");
		if ((iNameLength == n) && (strncmp(pExtensionName, pExtensions, n) == 0)) {
			return true;
		}
		pExtensions += (n + 1);
	}
	return false;
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

/// Queries the device about its capabilities
void ncGfxCapabilities::Init()
{
	char *pVersion = (char *)glGetString(GL_VERSION);
#ifndef __ANDROID__
	sscanf(pVersion, "%2d.%2d.%2d", &m_iGLMajor, &m_iGLMinor, &m_iGLRelease);
#else
	sscanf(pVersion, "OpenGL ES-%*2s %2d.%2d", &m_iGLMajor, &m_iGLMinor);
#endif

	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &m_iMaxTextureSize);
	glGetIntegerv(GL_MAX_TEXTURE_UNITS, &m_iMaxTextureUnits);

#ifndef __ANDROID__
	m_bEXTTextureCompressionS3TC = CheckGLExtension("GL_EXT_texture_compression_s3tc");
#else
	m_bOESCompressedETC1RGB8Texture = CheckGLExtension("GL_OES_compressed_ETC1_RGB8_texture");
	m_bAMDCompressedATCTexture = CheckGLExtension("GL_AMD_compressed_ATC_texture");
	m_bIMGTextureCompressionPVRTC = CheckGLExtension("GL_IMG_texture_compression_pvrtc");
#endif
}


