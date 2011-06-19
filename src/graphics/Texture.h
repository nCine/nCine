#ifndef CLASS_TEXTURE
#define CLASS_TEXTURE

#ifndef NO_GLEW
	#include <GL/glew.h>
#else
	#include <GL/gl.h>
	#include <GL/glext.h>
#endif
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

#include "TextureFormat.h"

/// Texture class
class Texture
{
private:
	GLuint m_uiId;

	void Load(const char *pFilename);
	void Load(SDL_Surface *image);
	void Load(TextureFormat format, int iWidth, int iHeight, GLubyte *pPixels);
public:
	Texture();
	Texture(const char *pFilename);
	Texture(SDL_Surface *pSurface);
	Texture(TextureFormat format, int iWidth, int iHeight, GLubyte *pPixels);
	~Texture();
	// Set texture filtering for both magnification and minification
	void SetFiltering(GLenum eFilter);
	/// Bind the texture to the current unit
	inline void Bind() { glBindTexture(GL_TEXTURE_2D, m_uiId); }
	/// Disable texture rendering for the current unit
	static void Unbind() { glBindTexture(GL_TEXTURE_2D, 0); };
};

#endif
