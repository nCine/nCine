#include <cstdlib> // for exit()
#include "ncTextureLoaderSDL.h"
#include "ncServiceLocator.h"

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

ncTextureLoaderSDL::ncTextureLoaderSDL(const char *pFilename)
	: ncITextureLoader(pFilename), m_pSDLSurface(NULL)
{
	Init();
}

ncTextureLoaderSDL::ncTextureLoaderSDL(ncIFile *pFileHandle)
	: ncITextureLoader(pFileHandle), m_pSDLSurface(NULL)
{
	Init();
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void ncTextureLoaderSDL::Init()
{
	ncServiceLocator::Logger().Write(ncILogger::LOG_INFO, (const char *)"ncTextureLoaderSDL::Init - Loading \"%s\"", m_pFileHandle->Filename());
	m_pSDLSurface = (SDL_Surface *)IMG_Load(m_pFileHandle->Filename());
	if (!m_pSDLSurface)
	{
		ncServiceLocator::Logger().Write(ncILogger::LOG_FATAL, (const char *)"ncTextureLoaderSDL::Init - Cannot load \"%s\"", m_pFileHandle->Filename());
		exit(-1);
	}

	m_iBpp = m_pSDLSurface->format->BitsPerPixel;
	if (m_iBpp == 32)
		m_texFormat = ncTextureFormat(GL_RGBA8);
	else if (m_iBpp  == 24)
		m_texFormat = ncTextureFormat(GL_RGB8);
	else if (m_iBpp == 8)
		m_texFormat = ncTextureFormat(GL_ALPHA8);
	else
	{
		ncServiceLocator::Logger().Write(ncILogger::LOG_FATAL, (const char *)"ncTextureLoaderSDL::Init - Not a true color or alpha image: %d", m_iBpp);
		exit(-1);
	}

	if (m_pSDLSurface->format->Rmask != 0x000000ff)
		m_texFormat.BGRFormat();

	m_iWidth = m_pSDLSurface->w;
	m_iHeight = m_pSDLSurface->h;
	m_uPixels = static_cast<GLubyte*>(m_pSDLSurface->pixels);
}

ncTextureLoaderSDL::~ncTextureLoaderSDL()
{
	if (m_pSDLSurface)
	{
		SDL_FreeSurface(m_pSDLSurface);
		m_uPixels = NULL;
	}
}
