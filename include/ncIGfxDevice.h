#ifndef CLASS_NCIGFXDEVICE
#define CLASS_NCIGFXDEVICE

#include "ncPoint.h"
#include "ncDisplayMode.h"

/// It represents the interface to the graphics device where everything is rendered
class ncIGfxDevice
{
protected:
	/// Device width
	int m_iWidth;
	/// Device height
	int m_iHeight;
	/// Display properties
	ncDisplayMode m_mode;
	/// Device rendering occurs inside a window
	bool m_bIsWindowed;

public:
	virtual ~ncIGfxDevice() = 0;

	// Set screen resolution with two integers
	virtual void SetResolution(int iWidth, int iHeight) = 0;
	// Set screen resolution with the Size class
	virtual void SetResolution(ncPoint size) = 0;

	// Toggle between fullscreen and windowed mode
	virtual void ToggleFullScreen() { }

	/// Update the screen swapping back and front buffers
	virtual void Update() = 0;
	/// Clear the screen
	virtual void Clear() = 0;

	/// Return device resolution
	inline ncPoint Resolution() const { return ncPoint(m_iWidth, m_iHeight); }
	/// Return device width
	inline int Width() const { return m_iWidth; }
	/// Return device height
	inline int Height() const { return m_iHeight; }
	/// Return true if the device renders inside a window
	inline bool isWindowed() const { return m_bIsWindowed; }
	/// Return display mode
	inline const ncDisplayMode& Mode() const { return m_mode; }

	// Log OpenGL device info
	void LogGLInfo();
	// Log OpenGL extensions
	void LogGLExtensions();
	// Check for an OpenGL extension
	bool CheckGLExtension(const char *pExtensionName);
};

inline ncIGfxDevice::~ncIGfxDevice() { }

#endif
