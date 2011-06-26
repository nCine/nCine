#ifndef CLASS_NCTEXTURE
#define CLASS_NCTEXTURE

#ifndef NO_GLEW
	#include <GL/glew.h>
#else
	#include <GL/gl.h>
	#include <GL/glext.h>
#endif
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include "ncTextureFormat.h"
#include "../base/ncPoint.h"

/// Texture class
class ncTexture
{
private:
	GLuint m_uId;
	int m_iWidth;
	int m_iHeight;

	void Load(const char *pFilename);
	void Load(SDL_Surface *image);
	void Load(ncTextureFormat format, int iWidth, int iHeight, GLubyte *pPixels);
public:
	ncTexture();
	ncTexture(const char *pFilename);
	ncTexture(SDL_Surface *pSurface);
	ncTexture(ncTextureFormat format, ncPoint size, GLubyte *pPixels);
	ncTexture(ncTextureFormat format, int iWidth, int iHeight, GLubyte *pPixels);
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
};

#endif
