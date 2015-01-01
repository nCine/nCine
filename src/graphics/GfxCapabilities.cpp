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
#include "GfxCapabilities.h"
#include "ServiceLocator.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

GfxCapabilities::GfxCapabilities()
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
	  oesCompressedEtc1Rgb8Texture_(false),
	  amdCompressedAtcTexture_(false),
	  imgTextureCompressionPvrTC_(false)
#endif
{

}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

/// Logs OpenGL device info
void GfxCapabilities::logGLInfo()
{
	LOGI("OpenGL device info ---");
	LOGI_X("Vendor: %s", glGetString(GL_VENDOR));
	LOGI_X("Renderer: %s", glGetString(GL_RENDERER));
	LOGI_X("OpenGL Version: %s", glGetString(GL_VERSION));
#ifndef __ANDROID__
	// Using OpenGL ES 1.1 at the moment
	LOGI_X("GLSL Version: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));
#endif
	LOGI("OpenGL device info ---");
}

/// Logs OpenGL extensions
void GfxCapabilities::logGLExtensions()
{
	LOGI("OpenGL extensions ---");
	LOGI_X("Extensions: %s", glGetString(GL_EXTENSIONS));
	LOGI("OpenGL extensions ---");
}

/// Logs OpenGL device capabilites
void GfxCapabilities::logGLCaps() const
{
	LOGI("OpenGL device capabilities ---");
	LOGI_X("OpenGL Major: %d", majorGL_);
	LOGI_X("OpenGL Minor: %d", minorGL_);
#ifndef __ANDROID__
	LOGI_X("OpenGL Release: %d", releaseGL_);
#endif
	LOGI("---");
	LOGI_X("GL_MAX_TEXTURE_SIZE: %d", maxTextureSize_);
	LOGI_X("GL_MAX_TEXTURE_UNITS: %d", maxTextureUnits_);
	LOGI("---");
#ifndef __ANDROID__
	LOGI_X("GL_EXT_texture_compression_s3tc: %d", extTextureCompressionS3TC_);
#else
	LOGI_X("GL_OES_compressed_ETC1_RGB8_texture: %d", oesCompressedEtc1Rgb8Texture_);
	LOGI_X("GL_AMD_compressed_ATC_texture: %d", amdCompressedAtcTexture_);
	LOGI_X("GL_IMG_texture_compression_pvrtc: %d", imgTextureCompressionPvrTC_);
#endif
	LOGI("OpenGL device capabilities ---");
}

/// Check for an OpenGL extension
bool GfxCapabilities::checkGLExtension(const char *extensionName) const
{
	/*
	 ** Search for extName in the extensions string.  Use of strstr()
	 ** is not sufficient because extension names can be prefixes of
	 ** other extension names.  Could use strtok() but the constant
	 ** string returned by glGetString can be in read-only memory.
	 */
	const char *extensions = reinterpret_cast<const char *>(glGetString(GL_EXTENSIONS));
	int nameLength = strlen(extensionName);
	const char *end = extensions + strlen(extensions);

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
void GfxCapabilities::init()
{
	const char *pVersion = reinterpret_cast<const char *>(glGetString(GL_VERSION));
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
	oesCompressedEtc1Rgb8Texture_ = checkGLExtension("GL_OES_compressed_ETC1_RGB8_texture");
	amdCompressedAtcTexture_ = checkGLExtension("GL_AMD_compressed_ATC_texture");
	imgTextureCompressionPvrTC_ = checkGLExtension("GL_IMG_texture_compression_pvrtc");
#endif
}

}
