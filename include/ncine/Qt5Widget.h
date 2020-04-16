#ifndef CLASS_NCINE_QT5WIDGET
#define CLASS_NCINE_QT5WIDGET

#include "common_macros.h"
#include <QOpenGLWidget>

#ifdef _WIN32
	#undef ERROR
#endif

namespace ncine {

class PCApplication;
class IAppEventHandler;

}

class QWindow;

namespace ncine {

/// The Qt5 nCine widget
class DLL_PUBLIC Qt5Widget : public QOpenGLWidget
{
	Q_OBJECT

  public:
	Qt5Widget(nctl::UniquePtr<IAppEventHandler> (*createAppEventHandler)())
	    : Qt5Widget(nullptr, createAppEventHandler) {}
	Qt5Widget(QWidget *parent, nctl::UniquePtr<IAppEventHandler> (*createAppEventHandler)());
	~Qt5Widget();

	IAppEventHandler &appEventHandler();

  protected:
	bool event(QEvent *event) override;

  private:
	PCApplication &application_;
	nctl::UniquePtr<IAppEventHandler> (*createAppEventHandler_)();
	bool isInitialized_;

	void initializeGL() override;
	void resizeGL(int w, int h) override;
	void paintGL() override;

	QSize minimumSizeHint() const override;
	QSize sizeHint() const override;

	void shutdown();

	/// Deleted copy constructor
	Qt5Widget(const Qt5Widget &) = delete;
	/// Deleted assignment operator
	Qt5Widget &operator=(const Qt5Widget &) = delete;
};

}

#endif
