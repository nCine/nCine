#ifndef CLASS_NCTEXTURE
#define CLASS_NCTEXTURE

#if defined(__ANDROID__)
	#include <GLES/gl.h>
	#include <GLES/glext.h>
#elif !defined(NO_GLEW)
	#include <GL/glew.h>
#else
	#include <GL/gl.h>
	#include <GL/glext.h>
#endif

#ifndef __ANDROID__
	#include <SDL/SDL.h>
	#include <SDL/SDL_image.h>
#endif

#include "ncTextureFormat.h"
#include "ncPoint.h"

/// Texture class
class ncTexture
{
private:
	GLuint m_uId;
	int m_iWidth;
	int m_iHeight;

#ifndef __ANDROID__
	void Load(const char *pFilename);
	void Load(SDL_Surface *image);
#endif
	void Load(ncTextureFormat format, int iWidth, int iHeight, GLubyte *pPixels, GLsizei iBytes);
public:
	ncTexture();
#ifndef __ANDROID__
	ncTexture(const char *pFilename);
	ncTexture(SDL_Surface *pSurface);
#endif
	ncTexture(ncTextureFormat format, ncPoint size, GLubyte *pPixels, GLsizei iBytes = 0);
	ncTexture(ncTextureFormat format, int iWidth, int iHeight, GLubyte *pPixels, GLsizei iBytes = 0);
	~ncTexture();

	/// Return texture size
	inline ncPoint Size() const { return ncPoint(m_iWidth, m_iHeight); }

	// Set texture filtering for both magnification and minification
	void SetFiltering(GLenum eFilter);
	/// Bind the texture to the current unit
	inline void Bind() { glBindTexture(GL_TEXTURE_2D, m_uId); }
	/// Disable texture rendering for the current unit
	static void Unbind() { glBindTexture(GL_TEXTURE_2D, 0); }

	friend class ncSpriteBatch;
	friend class ncRenderGraph;
};

#endif
