#ifndef CLASS_NCDISPLAYMODE
#define CLASS_NCDISPLAYMODE

#include "ncPoint.h"

/// Display properties
class ncDisplayMode
{
  public:
	// Constructor with no parameters
	ncDisplayMode()
		: m_uRedBits(0), m_uGreenBits(0), m_uBlueBits(0), m_uAlphaBits(0), m_uBufferBits(0),
		  m_uDepthBits(0), m_uStencilBits(0), m_bIsDoubleBuffered(true), m_bIsVSynced(false) { }
	ncDisplayMode(int iRedBits, int iGreenBits, int iBlueBits)
		: m_uRedBits(iRedBits), m_uGreenBits(iGreenBits), m_uBlueBits(iBlueBits),
		  m_uAlphaBits(0), m_uBufferBits(0), m_uDepthBits(0), m_uStencilBits(0),
		  m_bIsDoubleBuffered(true), m_bIsVSynced(false) { }
	ncDisplayMode(int iRedBits, int iGreenBits, int iBlueBits, int iAlphaBits)
		: m_uRedBits(iRedBits), m_uGreenBits(iGreenBits), m_uBlueBits(iBlueBits),
		  m_uAlphaBits(iAlphaBits), m_uBufferBits(0), m_uDepthBits(0), m_uStencilBits(0),
		  m_bIsDoubleBuffered(true), m_bIsVSynced(false) { }
	ncDisplayMode(bool bDoubleBuffered, bool bVSynced, int iDepthBits, int iStencilBits)
		: m_uRedBits(0), m_uGreenBits(0), m_uBlueBits(0), m_uAlphaBits(0), m_uBufferBits(0),
		  m_uDepthBits(iDepthBits), m_uStencilBits(iStencilBits),
		  m_bIsDoubleBuffered(bDoubleBuffered), m_bIsVSynced(bVSynced) { }
	ncDisplayMode(int iRedBits, int iGreenBits, int iBlueBits, int iAlphaBits, int iBufferBits,
	              bool bDoubleBuffered, bool bVSynced, int iDepthBits, int iStencilBits)
		: m_uRedBits(iRedBits), m_uGreenBits(iGreenBits), m_uBlueBits(iBlueBits),
		  m_uAlphaBits(iAlphaBits), m_uBufferBits(iBufferBits),
		  m_uDepthBits(iDepthBits), m_uStencilBits(iStencilBits),
		  m_bIsDoubleBuffered(bDoubleBuffered), m_bIsVSynced(bVSynced) { }
	// Destructor
	~ncDisplayMode() { }

	// Getters
	/// Returns the number of bits for the red channel
	inline unsigned int RedBits() const { return m_uRedBits; }
	/// Returns the number of bits for the green channel
	inline unsigned int GreenBits() const { return m_uGreenBits; }
	/// Returns the number of bits for the blue channel
	inline unsigned int BlueBits() const { return m_uBlueBits; }
	/// Returns the number of bits for the alpha channel
	inline unsigned int AlphaBits() const { return m_uAlphaBits; }
	/// Returns the number of bits for the color buffer
	inline unsigned int BufferBits() const { return m_uBufferBits; }
	/// Returns the number of bits for the depth buffer
	inline unsigned int DepthBits() const { return m_uDepthBits; }
	/// Returns the number of bits for the stencil buffer
	inline unsigned int StencilBits() const { return m_uStencilBits; }
	/// Returns true if the display is double buffered
	inline bool isDoubleBuffered() const { return m_bIsDoubleBuffered; }
	/// Returns true if the dislpay has V-sync enabled
	inline bool isVSynced() const { return m_bIsVSynced; }

  private:
	/// Red component bits
	unsigned int m_uRedBits;
	/// Green component bits
	unsigned int m_uGreenBits;
	/// Blue component bits
	unsigned int m_uBlueBits;
	/// Alpha component bits
	unsigned int m_uAlphaBits;
	/// Frame buffer bits
	unsigned int m_uBufferBits;
	/// Depth buffer size in bit
	unsigned int m_uDepthBits;
	/// Stencil buffer size in bit
	unsigned int m_uStencilBits;
	/// Double buffering flag
	bool m_bIsDoubleBuffered;
	/// VSync flag
	bool m_bIsVSynced;
};

#endif
