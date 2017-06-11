#define NCINE_INCLUDE_OPENGL
#include "common_headers.h"

#include <cstring> // for CheckGLExtension()
#include "GfxCapabilities.h"
#include "ServiceLocator.h"

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

GfxCapabilities::GfxCapabilities()
	: glMajorVersion_(0),
	  glMinorVersion_(0),
	  glReleaseVersion_(0)
{
	for (unsigned int i = 0; i < IGfxCapabilities::NUM_INTVALUES; i++)
	{
		glIntValues_[i] = 0;
	}

	for (unsigned int i = 0; i < IGfxCapabilities::NUM_EXTENSIONS; i++)
	{
		glExtensions_[i] = false;
	}

	init();
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

int GfxCapabilities::glVersion(IGfxCapabilities::GLVersion version) const
{
	switch (version)
	{
		case IGfxCapabilities::MAJOR:	return glMajorVersion_;
		case IGfxCapabilities::MINOR:	return glMinorVersion_;
		case IGfxCapabilities::RELEASE:	return glReleaseVersion_;

		default : return 0;
	}
}

int GfxCapabilities::value(GLIntValues valueName) const
{
	int value = 0;

	if (valueName >= 0 && valueName < NUM_INTVALUES)
	{
		value = glIntValues_[valueName];
	}

	return value;
}

bool GfxCapabilities::hasExtension(GLExtensions extensionName) const
{
	bool extensionAvailable = false;

	if (extensionName >= 0 && extensionName < NUM_EXTENSIONS)
	{
		extensionAvailable = glExtensions_[extensionName];
	}

	return extensionAvailable;
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void GfxCapabilities::init()
{
	const char *pVersion = reinterpret_cast<const char *>(glGetString(GL_VERSION));
#ifndef __ANDROID__
	sscanf(pVersion, "%2d.%2d.%2d", &glMajorVersion_, &glMinorVersion_, &glReleaseVersion_);
#else
	sscanf(pVersion, "OpenGL ES %2d.%2d", &glMajorVersion_, &glMinorVersion_);
#endif

	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &glIntValues_[MAX_TEXTURE_SIZE]);
	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &glIntValues_[MAX_TEXTURE_IMAGE_UNITS]);

	glExtensions_[EXT_TEXTURE_COMPRESSION_S3TC] = checkGLExtension("GL_EXT_texture_compression_s3tc");
	glExtensions_[OES_COMPRESSED_ETC1_RGB8_TEXTURE] = checkGLExtension("GL_OES_compressed_ETC1_RGB8_texture");
	glExtensions_[AMD_COMPRESSED_ATC_TEXTURE] = checkGLExtension("GL_AMD_compressed_ATC_texture");
	glExtensions_[IMG_TEXTURE_COMPRESSION_PVRTC] = checkGLExtension("GL_IMG_texture_compression_pvrtc");
	glExtensions_[KHR_TEXTURE_COMPRESSION_ASTC_LDR] = checkGLExtension("GL_KHR_texture_compression_astc_ldr");
}

void GfxCapabilities::logGLInfo()
{
	LOGI("--- OpenGL device info ---");
	LOGI_X("Vendor: %s", glGetString(GL_VENDOR));
	LOGI_X("Renderer: %s", glGetString(GL_RENDERER));
	LOGI_X("OpenGL Version: %s", glGetString(GL_VERSION));
	LOGI_X("GLSL Version: %s", glGetString(GL_SHADING_LANGUAGE_VERSION));
	LOGI("--- OpenGL device info ---");
}

void GfxCapabilities::logGLExtensions()
{
	LOGI("--- OpenGL extensions ---");
	LOGI_X("Extensions: %s", glGetString(GL_EXTENSIONS));
	LOGI("--- OpenGL extensions ---");
}

void GfxCapabilities::logGLCaps() const
{
	LOGI("--- OpenGL device capabilities ---");
	LOGI_X("GL_MAX_TEXTURE_SIZE: %d", glIntValues_[MAX_TEXTURE_SIZE]);
	LOGI_X("GL_MAX_TEXTURE_IMAGE_UNITS: %d", glIntValues_[MAX_TEXTURE_IMAGE_UNITS]);
	LOGI("---");
	LOGI_X("GL_EXT_texture_compression_s3tc: %d", glExtensions_[EXT_TEXTURE_COMPRESSION_S3TC]);
	LOGI_X("GL_OES_compressed_ETC1_RGB8_texture: %d", glExtensions_[OES_COMPRESSED_ETC1_RGB8_TEXTURE]);
	LOGI_X("GL_AMD_compressed_ATC_texture: %d", glExtensions_[AMD_COMPRESSED_ATC_TEXTURE]);
	LOGI_X("GL_IMG_texture_compression_pvrtc: %d", glExtensions_[IMG_TEXTURE_COMPRESSION_PVRTC]);
	LOGI_X("GL_KHR_texture_compression_astc_ldr: %d", glExtensions_[KHR_TEXTURE_COMPRESSION_ASTC_LDR]);
	LOGI("--- OpenGL device capabilities ---");
}

bool GfxCapabilities::checkGLExtension(const char *extensionName) const
{
	size_t nameLength = strlen(extensionName);
	const char *extensions = reinterpret_cast<const char *>(glGetString(GL_EXTENSIONS));

	if (extensions)
	{
		/*
		 ** Search for extName in the extensions string.  Use of strstr()
		 ** is not sufficient because extension names can be prefixes of
		 ** other extension names.  Could use strtok() but the constant
		 ** string returned by glGetString can be in read-only memory.
		 */

		const char *end = extensions + strlen(extensions);

		while (extensions < end)
		{
			size_t n = strcspn(extensions, " ");
			if ((nameLength == n) && (strncmp(extensionName, extensions, n) == 0))
			{
				return true;
			}
			extensions += (n + 1);
		}
	}
#if !(defined(__ANDROID__) && !defined(GL_ES_VERSION_3_0))
	else
	{
		GLint n, i;
		glGetIntegerv(GL_NUM_EXTENSIONS, &n);

		for (i = 0; i < n; i++)
		{
			const char *extension = reinterpret_cast<const char *>(glGetStringi(GL_EXTENSIONS, i));
			if (strncmp(extensionName, extension, nameLength) == 0)
			{
				return true;
			}
		}
	}
#endif

	return false;
}

}
