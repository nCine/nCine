#include "QtWidget.h"
#include "QtGfxDevice.h"
#include "QtInputManager.h"
#include "PCApplication.h"
#include "IAppEventHandler.h"

#include <QApplication>
#include <QCoreApplication>
#include <QResizeEvent>
#include <QTimer>

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

QtWidget::QtWidget(QWidget *parent, nctl::UniquePtr<IAppEventHandler> (*createAppEventHandler)(), int argc, char **argv)
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
	application_.qtWidget_ = this;
	application_.init(createAppEventHandler_, argc, argv);
	application_.setAutoSuspension(false);

	// The graphics device is initialized and can react to resize events
	if (application_.gfxDevice().isFullscreen())
		window()->showFullScreen();
	else if (application_.appConfiguration().window.resizable == false)
	{
		const Vector2i &windowSize = application_.appConfiguration().window.resolution;
		setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		setMinimumSize(windowSize.x, windowSize.y);
		setMaximumSize(windowSize.x, windowSize.y);
	}
}

QtWidget::~QtWidget()
{
	shutdown();
}

///////////////////////////////////////////////////////////
// PUBLIC FUNCTIONS
///////////////////////////////////////////////////////////

IAppEventHandler &QtWidget::appEventHandler()
{
	return *application_.appEventHandler_;
}

/*! \note This custom version of the method also resets the FBO binding */
void QtWidget::makeCurrent()
{
	QOpenGLWidget::makeCurrent();
	QtGfxDevice &gfxDevice = static_cast<QtGfxDevice &>(*application_.gfxDevice_);
	gfxDevice.resetFramebufferObjectBinding();
	gfxDevice.resetBufferObjectBinding();
	gfxDevice.resetShaderProgramBinding();
}

///////////////////////////////////////////////////////////
// PROTECTED FUNCTIONS
///////////////////////////////////////////////////////////

bool QtWidget::event(QEvent *event)
{
	QtInputManager *inputManager = static_cast<QtInputManager *>(&application_.inputManager());

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
				QtGfxDevice &gfxDevice = static_cast<QtGfxDevice &>(*application_.gfxDevice_);

				makeCurrent();
				gfxDevice.setSize(size.width(), size.height());
				application_.resizeScreenViewport(size.width(), size.height());
				doneCurrent();
			}
			return QOpenGLWidget::event(event);
		}
		case QEvent::Close:
		{
			// This event might be sent again by a quitting Qt application,
			// in this case there is nothing left to do.
			if (isInitialized_ == false)
				return true;

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
			return QOpenGLWidget::event(event);
	}
}

void QtWidget::initializeGL()
{
	QtGfxDevice &gfxDevice = static_cast<QtGfxDevice &>(*application_.gfxDevice_);

	connect(QApplication::instance(), SIGNAL(screenAdded(QScreen *)), this, SLOT(screenConfigurationChange(QScreen *)));
	connect(QApplication::instance(), SIGNAL(screenRemoved(QScreen *)), this, SLOT(screenConfigurationChange(QScreen *)));

#ifdef WITH_GLEW
	gfxDevice.initGlew();
#endif
	application_.initCommon();
	gfxDevice.resetTextureBinding();
	isInitialized_ = true;
}

void QtWidget::resizeGL(int w, int h)
{
	if (isInitialized_)
	{
		QtGfxDevice &gfxDevice = static_cast<QtGfxDevice &>(*application_.gfxDevice_);
		gfxDevice.setSize(w, h);
		application_.resizeScreenViewport(w, h);
		gfxDevice.resetTextureBinding();
	}
}

void QtWidget::paintGL()
{
	// Avoid calling this method from the resize event
	static bool insidePaintEvent = false;

	if (isInitialized_ && insidePaintEvent == false)
	{
		insidePaintEvent = true;
		if (application_.shouldQuit() == false)
		{
			QtGfxDevice &gfxDevice = static_cast<QtGfxDevice &>(*application_.gfxDevice_);
			gfxDevice.resetBufferObjectBinding();
			gfxDevice.resetShaderProgramBinding();
			gfxDevice.clearScreen(); // clearing only if scenegraph is disabled
			application_.run();
			gfxDevice.forceOpaqueAlpha();
		}
		else
		{
			shutdown();
			// Quitting the application is deferred, so Qt can finish flushing this paint cycle
			QTimer::singleShot(0, QCoreApplication::instance(), &QCoreApplication::quit);
		}
		insidePaintEvent = false;
	}
}

QSize QtWidget::minimumSizeHint() const
{
	if (application_.appConfiguration().window.resizable == true)
		return QSize(-1, -1);

	if (isInitialized_)
		return QSize(application_.widthInt(), application_.heightInt());
	else
		return QSize(application_.appCfg_.window.resolution.x, application_.appCfg_.window.resolution.y);
}

QSize QtWidget::sizeHint() const
{
	if (isInitialized_)
		return QSize(application_.widthInt(), application_.heightInt());
	else
		return QSize(application_.appCfg_.window.resolution.x, application_.appCfg_.window.resolution.y);
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

void QtWidget::autoUpdate()
{
	if (shouldUpdate_)
		update();
}

void QtWidget::screenConfigurationChange(QScreen *screen)
{
	QtGfxDevice &gfxDevice = static_cast<QtGfxDevice &>(*application_.gfxDevice_);
	gfxDevice.updateMonitors();
}

void QtWidget::shutdown()
{
	if (isInitialized_)
	{
		application_.shutdownCommon();
		application_.qtWidget_ = nullptr;
		isInitialized_ = false;
	}
	disconnect(QApplication::instance(), SIGNAL(screenRemoved(QScreen *)));
	disconnect(QApplication::instance(), SIGNAL(screenAdded(QScreen *)));
	disconnect(SIGNAL(frameSwapped()));
}

}
