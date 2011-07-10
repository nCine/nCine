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

#include "ncTextureLoader.h"
#include "ncPoint.h"

/// Texture class
class ncTexture
{
private:
	GLuint m_uId;
	int m_iWidth;
	int m_iHeight;

	void Load(const ncTextureLoader &texLoader);
	void Load(const ncTextureLoader &texLoader, int iWidth, int iHeight);
public:
	ncTexture();
	ncTexture(const char *pFilename);
	ncTexture(const char *pFilename, int iWidth, int iHeight);
	ncTexture(const char *pFilename, ncPoint size);
	~ncTexture();

	/// Return OpenGL id
	inline GLuint Id() const { return m_uId; }
	/// Return texture width
	inline int Width() const { return m_iWidth; }
	/// Return texture height
	inline int Height() const { return m_iHeight; }
	/// Return texture size
	inline ncPoint Size() const { return ncPoint(m_iWidth, m_iHeight); }

	// Set texture filtering for both magnification and minification
	void SetFiltering(GLenum eFilter);
	/// Bind the texture to the current unit
	inline void Bind() { glBindTexture(GL_TEXTURE_2D, m_uId); }
	/// Disable texture rendering for the current unit
	static void Unbind() { glBindTexture(GL_TEXTURE_2D, 0); }
};

#endif
