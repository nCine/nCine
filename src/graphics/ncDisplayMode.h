#ifndef CLASS_NCDISPLAYMODE
#define CLASS_NCDISPLAYMODE

#include <SDL/SDL.h>
#include "../base/ncPoint.h"

/// Display properties
class ncDisplayMode
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
	ncDisplayMode()
		: m_bIsDoubleBuffered(true), m_bIsVSynced(false), m_iDepthBits(-1), m_iStencilBits(-1) {}
	ncDisplayMode(bool bDoubleBuffered, bool bVSynced, int iDepthBits, int iStencilBits)
		: m_bIsDoubleBuffered(bDoubleBuffered), m_bIsVSynced(bVSynced),
		  m_iDepthBits(iDepthBits), m_iStencilBits(iStencilBits) {}
	// Destructor
	~ncDisplayMode() { }

	// Getters
	bool isDoubleBuffered() const { return m_bIsDoubleBuffered; }
	bool isVSynced() const { return m_bIsVSynced; }
	int DepthBits() const { return m_iDepthBits; }
	int StencilBits() const { return m_iStencilBits; }
};

#endif
