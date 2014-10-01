#ifndef CLASS_NCIGFXDEVICE
#define CLASS_NCIGFXDEVICE

#include "ncPoint.h"
#include "ncDisplayMode.h"

/// It represents the interface to the graphics device where everything is rendered
class ncIGfxDevice
{
 public:
	virtual ~ncIGfxDevice() = 0;

	/// Sets screen resolution with two integers
	virtual void SetResolution(int iWidth, int iHeight) = 0;
	/// Sets screen resolution with the Size class
	virtual void SetResolution(ncPoint size) = 0;

	/// Toggles between fullscreen and windowed mode
	virtual void ToggleFullScreen() { }

	/// Updates the screen swapping back and front buffers
	virtual void Update() = 0;
	/// Clears the screen
	virtual void Clear();

	/// Sets the application window title
	virtual void SetWindowTitle(const char *pWindowTitle) = 0;

	/// Returns device resolution
	inline ncPoint Resolution() const { return ncPoint(m_iWidth, m_iHeight); }
	/// Returns device width
	inline int Width() const { return m_iWidth; }
	/// Returns device height
	inline int Height() const { return m_iHeight; }
	/// Returns true if the device renders inside a window
	inline bool isWindowed() const { return m_bIsWindowed; }
	/// Returns display mode
	inline const ncDisplayMode& Mode() const { return m_mode; }

 protected:
	// Initilizes starting OpenGL state
	virtual void InitGL();

	/// Device width
	int m_iWidth;
	/// Device height
	int m_iHeight;
	/// Display properties
	ncDisplayMode m_mode;
	/// Device rendering occurs inside a window
	bool m_bIsWindowed;
};

inline ncIGfxDevice::~ncIGfxDevice() { }

#endif
