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

#include "ncObject.h"
#include "ncTextureLoader.h"
#include "ncPoint.h"

/// Texture class
class ncTexture : public ncObject
{
private:
	GLuint m_uGLId;
	int m_iWidth;
	int m_iHeight;

	// Loads a texture based on information from the texture format and loader
	void Load(const ncTextureLoader &texLoader);
	// Loads a texture overriding the size detected by the texture loader
	void Load(const ncTextureLoader &texLoader, int iWidth, int iHeight);
public:
	ncTexture();
	ncTexture(const char *pFilename);
	ncTexture(const char *pFilename, int iWidth, int iHeight);
	ncTexture(const char *pFilename, ncPoint size);
	virtual ~ncTexture();

	/// Returns OpenGL id
	inline GLuint GLId() const { return m_uGLId; }
	/// Returns texture width
	inline int Width() const { return m_iWidth; }
	/// Returns texture height
	inline int Height() const { return m_iHeight; }
	/// Returns texture size
	inline ncPoint Size() const { return ncPoint(m_iWidth, m_iHeight); }

	// Sets texture filtering for both magnification and minification
	void SetFiltering(GLenum eFilter);
	/// Binds the texture to the current unit
	inline void Bind() { glBindTexture(GL_TEXTURE_2D, m_uGLId); }
	/// Disables texture rendering for the current unit
	static void Unbind() { glBindTexture(GL_TEXTURE_2D, 0); }
	// Sets the red channel of a second texture as the alpha channel
	void SetAlphaFromRed(ncTexture *pAlphaTex);

	inline static eObjectType sType() { return TEXTURE_TYPE; }
};

#endif
