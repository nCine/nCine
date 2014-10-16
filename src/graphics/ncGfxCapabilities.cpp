#if defined(__ANDROID__)
	#include <GLES/gl.h>
	#include <GLES/glext.h>
#elif defined(WITH_GLEW)
	#include <GL/glew.h>
#elif defined(__APPLE__)
	#include <OpenGL/gl.h>
	#include <OpenGL/glext.h>
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
	: majorGL_(-1),
	  minorGL_(-1),
#ifndef __ANDROID__
	  releaseGL_(-1),
#endif
	  maxTextureSize_(-1),
	  maxTextureUnits_(-1),
#ifndef __ANDROID__
	  extTextureCompressionS3TC_(false)
#else
	  oesCompressedETC1RGB8Texture_(false),
	  amdCompressedATCTexture_(false),
	  imgTextureCompressionPVRTC_(false)
#endif
{

}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/// Logs OpenGL device info
void ncGfxCapabilities::logGLInfo()
{
	ncServiceLocator::logger().write(ncILogger::LOG_INFO, "ncGfxCapabilities::logGLInfo - OpenGL device info ---");
	ncServiceLocator::logger().write(ncILogger::LOG_INFO, "Vendor: %s", glGetString(GL_VENDOR));
	ncServiceLocator::logger().write(ncILogger::LOG_INFO, "Renderer: %s", glGetString(GL_RENDERER));
	ncServiceLocator::logger().write(ncILogger::LOG_INFO, "OpenGL Version: %s", glGetString(GL_VERSION));
#ifndef __ANDROID__
	// Using OpenGL ES 1.1 at the moment
	ncServiceLocator::logger().write(ncILogger::LOG_INFO, "GLSL Version: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));
#endif
	ncServiceLocator::logger().write(ncILogger::LOG_INFO, "ncGfxCapabilities::logGLInfo - OpenGL device info ---");
}

/// Logs OpenGL extensions
void ncGfxCapabilities::logGLExtensions()
{
	ncServiceLocator::logger().write(ncILogger::LOG_INFO, "ncGfxCapabilities::logGLExtensions - OpenGL extensions ---");
	ncServiceLocator::logger().write(ncILogger::LOG_INFO, "Extensions: %s", glGetString(GL_EXTENSIONS));
	ncServiceLocator::logger().write(ncILogger::LOG_INFO, "ncGfxCapabilities::logGLExtensions - OpenGL extensions ---");
}

/// Logs OpenGL device capabilites
void ncGfxCapabilities::logGLCaps() const
{
	ncServiceLocator::logger().write(ncILogger::LOG_INFO, "ncGfxCapabilities::logGLCaps - OpenGL device capabilities ---");
	ncServiceLocator::logger().write(ncILogger::LOG_INFO, "OpenGL Major: %d", majorGL_);
	ncServiceLocator::logger().write(ncILogger::LOG_INFO, "OpenGL Minor: %d", minorGL_);
#ifndef __ANDROID__
	ncServiceLocator::logger().write(ncILogger::LOG_INFO, "OpenGL Release: %d", releaseGL_);
#endif
	ncServiceLocator::logger().write(ncILogger::LOG_INFO, "ncGfxCapabilities::logGLCaps - ---");
	ncServiceLocator::logger().write(ncILogger::LOG_INFO, "GL_MAX_TEXTURE_SIZE: %d", maxTextureSize_);
	ncServiceLocator::logger().write(ncILogger::LOG_INFO, "GL_MAX_TEXTURE_UNITS: %d", maxTextureUnits_);
	ncServiceLocator::logger().write(ncILogger::LOG_INFO, "ncGfxCapabilities::logGLCaps - ---");
#ifndef __ANDROID__
	ncServiceLocator::logger().write(ncILogger::LOG_INFO, "GL_EXT_texture_compression_s3tc: %d", extTextureCompressionS3TC_);
#else
	ncServiceLocator::logger().write(ncILogger::LOG_INFO, "GL_OES_compressed_ETC1_RGB8_texture: %d", oesCompressedETC1RGB8Texture_);
	ncServiceLocator::logger().write(ncILogger::LOG_INFO, "GL_AMD_compressed_ATC_texture: %d", amdCompressedATCTexture_);
	ncServiceLocator::logger().write(ncILogger::LOG_INFO, "GL_IMG_texture_compression_pvrtc: %d", imgTextureCompressionPVRTC_);
#endif
	ncServiceLocator::logger().write(ncILogger::LOG_INFO, "ncGfxCapabilities::logGLCaps - OpenGL device capabilities ---");
}

/// Check for an OpenGL extension
bool ncGfxCapabilities::checkGLExtension(const char *extensionName) const
{
	/*
	 ** Search for extName in the extensions string.  Use of strstr()
	 ** is not sufficient because extension names can be prefixes of
	 ** other extension names.  Could use strtok() but the constant
	 ** string returned by glGetString can be in read-only memory.
	 */
	char *extensions = (char *)glGetString(GL_EXTENSIONS);
	char *end;

	int nameLength = strlen(extensionName);
	end = extensions + strlen(extensions);

	while (extensions < end)
	{
		int n = strcspn(extensions, " ");
		if ((nameLength == n) && (strncmp(extensionName, extensions, n) == 0))
		{
			return true;
		}
		extensions += (n + 1);
	}
	return false;
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

/// Queries the device about its capabilities
void ncGfxCapabilities::init()
{
	char *pVersion = (char *)glGetString(GL_VERSION);
#ifndef __ANDROID__
	sscanf(pVersion, "%2d.%2d.%2d", &majorGL_, &minorGL_, &releaseGL_);
#else
	sscanf(pVersion, "OpenGL ES-%*2s %2d.%2d", &majorGL_, &minorGL_);
#endif

	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize_);
	glGetIntegerv(GL_MAX_TEXTURE_UNITS, &maxTextureUnits_);

#ifndef __ANDROID__
	extTextureCompressionS3TC_ = checkGLExtension("GL_EXT_texture_compression_s3tc");
#else
	oesCompressedETC1RGB8Texture_ = checkGLExtension("GL_OES_compressed_ETC1_RGB8_texture");
	amdCompressedATCTexture_ = checkGLExtension("GL_AMD_compressed_ATC_texture");
	imgTextureCompressionPVRTC_ = checkGLExtension("GL_IMG_texture_compression_pvrtc");
#endif
}


