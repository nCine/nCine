#ifndef CLASS_NCINE_DISPLAYMODE
#define CLASS_NCINE_DISPLAYMODE

#include "common_defines.h"

namespace ncine {

/// Display properties
class DLL_PUBLIC DisplayMode
{
  public:
	enum class DoubleBuffering
	{
		DISABLED,
		ENABLED
	};

	enum class VSync
	{
		DISABLED,
		ENABLED
	};

	DisplayMode(unsigned int redBits, unsigned int greenBits, unsigned int blueBits)
		: DisplayMode(redBits, greenBits, blueBits, 0, 0, 0, DoubleBuffering::ENABLED, VSync::DISABLED) { }
	DisplayMode(unsigned int redBits, unsigned int greenBits, unsigned int blueBits, unsigned int alphaBits)
		: DisplayMode(redBits, greenBits, blueBits, alphaBits, 0, 0, DoubleBuffering::ENABLED, VSync::DISABLED) { }
	DisplayMode(unsigned int depthBits, unsigned int stencilBits, DoubleBuffering dbMode, VSync vsMode)
		: DisplayMode(0, 0, 0, 0, depthBits, stencilBits, dbMode, vsMode) { }
	DisplayMode(unsigned int redBits, unsigned int greenBits, unsigned int blueBits, unsigned int alphaBits,
	            unsigned int depthBits, unsigned int stencilBits, DoubleBuffering dbMode, VSync vsMode)
		: redBits_(redBits), greenBits_(greenBits), blueBits_(blueBits), alphaBits_(alphaBits),
		  depthBits_(depthBits), stencilBits_(stencilBits), isDoubleBuffered_(dbMode == DoubleBuffering::ENABLED),
		  hasVSync_(vsMode == VSync::ENABLED) { }

	/// Returns the number of bits for the red channel
	inline unsigned int redBits() const { return redBits_; }
	/// Returns the number of bits for the green channel
	inline unsigned int greenBits() const { return greenBits_; }
	/// Returns the number of bits for the blue channel
	inline unsigned int blueBits() const { return blueBits_; }
	/// Returns the number of bits for the alpha channel
	inline unsigned int alphaBits() const { return alphaBits_; }
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
