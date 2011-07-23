#ifndef CLASS_NCTEXTURELOADER
#define CLASS_NCTEXTURELOADER

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

#include <stdint.h> // for ETC1 header
#include "ncTextureFormat.h"
#include "ncPoint.h"

/// Texture loader class
class ncTextureLoader
{
private:

	typedef struct ETC1_magic {
		char cMagicId[6];
		uint16_t uZero;
	} ETC1_magic;

	typedef struct ETC1_header {
		uint16_t uWidth;
		uint16_t uHeight;
		uint16_t uWidth2;
		uint16_t uHeight2;
	} ETC1_header;

	int m_iWidth;
	int m_iHeight;
	int m_iBpp;
	int m_iHeaderSize;
	long m_lFileSize;
	ncTextureFormat m_texFormat;
	GLubyte* m_uPixels;
	FILE *m_pFile;

#ifndef __ANDROID__
	SDL_Surface *m_pSDLSurface;
	void LoadSDL(const char *pFilename);
#endif

	void ReadETC1Header(const char *pFilename);
	void LoadCompressed(const char *pFilename, GLenum eInternalFormat);
public:
	ncTextureLoader(const char *pFilename);
	~ncTextureLoader();

	inline int Width() const { return m_iWidth; }
	inline int Height() const { return m_iHeight; }
	inline ncPoint Size() const { return ncPoint(m_iWidth, m_iHeight); }
	inline int Bpp() const { return m_iBpp; }
	inline long FileSize() const { return m_lFileSize; }
	inline const ncTextureFormat& TexFormat() const { return m_texFormat; }
	inline const GLubyte* Pixels() const { return m_uPixels; }
};

#endif
