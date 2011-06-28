#ifndef CLASS_NCGFXDEVICE
#define CLASS_NCGFXDEVICE

#ifndef NO_GLEW
	#include <GL/glew.h>
#else
	#include <GL/gl.h>
	#include <GL/glext.h>
#endif
#include <SDL/SDL.h>

#include "ncPoint.h"
#include "ncDisplayMode.h"

/// This represents a graphics device where everything is rendered.
/// It uses SDL to create an OpenGL context.
class ncGfxDevice
{
private:
	/// Device width
	int m_iWidth;
	/// Device height
	int m_iHeight;
	/// Device rendering occurs inside a window
	bool m_bIsWindowed;
	/// Display properties
	ncDisplayMode m_mode;

	// Init the video subsystem (SDL)
	void InitGraphics();
	// Init the OpenGL graphic context
	void InitDevice();
	// Init starting OpenGL state
	void InitGL();
public:
	// Constructor
	// Constructor taking the resolution as two integer
	ncGfxDevice(int iWidth, int iHeight);
	// Constructor taking the resolution as a size class
	ncGfxDevice(ncPoint size);
	// Constructor taking the resolution as two integer and a DisplayMode
	ncGfxDevice(int iWidth, int iHeight, ncDisplayMode mode);
	// Constructor taking the resolution as a Size class and a DisplayMode
	ncGfxDevice(ncPoint size, ncDisplayMode mode);
	// Destructor
	~ncGfxDevice() { }

	// Set screen resolution with two integers
	void SetResolution(int iWidth, int iHeight);
	// Set screen resolution with the Size class
	void SetResolution(ncPoint size);

	// Toggle between fullscreen and windowed mode
	void ToggleFullScreen();

	/// Update the screen swapping back and front buffers
	inline void Update() { SDL_GL_SwapBuffers(); }
	/// Clear the screen
	inline void Clear() { glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); }

	/// Return device width
	inline int Width() const { return m_iWidth; }
	/// Return device height
	inline int Height() const { return m_iHeight; }
	/// Return true if the device renders inside a window
	inline bool isWindowed() const { return m_bIsWindowed; }
	/// Return display mode
	inline ncDisplayMode Mode() const { return m_mode; }
};

#endif
