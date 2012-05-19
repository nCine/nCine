#ifndef CLASS_NCITEXTURELOADER
#define CLASS_NCITEXTURELOADER

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

#include "ncTextureFormat.h"
#include "ncPoint.h"
#include "ncIFile.h"

/// Texture loader interface
class ncITextureLoader
{
protected:
	/// Texture file handle
	ncIFile *m_pFileHandle;

	int m_iWidth;
	int m_iHeight;
	int m_iBpp;
	int m_iHeaderSize;
	long m_lFileSize;
	ncTextureFormat m_texFormat;
	GLubyte* m_uPixels;

	ncITextureLoader(const char *pFilename);
	ncITextureLoader(ncIFile *pFileHandle);

	// Loads pixel data from a texture file holding either compressed or uncompressed data
	void LoadPixels(GLenum eInternalFormat);
	// Loads pixel data from a texture file holding either compressed or uncompressed data, overriding pixel type
	void LoadPixels(GLenum eInternalFormat, GLenum eType);

	/// Load the texture data from file
	virtual void Init() = 0;

public:
	virtual ~ncITextureLoader();

	/// Returns texture width
	inline int Width() const { return m_iWidth; }
	/// Returns texture height
	inline int Height() const { return m_iHeight; }
	/// Returns texture size as a ncPoint class
	inline ncPoint Size() const { return ncPoint(m_iWidth, m_iHeight); }
	/// Returns texture bytes per pixel
	inline int Bpp() const { return m_iBpp; }
	/// Returns file size in bytes
	inline long FileSize() const { return m_lFileSize; }
	/// Returns the texture format class
	inline const ncTextureFormat& TexFormat() const { return m_texFormat; }
	/// Returns the pointer to pixel data
	inline const GLubyte* Pixels() const { return m_uPixels; }

	// Returns the proper texture loader according to the file extension
	static ncITextureLoader* CreateFromFile(const char *pFilename);
};

#endif
