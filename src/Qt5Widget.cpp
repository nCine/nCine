#include "Qt5Widget.h"
#include "Qt5InputManager.h"
#include "PCApplication.h"
#include "IAppEventHandler.h"

#ifdef WITH_GLEW
	#include "Qt5GfxDevice.h"
#endif

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
	QObject::connect(this, SIGNAL(frameSwapped()), this, SLOT(autoUpdate()));

	ASSERT(createAppEventHandler_);
	application_.qt5Widget_ = this;
	application_.init(createAppEventHandler_, argc, argv);
	application_.setAutoSuspension(false);

	const int width = application_.appConfiguration().resolution.x;
	const int height = application_.appConfiguration().resolution.y;
	QRect rect = geometry();
	rect.setWidth(application_.appConfiguration().resolution.x);
	rect.setHeight(application_.appConfiguration().resolution.y);
	setGeometry(rect);

	if (application_.appCfg_.isResizable == false)
	{
		setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
		setMinimumSize(width, height);
		setMaximumSize(width, height);
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
				makeCurrent();
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
		gfxDevice.setResolution(w, h);
		gfxDevice.resetTextureBinding();
	}
}

void Qt5Widget::paintGL()
{
	if (isInitialized_)
	{
		if (application_.shouldQuit() == false)
			application_.run();
		else
		{
			shutdown();
			QCoreApplication::quit();
		}
	}
}

QSize Qt5Widget::minimumSizeHint() const
{
	if (application_.appConfiguration().isResizable == true)
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

void Qt5Widget::shutdown()
{
	if (isInitialized_)
	{
		application_.shutdownCommon();
		application_.qt5Widget_ = nullptr;
		isInitialized_ = false;
	}
	disconnect(SIGNAL(frameSwapped()));
}

}
