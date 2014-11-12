#ifndef CLASS_NCINE_TEXTURE
#define CLASS_NCINE_TEXTURE

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

#include "Object.h"
#include "Point.h"
#include "Rect.h"

namespace ncine {

class ITextureLoader;

/// Texture class
class Texture : public Object
{
  public:
	Texture();
	Texture(const char *filename);
	Texture(const char *filename, int width, int height);
	Texture(const char *filename, Point size);
	virtual ~Texture();

	/// Returns OpenGL id
	inline GLuint gLId() const { return gLId_; }
	/// Returns texture width
	inline int width() const { return width_; }
	/// Returns texture height
	inline int height() const { return height_; }
	/// Returns texture MIP map levels
	inline int mipMapLevels() const { return mipMapLevels_; }
	/// Returns texture size
	inline Point size() const { return Point(width_, height_); }
	/// Returns texture rectangle
	inline Rect rect() const { return Rect(0, 0, width_, height_); }

	/// Returns true if the texture holds compressed data
	inline bool isCompressed() const { return isCompressed_; }
	/// Returns true if the texture provides an alpha channel
	inline bool hasAlpha() const { return hasAlphaChannel_; }

	// Sets texture filtering for both magnification and minification
	void setFiltering(GLenum filter);
	/// Binds the texture to the current unit
	inline void bind() { glBindTexture(GL_TEXTURE_2D, gLId_); }
	/// Disables texture rendering for the current unit
	static void unbind() { glBindTexture(GL_TEXTURE_2D, 0); }

	inline static ObjectType sType() { return TEXTURE_TYPE; }

  private:
	GLuint gLId_;
	int width_;
	int height_;
	int mipMapLevels_;
	bool isCompressed_;
	bool hasAlphaChannel_;

	// Loads a texture based on information from the texture format and loader
	void load(const ITextureLoader& texLoader);
	// Loads a texture overriding the size detected by the texture loader
	void load(const ITextureLoader& texLoader, int width, int height);
};

}

#endif
