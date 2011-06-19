#ifndef CLASS_DISPLAYMODE
#define CLASS_DISPLAYMODE

#include <SDL/SDL.h>
#include "../base/Size.h"

/// Display properties
class DisplayMode
{
private:
	/// Double buffering flag
	bool m_bIsDoubleBuffered;
	/// VSync flag
	bool m_bIsVSynced;
	/// Depth buffer size in bit
	int m_iDepthBits;
	/// Stencil buffer size in bit
	int m_iStencilBits;
public:
	// Constructor with no parameters
	DisplayMode()
		: m_bIsDoubleBuffered(true), m_bIsVSynced(false), m_iDepthBits(-1), m_iStencilBits(-1) {}
	DisplayMode(bool bDoubleBuffered, bool bVSynced, int iDepthBits, int iStencilBits)
		: m_bIsDoubleBuffered(bDoubleBuffered), m_bIsVSynced(bVSynced),
		  m_iDepthBits(iDepthBits), m_iStencilBits(iStencilBits) {}
	// Destructor
	~DisplayMode() { }

	// Getters
	bool isDoubleBuffered() const { return m_bIsDoubleBuffered; }
	bool isVSynced() const { return m_bIsVSynced; }
	int DepthBits() const { return m_iDepthBits; }
	int StencilBits() const { return m_iStencilBits; }
};

#endif
