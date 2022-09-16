#include "Qt5Widget.h"
#include "Qt5InputManager.h"
#include "PCApplication.h"
#include "IAppEventHandler.h"

#ifdef WITH_GLEW
	#include "Qt5GfxDevice.h"
#endif

#include <QApplication>
#include <QCoreApplication>
#include <QResizeEvent>

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

Qt5Widget::Qt5Widget(QWidget *parent, nctl::UniquePtr<IAppEventHandler> (*createAppEventHandler)(), int argc, char **argv)
    : QOpenGLWidget(parent),
      application_(static_cast<PCApplication &>(theApplication())),
      createAppEventHandler_(createAppEventHandler),
      isInitialized_(false), shouldUpdate_(true)
{
	setFocusPolicy(Qt::StrongFocus);
	setMouseTracking(true);
	setAcceptDrops(true);
	connect(this, SIGNAL(frameSwapped()), this, SLOT(autoUpdate()));

	ASSERT(createAppEventHandler_);
	application_.qt5Widget_ = this;
	application_.init(createAppEventHandler_, argc, argv);
	application_.setAutoSuspension(false);

	// The graphics device is initialized and can react to resize events
	if (application_.gfxDevice().isFullScreen())
		window()->showFullScreen();
	else if (application_.appConfiguration().resizable == false)
	{
		const Vector2i &windowSize = application_.appConfiguration().resolution;
		setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		setMinimumSize(windowSize.x, windowSize.y);
		setMaximumSize(windowSize.x, windowSize.y);
	}
}

Qt5Widget::~Qt5Widget()
{
	shutdown();
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

IAppEventHandler &Qt5Widget::appEventHandler()
{
	return *application_.appEventHandler_;
}

/*! \note This custom version of the method also resets the FBO binding */
void Qt5Widget::makeCurrent()
{
	QOpenGLWidget::makeCurrent();
	Qt5GfxDevice &gfxDevice = static_cast<Qt5GfxDevice &>(*application_.gfxDevice_);
	gfxDevice.resetFramebufferObjectBinding();
}

///////////////////////////////////////////////////////////
// PROTECTED FUNCTIONS
///////////////////////////////////////////////////////////

bool Qt5Widget::event(QEvent *event)
{
	Qt5InputManager *inputManager = static_cast<Qt5InputManager *>(&application_.inputManager());

	if (event->type() == QEvent::FocusIn)
		application_.setFocus(true);
	else if (event->type() == QEvent::FocusOut)
		application_.setFocus(false);

	switch (event->type())
	{
		case QEvent::FocusIn:
		case QEvent::FocusOut:
		case QEvent::KeyPress:
		case QEvent::KeyRelease:
		case QEvent::MouseButtonPress:
		case QEvent::MouseButtonRelease:
		case QEvent::MouseMove:
		case QEvent::TouchBegin:
		case QEvent::TouchUpdate:
		case QEvent::TouchEnd:
		case QEvent::Wheel:
		case QEvent::DragEnter:
		case QEvent::Drop:
			if (inputManager)
			{
				if (inputManager->handler())
					makeCurrent();
				const bool result = inputManager->event(event);
				if (inputManager->handler())
					doneCurrent();
				return result;
			}
			return false;
		case QEvent::Resize:
		{
			const QSize size = static_cast<QResizeEvent *>(event)->size();
			if (size.width() != application_.widthInt() || size.height() != application_.heightInt())
			{
				Qt5GfxDevice &gfxDevice = static_cast<Qt5GfxDevice &>(*application_.gfxDevice_);

				makeCurrent();
				gfxDevice.setSize(size.width(), size.height());
				application_.resizeScreenViewport(size.width(), size.height());
				application_.appEventHandler_->onResizeWindow(size.width(), size.height());
				doneCurrent();
			}
			return QWidget::event(event);
		}
		case QEvent::Close:
		{
			const bool shouldQuit = inputManager ? inputManager->shouldQuitOnRequest() : true;
			if (shouldQuit)
			{
				makeCurrent();
				shutdown();
				doneCurrent();
			}
			else
				static_cast<QCloseEvent *>(event)->ignore();
			return true;
		}
		default:
			return QWidget::event(event);
	}
}

void Qt5Widget::initializeGL()
{
	Qt5GfxDevice &gfxDevice = static_cast<Qt5GfxDevice &>(*application_.gfxDevice_);

	connect(QApplication::instance(), SIGNAL(screenAdded(QScreen *)), this, SLOT(screenConfigurationChange(QScreen *)));
	connect(QApplication::instance(), SIGNAL(screenRemoved(QScreen *)), this, SLOT(screenConfigurationChange(QScreen *)));

#ifdef WITH_GLEW
	gfxDevice.initGlew();
#endif
	application_.initCommon();
	gfxDevice.resetTextureBinding();
	isInitialized_ = true;
}

void Qt5Widget::resizeGL(int w, int h)
{
	if (isInitialized_)
	{
		Qt5GfxDevice &gfxDevice = static_cast<Qt5GfxDevice &>(*application_.gfxDevice_);
		gfxDevice.setSize(w, h);
		application_.resizeScreenViewport(w, h);
		application_.appEventHandler_->onResizeWindow(w, h);
		gfxDevice.resetTextureBinding();
	}
}

void Qt5Widget::paintGL()
{
	// Avoid calling this method from the resize event
	static bool insidePaintEvent = false;

	if (isInitialized_ && insidePaintEvent == false)
	{
		insidePaintEvent = true;
		if (application_.shouldQuit() == false)
			application_.run();
		else
		{
			shutdown();
			QCoreApplication::quit();
		}
		insidePaintEvent = false;
	}
}

QSize Qt5Widget::minimumSizeHint() const
{
	if (application_.appConfiguration().resizable == true)
		return QSize(-1, -1);

	if (isInitialized_)
		return QSize(application_.widthInt(), application_.heightInt());
	else
		return QSize(application_.appCfg_.resolution.x, application_.appCfg_.resolution.y);
}

QSize Qt5Widget::sizeHint() const
{
	if (isInitialized_)
		return QSize(application_.widthInt(), application_.heightInt());
	else
		return QSize(application_.appCfg_.resolution.x, application_.appCfg_.resolution.y);
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void Qt5Widget::autoUpdate()
{
	if (shouldUpdate_)
		update();
}

void Qt5Widget::screenConfigurationChange(QScreen *screen)
{
	Qt5GfxDevice &gfxDevice = static_cast<Qt5GfxDevice &>(*application_.gfxDevice_);
	gfxDevice.updateMonitors();
}

void Qt5Widget::shutdown()
{
	if (isInitialized_)
	{
		application_.shutdownCommon();
		application_.qt5Widget_ = nullptr;
		isInitialized_ = false;
	}
	disconnect(QApplication::instance(), SIGNAL(screenRemoved(QScreen *)));
	disconnect(QApplication::instance(), SIGNAL(screenAdded(QScreen *)));
	disconnect(SIGNAL(frameSwapped()));
}

}
