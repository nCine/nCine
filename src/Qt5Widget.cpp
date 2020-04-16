#include "Qt5Widget.h"
#include "Qt5InputManager.h"
#include "PCApplication.h"

#ifdef WITH_GLEW
	#include "Qt5GfxDevice.h"
#endif

#include <QCoreApplication>

namespace ncine {

///////////////////////////////////////////////////////////
// CONSTRUCTORS and DESTRUCTOR
///////////////////////////////////////////////////////////

Qt5Widget::Qt5Widget(QWidget *parent, nctl::UniquePtr<IAppEventHandler> (*createAppEventHandler)())
    : QOpenGLWidget(parent),
      application_(static_cast<PCApplication &>(theApplication())),
      createAppEventHandler_(createAppEventHandler), isInitialized_(false)
{
	application_.widget_ = this;
	setFocusPolicy(Qt::StrongFocus);
	setMouseTracking(true);
	setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	QObject::connect(this, SIGNAL(frameSwapped()), this, SLOT(update()));

	ASSERT(createAppEventHandler_);
	application_.init(createAppEventHandler_);
	application_.setAutoSuspension(false);
	const int width = application_.appConfiguration().resolution.x;
	const int height = application_.appConfiguration().resolution.y;
	setMinimumSize(width, height);
	setMaximumSize(width, height);
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

	switch (event->type())
	{
		case QEvent::FocusIn:
			application_.setFocus(true);
			return true;
		case QEvent::FocusOut:
			application_.setFocus(false);
			return true;
		case QEvent::KeyPress:
		case QEvent::KeyRelease:
		case QEvent::MouseButtonPress:
		case QEvent::MouseButtonRelease:
		case QEvent::MouseMove:
		case QEvent::TouchBegin:
		case QEvent::TouchUpdate:
		case QEvent::TouchEnd:
		case QEvent::Wheel:
			return inputManager ? inputManager->event(event) : false;
		case QEvent::Close:
		case QEvent::Quit:
			shutdown();
			return true;
		default:
			return QWidget::event(event);
	}
}

///////////////////////////////////////////////////////////
// PRIVATE FUNCTIONS
///////////////////////////////////////////////////////////

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
		application_.gfxDevice().setResolution(w, h);
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

void Qt5Widget::shutdown()
{
	if (isInitialized_)
	{
		application_.shutdownCommon();
		application_.widget_ = nullptr;
		isInitialized_ = false;
	}
}

}
