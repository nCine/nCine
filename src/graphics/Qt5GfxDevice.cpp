#ifdef WITH_GLEW
	#define GLEW_NO_GLU
	#include <GL/glew.h>
#endif

#include <cmath>
#include <QWindow>
#include <QApplication>
#include <QScreen>

#include "common_macros.h"
#include "GLTexture.h"
#include "GLFramebufferObject.h"
#include "Qt5GfxDevice.h"
#include "PCApplication.h"
#include "Qt5Widget.h"

namespace ncine {

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

QScreen *Qt5GfxDevice::screenPointers_[MaxMonitors];
nctl::StaticString<Qt5GfxDevice::MaxMonitorNameLength> Qt5GfxDevice::monitorNames_[MaxMonitors];

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

Qt5GfxDevice::Qt5GfxDevice(const WindowMode &windowMode, const GLContextInfo &glContextInfo, const DisplayMode &displayMode, Qt5Widget &widget)
    : IGfxDevice(windowMode, glContextInfo, displayMode), widget_(widget)
{
#if QT_VERSION >= 0x050600
	const Qt::ApplicationAttribute scalingAttribute = windowMode.hasWindowScaling ? Qt::AA_EnableHighDpiScaling : Qt::AA_DisableHighDpiScaling;
	QCoreApplication::setAttribute(scalingAttribute);
#endif

	initWindowScaling(windowMode);
	initDevice(windowMode);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void Qt5GfxDevice::setSwapInterval(int interval)
{
	widget_.format().setSwapInterval(interval);
}

void Qt5GfxDevice::setFullScreen(bool fullScreen)
{
	if (isFullScreen_ == fullScreen)
		return;

	QWidget *window = widget_.window();
	if (fullScreen != window->isFullScreen())
	{
		if (fullScreen)
			window->showFullScreen();
		else
			window->showNormal();
	}

	isFullScreen_ = fullScreen;
	// width and height are updated by the resize event that calls `resizeWindow()`
}

int Qt5GfxDevice::windowPositionX() const
{
	QWidget *window = widget_.window();
	return window->pos().x();
}

int Qt5GfxDevice::windowPositionY() const
{
	QWidget *window = widget_.window();
	return window->pos().y();
}

const Vector2i Qt5GfxDevice::windowPosition() const
{
	QWidget *window = widget_.window();
	return Vector2i(window->pos().x(), window->pos().y());
}

void Qt5GfxDevice::setWindowPosition(int x, int y)
{
	QWidget *window = widget_.window();
	window->move(x, y);
}

void Qt5GfxDevice::setWindowSize(int width, int height)
{
	// change resolution only in case it is valid and it really changes
	if (width <= 0 || height <= 0 || (width == width_ && height == height_))
		return;

	if (theApplication().appConfiguration().resizable == false)
	{
		widget_.setMinimumSize(width, height);
		widget_.setMaximumSize(width, height);
	}

	QWidget *window = widget_.window();
	window->resize(width, height);
	// width and height are updated by the resize event that calls `resizeWindow()`
}

void Qt5GfxDevice::setWindowTitle(const char *windowTitle)
{
	QWidget *window = widget_.window();
	window->setWindowTitle(windowTitle);
}

void Qt5GfxDevice::setWindowIcon(const char *windowIconFilename)
{
	QWidget *window = widget_.window();
	window->setWindowIcon(QIcon(windowIconFilename));
}

void Qt5GfxDevice::flashWindow() const
{
	QApplication::alert(&widget_, 0);
}

unsigned int Qt5GfxDevice::primaryMonitorIndex() const
{
	QScreen *screen = QApplication::primaryScreen();

	const int retrievedIndex = retrieveMonitorIndex(screen);
	const unsigned int index = (retrievedIndex >= 0) ? static_cast<unsigned int>(retrievedIndex) : 0;
	return index;
}

unsigned int Qt5GfxDevice::windowMonitorIndex() const
{
	// Fallback value if a monitor containing the window cannot be found
	QScreen *screen = QApplication::primaryScreen();
	QWindow *window = widget_.window()->windowHandle();
	if (window != nullptr)
		screen = window->screen();

	const int retrievedIndex = retrieveMonitorIndex(screen);
	const unsigned int index = (retrievedIndex >= 0) ? static_cast<unsigned int>(retrievedIndex) : 0;
	return index;
}

const IGfxDevice::VideoMode &Qt5GfxDevice::currentVideoMode(unsigned int monitorIndex) const
{
	if (monitorIndex >= numMonitors_)
		monitorIndex = 0;

	return monitors_[monitorIndex].videoModes[0];
}

#ifdef WITH_GLEW
void Qt5GfxDevice::initGlew()
{
	const GLenum err = glewInit();
	FATAL_ASSERT_MSG_X(err == GLEW_OK, "GLEW error: %s", glewGetErrorString(err));

	glContextInfo_.debugContext = glContextInfo_.debugContext && glewIsSupported("GL_ARB_debug_output");
}
#endif

void Qt5GfxDevice::resetTextureBinding()
{
	GLTexture::bindHandle(GL_TEXTURE_2D, 0);
}

void Qt5GfxDevice::bindDefaultDrawFramebufferObject()
{
	const GLuint glHandle = widget_.defaultFramebufferObject();
	GLFramebufferObject::bindHandle(GL_DRAW_FRAMEBUFFER, glHandle);
}

/*! \note It should be used after each `QOpenGLWidget::makeCurrent()` call */
void Qt5GfxDevice::resetFramebufferObjectBinding()
{
	const GLuint glHandle = widget_.defaultFramebufferObject();
	GLFramebufferObject::setBoundHandle(GL_FRAMEBUFFER, glHandle);
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void Qt5GfxDevice::initDevice(const WindowMode &windowMode)
{
	// At this point `updateMonitors()` has already been called by `initWindowScaling()`

	QSurfaceFormat format;
	format.setRedBufferSize(displayMode_.redBits());
	format.setGreenBufferSize(displayMode_.greenBits());
	format.setBlueBufferSize(displayMode_.blueBits());
	//format.setAlphaBufferSize(displayMode_.alphaBits());
	format.setSwapBehavior(displayMode_.isDoubleBuffered() ? QSurfaceFormat::DoubleBuffer : QSurfaceFormat::SingleBuffer);
	format.setDepthBufferSize(displayMode_.depthBits());
	format.setStencilBufferSize(displayMode_.stencilBits());
	format.setVersion(glContextInfo_.majorVersion, glContextInfo_.minorVersion);
#if defined(WITH_OPENGLES)
	format.setRenderableType(QSurfaceFormat::OpenGLES);
#endif
	format.setProfile(glContextInfo_.coreProfile ? QSurfaceFormat::CoreProfile : QSurfaceFormat::CompatibilityProfile);
	if (glContextInfo_.debugContext)
		format.setOptions(QSurfaceFormat::DebugContext);

	if (width_ <= 0 || height_ <= 0 || isFullScreen_)
	{
		// Can't set the full screen window state in a method called by the constructor
		isFullScreen_ = true;
	}

	QWidget *window = widget_.window();
	const bool windowPositionIsValid = (containingMonitorIndex(windowMode) != -1);
	const bool ignoreBothWindowPosition = (windowMode.windowPositionX == AppConfiguration::WindowPositionIgnore &&
	                                       windowMode.windowPositionY == AppConfiguration::WindowPositionIgnore);
	if (isFullScreen_ == false && windowPositionIsValid && ignoreBothWindowPosition == false)
	{
		QPoint windowPos = window->pos();
		if (windowMode.windowPositionX != AppConfiguration::WindowPositionIgnore)
			windowPos.setX(windowMode.windowPositionX);
		if (windowMode.windowPositionY != AppConfiguration::WindowPositionIgnore)
			windowPos.setY(windowMode.windowPositionY);
		window->move(windowPos);
	}

	const int interval = displayMode_.hasVSync() ? 1 : 0;
	format.setSwapInterval(interval);

	widget_.setFormat(format);
	QSurfaceFormat::setDefaultFormat(format);

	initGLViewport();
}

void Qt5GfxDevice::updateMonitors()
{
	const QList<QScreen *> screens = QApplication::screens();

	const int monitorCount = screens.count();
	ASSERT(monitorCount >= 1);
	numMonitors_ = (monitorCount < MaxMonitors) ? monitorCount : MaxMonitors;

	for (unsigned int i = 0; i < MaxMonitors; i++)
		screenPointers_[i] = (i < numMonitors_) ? screens[i] : nullptr;

	for (unsigned int i = 0; i < numMonitors_; i++)
	{
		const int charsToCopy = screens[i]->name().length() < MaxMonitorNameLength - 1 ? screens[i]->name().length() : MaxMonitorNameLength - 1;
		ASSERT(charsToCopy > 0);
		monitorNames_[i].setLength(charsToCopy + 1);
		for (int j = 0; j < charsToCopy; j++)
			monitorNames_[i][j] = static_cast<char>(screens[i]->name().at(j).cell());
		monitorNames_[i][charsToCopy] = '\0';
		monitorNames_[i].setLength(charsToCopy);

		monitors_[i].name = monitorNames_[i].data();

		const QRect geometry = screens[i]->geometry();
		monitors_[i].position.x = geometry.x();
		monitors_[i].position.y = geometry.y();

		const qreal logicalDpiX = screens[i]->logicalDotsPerInchX(); // can return an infinite value
		const qreal logicalDpiY = screens[i]->logicalDotsPerInchY(); // can return an infinite value
		const float DpiX = (std::isfinite(logicalDpiX)) ? logicalDpiX : DefaultDpi;
		const float DpiY = (std::isfinite(logicalDpiY)) ? logicalDpiY : DefaultDpi;
		monitors_[i].dpi.x = static_cast<int>(DpiX);
		monitors_[i].dpi.y = static_cast<int>(DpiY);
		monitors_[i].scale.x = DpiX / DefaultDpi;
		monitors_[i].scale.y = DpiY / DefaultDpi;

		monitors_[i].numVideoModes = 1;
		monitors_[i].videoModes[0].width = screens[i]->size().width();
		monitors_[i].videoModes[0].height = screens[i]->size().height();
		monitors_[i].videoModes[0].refreshRate = screens[i]->refreshRate();
		if (screens[i]->depth() >= 24)
		{
			monitors_[i].videoModes[0].redBits = 8;
			monitors_[i].videoModes[0].greenBits = 8;
			monitors_[i].videoModes[0].blueBits = 8;
		}
	}
}

int Qt5GfxDevice::retrieveMonitorIndex(QScreen *screen) const
{
	if (screen == nullptr)
		return -1;

	int index = -1;
	for (unsigned int i = 0; i < numMonitors_; i++)
	{
		if (screenPointers_[i] == screen)
		{
			index = i;
			break;
		}
	}
	return index;
}

void Qt5GfxDevice::setSize(int width, int height)
{
	width_ = width;
	height_ = height;
	QWidget *window = widget_.window();
	const int pixelRatio = window->devicePixelRatio();
	drawableWidth_ = width_ * pixelRatio;
	drawableHeight_ = height_ * pixelRatio;
}

}
