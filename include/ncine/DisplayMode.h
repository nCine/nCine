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

	DisplayMode(unsigned char redBits, unsigned char greenBits, unsigned char blueBits)
	    : DisplayMode(redBits, greenBits, blueBits, 0, 0, 0, DoubleBuffering::ENABLED, VSync::DISABLED) {}
	DisplayMode(unsigned char redBits, unsigned char greenBits, unsigned char blueBits, unsigned char alphaBits)
	    : DisplayMode(redBits, greenBits, blueBits, alphaBits, 0, 0, DoubleBuffering::ENABLED, VSync::DISABLED) {}
	DisplayMode(unsigned char depthBits, unsigned char stencilBits, DoubleBuffering dbMode, VSync vsMode)
	    : DisplayMode(0, 0, 0, 0, depthBits, stencilBits, dbMode, vsMode) {}
	DisplayMode(unsigned char redBits, unsigned char greenBits, unsigned char blueBits, unsigned char alphaBits,
	            unsigned char depthBits, unsigned char stencilBits, DoubleBuffering dbMode, VSync vsMode)
	    : redBits_(redBits), greenBits_(greenBits), blueBits_(blueBits), alphaBits_(alphaBits),
	      depthBits_(depthBits), stencilBits_(stencilBits), isDoubleBuffered_(dbMode == DoubleBuffering::ENABLED),
	      hasVSync_(vsMode == VSync::ENABLED) {}

	/// Returns the number of bits for the red channel
	inline unsigned char redBits() const { return redBits_; }
	/// Returns the number of bits for the green channel
	inline unsigned char greenBits() const { return greenBits_; }
	/// Returns the number of bits for the blue channel
	inline unsigned char blueBits() const { return blueBits_; }
	/// Returns the number of bits for the alpha channel
	inline unsigned char alphaBits() const { return alphaBits_; }
	/// Returns the number of bits for the depth buffer
	inline unsigned char depthBits() const { return depthBits_; }
	/// Returns the number of bits for the stencil buffer
	inline unsigned char stencilBits() const { return stencilBits_; }
	/// Returns true if the display is double buffered
	inline bool isDoubleBuffered() const { return isDoubleBuffered_; }
	/// Returns true if the dislpay has V-sync enabled
	inline bool hasVSync() const { return hasVSync_; }

  private:
	/// Red component bits
	unsigned char redBits_;
	/// Green component bits
	unsigned char greenBits_;
	/// Blue component bits
	unsigned char blueBits_;
	/// Alpha component bits
	unsigned char alphaBits_;
	/// Depth buffer size in bit
	unsigned char depthBits_;
	/// Stencil buffer size in bit
	unsigned char stencilBits_;
	/// Double buffering flag
	bool isDoubleBuffered_;
	/// VSync flag
	bool hasVSync_;
};

}

#endif
