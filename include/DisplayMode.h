#ifndef CLASS_NCDISPLAYMODE
#define CLASS_NCDISPLAYMODE

#include "common_defines.h"

namespace ncine {

/// Display properties
class DLL_PUBLIC DisplayMode
{
  public:
	// Constructor with no parameters
	DisplayMode()
		: redBits_(0), greenBits_(0), blueBits_(0), alphaBits_(0), bufferBits_(0),
		  depthBits_(0), stencilBits_(0), isDoubleBuffered_(true), hasVSync_(false) { }
	DisplayMode(int redBits, int greenBits, int blueBits)
		: redBits_(redBits), greenBits_(greenBits), blueBits_(blueBits),
		  alphaBits_(0), bufferBits_(0), depthBits_(0), stencilBits_(0),
		  isDoubleBuffered_(true), hasVSync_(false) { }
	DisplayMode(int redBits, int greenBits, int blueBits, int alphaBits)
		: redBits_(redBits), greenBits_(greenBits), blueBits_(blueBits),
		  alphaBits_(alphaBits), bufferBits_(0), depthBits_(0), stencilBits_(0),
		  isDoubleBuffered_(true), hasVSync_(false) { }
	DisplayMode(bool doubleBuffered, bool withVSync, int depthBits, int stencilBits)
		: redBits_(0), greenBits_(0), blueBits_(0), alphaBits_(0), bufferBits_(0),
		  depthBits_(depthBits), stencilBits_(stencilBits),
		  isDoubleBuffered_(doubleBuffered), hasVSync_(withVSync) { }
	DisplayMode(int redBits, int greenBits, int blueBits, int alphaBits, int bufferBits,
				  bool doubleBuffered, bool withVSync, int depthBits, int stencilBits)
		: redBits_(redBits), greenBits_(greenBits), blueBits_(blueBits),
		  alphaBits_(alphaBits), bufferBits_(bufferBits),
		  depthBits_(depthBits), stencilBits_(stencilBits),
		  isDoubleBuffered_(doubleBuffered), hasVSync_(withVSync) { }

	// Getters
	/// Returns the number of bits for the red channel
	inline unsigned int redBits() const { return redBits_; }
	/// Returns the number of bits for the green channel
	inline unsigned int greenBits() const { return greenBits_; }
	/// Returns the number of bits for the blue channel
	inline unsigned int blueBits() const { return blueBits_; }
	/// Returns the number of bits for the alpha channel
	inline unsigned int alphaBits() const { return alphaBits_; }
	/// Returns the number of bits for the color buffer
	inline unsigned int bufferBits() const { return bufferBits_; }
	/// Returns the number of bits for the depth buffer
	inline unsigned int depthBits() const { return depthBits_; }
	/// Returns the number of bits for the stencil buffer
	inline unsigned int stencilBits() const { return stencilBits_; }
	/// Returns true if the display is double buffered
	inline bool isDoubleBuffered() const { return isDoubleBuffered_; }
	/// Returns true if the dislpay has V-sync enabled
	inline bool hasVSync() const { return hasVSync_; }

  private:
	/// Red component bits
	unsigned int redBits_;
	/// Green component bits
	unsigned int greenBits_;
	/// Blue component bits
	unsigned int blueBits_;
	/// Alpha component bits
	unsigned int alphaBits_;
	/// Frame buffer bits
	unsigned int bufferBits_;
	/// Depth buffer size in bit
	unsigned int depthBits_;
	/// Stencil buffer size in bit
	unsigned int stencilBits_;
	/// Double buffering flag
	bool isDoubleBuffered_;
	/// VSync flag
	bool hasVSync_;
};

}

#endif
