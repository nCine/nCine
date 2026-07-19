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
#include "GLBufferObject.h"
#include "GLShaderProgram.h"
#include "GLClearColor.h"
#include "QtGfxDevice.h"
#include "PCApplication.h"
#include "QtWidget.h"

namespace ncine {

///////////////////////////////////////////////////////////
// STATIC DEFINITIONS
///////////////////////////////////////////////////////////

QScreen *QtGfxDevice::screenPointers_[MaxMonitors];
nctl::StaticString<QtGfxDevice::MaxMonitorNameLength> QtGfxDevice::monitorNames_[MaxMonitors];

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

QtGfxDevice::QtGfxDevice(const WindowMode &windowMode, const GLContextInfo &glContextInfo, const DisplayMode &displayMode, QtWidget &widget)
    : IGfxDevice(windowMode, glContextInfo, displayMode), widget_(widget)
{
#if QT_VERSION < QT_VERSION_CHECK(6,0,0)
	const Qt::ApplicationAttribute scalingAttribute = windowMode.hasWindowScaling ? Qt::AA_EnableHighDpiScaling : Qt::AA_DisableHighDpiScaling;
	QCoreApplication::setAttribute(scalingAttribute);
#else
	// High DPI scaling is always enabled starting with Qt6 and cannot be disabled anymore
	if (windowMode.hasWindowScaling == false)
		LOGW("Disabling window scaling was requested but Qt6 always enables High DPI scaling");
#endif

	initWindowScaling(windowMode);
	initDevice(windowMode);
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

void QtGfxDevice::setSwapInterval(int interval)
{
	widget_.format().setSwapInterval(interval);
}

void QtGfxDevice::setFullscreen(bool fullscreen)
{
	if (isFullscreen_ == fullscreen)
		return;

	QWidget *window = widget_.window();
	if (fullscreen != window->isFullScreen())
	{
		if (fullscreen)
			window->showFullScreen();
		else
			window->showNormal();
	}

	isFullscreen_ = fullscreen;
	// width and height are updated by the resize event that calls `resizeWindow()`
}

void QtGfxDevice::setResizable(bool resizable)
{
	QWidget *window = widget_.window();
	if (resizable == false)
		window->setFixedSize(window->size());
	else
	{
		window->setMinimumSize(0, 0);
		window->setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
	}
	isResizable_ = resizable;
}

int QtGfxDevice::windowPositionX() const
{
	QWidget *window = widget_.window();
	return window->pos().x();
}

int QtGfxDevice::windowPositionY() const
{
	QWidget *window = widget_.window();
	return window->pos().y();
}

const Vector2i QtGfxDevice::windowPosition() const
{
	QWidget *window = widget_.window();
	return Vector2i(window->pos().x(), window->pos().y());
}

void QtGfxDevice::setWindowPosition(int x, int y)
{
	QWidget *window = widget_.window();
	window->move(x, y);
}

void QtGfxDevice::setWindowSize(int width, int height)
{
	// change resolution only in case it is valid and it really changes
	if (width <= 0 || height <= 0 || (width == width_ && height == height_))
		return;

	if (theApplication().appConfiguration().window.resizable == false)
	{
		widget_.setMinimumSize(width, height);
		widget_.setMaximumSize(width, height);
	}

	QWidget *window = widget_.window();
	window->resize(width, height);
	// width and height are updated by the resize event that calls `resizeWindow()`
}

void QtGfxDevice::setWindowTitle(const char *windowTitle)
{
	QWidget *window = widget_.window();
	window->setWindowTitle(windowTitle);
}

void QtGfxDevice::setWindowIcon(const char *windowIconFilename)
{
	QWidget *window = widget_.window();
	window->setWindowIcon(QIcon(windowIconFilename));
}

void QtGfxDevice::flashWindow() const
{
	QApplication::alert(&widget_, 0);
}

unsigned int QtGfxDevice::primaryMonitorIndex() const
{
	QScreen *screen = QApplication::primaryScreen();

	const int retrievedIndex = retrieveMonitorIndex(screen);
	const unsigned int index = (retrievedIndex >= 0) ? static_cast<unsigned int>(retrievedIndex) : 0;
	return index;
}

unsigned int QtGfxDevice::windowMonitorIndex() const
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

const IGfxDevice::VideoMode &QtGfxDevice::currentVideoMode(unsigned int monitorIndex) const
{
	if (monitorIndex >= numMonitors_)
		monitorIndex = 0;

	return monitors_[monitorIndex].videoModes[0];
}

#ifdef WITH_GLEW
void QtGfxDevice::initGlew()
{
	const GLenum err = glewInit();
	FATAL_ASSERT_MSG_X(err == GLEW_OK, "GLEW error: %s", glewGetErrorString(err));

	glContextInfo_.debugContext = glContextInfo_.debugContext && glewIsSupported("GL_ARB_debug_output");
}
#endif

void QtGfxDevice::resetTextureBinding()
{
	GLTexture::bindHandle(GL_TEXTURE_2D, 0);
}

void QtGfxDevice::bindDefaultDrawFramebufferObject()
{
	const GLuint glHandle = widget_.defaultFramebufferObject();
	GLFramebufferObject::bindHandle(GL_DRAW_FRAMEBUFFER, glHandle);
}

/*! \note It should be used after each `QOpenGLWidget::makeCurrent()` call */
void QtGfxDevice::resetFramebufferObjectBinding()
{
	const GLuint glHandle = widget_.defaultFramebufferObject();
	GLFramebufferObject::setBoundHandle(GL_FRAMEBUFFER, glHandle);
}

void QtGfxDevice::resetBufferObjectBinding()
{
	GLBufferObject::bindHandle(GL_ARRAY_BUFFER, 0);
	GLBufferObject::bindHandle(GL_ELEMENT_ARRAY_BUFFER, 0);
	GLBufferObject::bindHandle(GL_UNIFORM_BUFFER, 0);
}

void QtGfxDevice::resetShaderProgramBinding()
{
	GLShaderProgram::setBoundProgram(0);
}

void QtGfxDevice::forceOpaqueAlpha()
{
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_TRUE);
	GLClearColor::setColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
}

/*! \note `IGfxDevice::update()` only caches the intended clear color for Qt instead of
 *  actually clearing. This performs the real clear at the start of the next frame. */
void QtGfxDevice::clearScreen()
{
#ifdef WITH_SCENEGRAPH
	if (theApplication().appConfiguration().features.scenegraph == false)
#endif
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void QtGfxDevice::initDevice(const WindowMode &windowMode)
{
	// At this point `updateMonitors()` has already been called by `initWindowScaling()`

	QSurfaceFormat format;
	format.setRedBufferSize(displayMode_.redBits());
	format.setGreenBufferSize(displayMode_.greenBits());
	format.setBlueBufferSize(displayMode_.blueBits());
	format.setAlphaBufferSize(displayMode_.alphaBits());
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

	if (width_ <= 0 || height_ <= 0 || isFullscreen_)
	{
		// Can't set the fullscreen window state in a method called by the constructor
		isFullscreen_ = true;
	}

	QWidget *window = widget_.window();
	const bool windowPositionIsValid = (containingMonitorIndex(windowMode) != -1);
	const bool ignoreBothWindowPosition = (windowMode.windowPositionX == AppConfiguration::Window::IgnorePosition &&
	                                       windowMode.windowPositionY == AppConfiguration::Window::IgnorePosition);
	if (isFullscreen_ == false && windowPositionIsValid && ignoreBothWindowPosition == false)
	{
		QPoint windowPos = window->pos();
		if (windowMode.windowPositionX != AppConfiguration::Window::IgnorePosition)
			windowPos.setX(windowMode.windowPositionX);
		if (windowMode.windowPositionY != AppConfiguration::Window::IgnorePosition)
			windowPos.setY(windowMode.windowPositionY);
		window->move(windowPos);
	}

	const int interval = displayMode_.hasVSync() ? 1 : 0;
	format.setSwapInterval(interval);

	widget_.setFormat(format);
	QSurfaceFormat::setDefaultFormat(format);

	initGLViewport();
}

void QtGfxDevice::updateMonitors()
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
			monitorNames_[i][j] = static_cast<char>(screens[i]->name().at(j).toLatin1());
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

int QtGfxDevice::retrieveMonitorIndex(QScreen *screen) const
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

void QtGfxDevice::setSize(int width, int height)
{
	width_ = width;
	height_ = height;
	QWidget *window = widget_.window();
	const qreal pixelRatio = window->devicePixelRatio();
	drawableWidth_ = width_ * pixelRatio;
	drawableHeight_ = height_ * pixelRatio;
}

}
